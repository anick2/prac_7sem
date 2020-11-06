#include <iostream>
#include <fstream>


void createCSVFile(int n, int m, int begin, int end) {
    std::fstream fout;
    fout.open("tests.csv", std::ios::out | std::ios::app);

    //fout << "num_of_proc, num_of_tasks, durations[num_of_tasks]" << std::endl;

    srand((unsigned) time(NULL));

    fout << n << ',' << m;
    for (int i = 0; i < m; i++) {
        fout << ',';
        fout << begin + rand() % (end - begin + 1);
    }
    fout << std::endl;
}

int main(int argc, char *argv[]){
    int numProc = std::atoi(argv[1]);
    int numTasks = std::atoi(argv[2]);
    int Begin = std::atoi(argv[3]);
    int End = std::atoi(argv[4]);
    createCSVFile(numProc, numTasks, Begin, End);
}