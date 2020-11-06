#include <string>
#include <cstring>
#include "solution.h"
#include "simulated_annealing.h"
#include "sockets.h"
#include <map>
#include <sys/wait.h>
#include <csignal>
#include <fstream>
#include <sstream>
#include <ctime>


#define SADDRESS "mysocket"
#define MAX_ITER_OUT 10
#define N 1

template<typename T, typename S, typename M>
void Processing(const int &n, const int &m, const std::vector<int> &duration, T &temp, S &sol, M &mut) {

    SimulatedAnnealing<T, S, M> p(temp, sol, mut);

    while (1) {
        int sockfd = CreateSocket(std::to_string(getpid()).c_str());

        Connect(sockfd, SADDRESS);

        p.Annealing();
        Schedule t = p.GetState();
        std::vector<std::vector<int>> sol = t.GetSolution();

        pid_t pid = getpid();
        write(sockfd, &pid, sizeof(pid));

        WriteSolution(sockfd, sol);
        close(sockfd);

        int sockfd1 = CreateSocket(std::to_string(getpid()).c_str());
        listen(sockfd1, 1);
        int fl;
        while (1) {
            int client_socket_fd = GetClientSockFd(sockfd);
            read(client_socket_fd, &fl, sizeof(int));
            if (fl == 1) {
                Schedule x = ReadSolution(client_socket_fd, n, m, duration);
                p.SetState(x);
            }
            close(client_socket_fd);
            break;
        }
        close(sockfd1);
        if (fl == 2) {
            break;
        }
    }
}

void SendMinEnergy(std::map<pid_t, Schedule> &solutions, Schedule sol, int fl) {

    int sockfd2 = CreateSocket("send");

    for (auto &x : solutions) {
        Connect(sockfd2, std::to_string(x.first).c_str());
        std::vector<std::vector<int>> v = sol.GetSolution();

        write(sockfd2, &fl, sizeof(int));
        if (fl == 1) {
            WriteSolution(sockfd2, v);
        }
    }
}

Schedule FindMinEnergy(std::map<pid_t, Schedule> &solutions) {
    Schedule s = solutions.begin()->second;
    int minEnergy = solutions.begin()->second.getEnergy();

    for (auto &x : solutions) {
        int candidateEnergy = x.second.getEnergy();
        if (candidateEnergy <= minEnergy) {
            minEnergy = candidateEnergy;
            s = x.second;
        }
    }
    return s;
}


Schedule Collect(const int &n, const int &m, const std::vector<int> &duration) {

    int sockfd2 = CreateSocket(SADDRESS);
    listen(sockfd2, N);
    Schedule minSolution = Schedule(n, duration), currSolution;
    int K_out = 0;

    while (1) {

        std::map<pid_t, Schedule> solutions = {};
        int client_socket_fd;

        while (1) {
            client_socket_fd = GetClientSockFd(sockfd2);

            pid_t pid;
            read(client_socket_fd, &pid, sizeof(pid));

            Schedule x = ReadSolution(client_socket_fd, n, m, duration);

            if (!solutions.count(pid)) {
                solutions[pid] = x;
            }
            if (solutions.size() == N) {
                close(client_socket_fd);
                break;
            }

            close(client_socket_fd);
        }
        currSolution = FindMinEnergy(solutions);

        if (currSolution.getEnergy() < minSolution.getEnergy()) {
            minSolution = currSolution;
            K_out = 0;
        } else {
            K_out++;
        }
        if (K_out > MAX_ITER_OUT) {
            SendMinEnergy(solutions, currSolution, 2);
            close(sockfd2);
            break;
        } else {
            SendMinEnergy(solutions, currSolution, 1);
        }
    }
    wait(NULL);
    return minSolution;
}


template<typename T, typename S, typename M>
Schedule
CreateProcceses(const int &NumProcesses, const int &n, const int &m, const std::vector<int> &duration, T &temp, S &sol,
                M &mut) {

    Schedule solution;

    std::vector<pid_t> pids;
    int collect = 1;

    for (int i = 0; i < NumProcesses; i++) {
        pid_t pid = fork();
        if (!pid) {
            collect = 0;
            break;
        } else {
            pids.push_back(pid);
        }
    }

    if (!collect) {
        Processing<T, S, M>(n, m, duration, temp, sol, mut);
    } else {
        solution = Collect(n, m, duration);
        for(auto& pid : pids){
            kill(pid, SIGKILL);
        }
        return solution;
    }
}


int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Enter the number of processes" << std::endl;
        return 1;
    }

    int numProcesses = std::atoi(argv[1]);

    std::ifstream fin("../tests.csv");

    int n, m, i = 1;
    std::vector<int> durations;

    std::string temp, line;

    while(fin >> line){
        std::cout << "Test " << i << std::endl;
        std::stringstream s(line);

        getline(s, temp, ',');
        n = std::stoi(temp);
        std::cout << "Number of processors: " << n << std::endl;

        getline(s, temp, ',');
        m = std::stoi(temp);
        std::cout << "Number of tasks: " << m << std::endl;

        std::cout << "Durations: ";
        durations.clear();
        for(int j = 0; j < m; j++){
            getline(s, temp, ',');
            durations.push_back(std::stoi(temp));
            std::cout << std::stoi(temp) << ' ';
        }
        std::cout << std::endl;

        srand((unsigned) time(NULL));

        BoltzmannLaw temperature(rand() * 0.00001);
        Schedule schedule(n, durations);
        Mutation mutation(n, m);

        Schedule solution;
        auto begin = std::chrono::steady_clock::now();
        solution = CreateProcceses<BoltzmannLaw, Schedule, Mutation>(numProcesses, n, m, durations, temperature, schedule, mutation);
        auto end = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        std::cout << "Work time " << elapsed_ms.count() << std::endl;
        wait(NULL);
        std::cout << "Solution:" << std::endl;
        std::vector<std::vector<int>> vec = solution.GetSolution();
        //PrintVec(vec, i);
        std::cout << "Total waiting time = " << solution.getEnergy() << std::endl;
        i++;
        std::cout << std::endl << std::endl;
    }
    return 0;
}
