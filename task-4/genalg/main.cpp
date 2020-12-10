#include <iostream>
#include "genalgo.h"
#include <cmath>
#include <chrono>
#include <thread>

void Planer() {
    Configuration z;
    for(int i = 0; i < SIZE; i++){
        std::vector<int> line(SIZE);
        z.push_back(line);
    }
    z[1][2] = 1;
    z[2][3] = 1;
    z[3][1] = 1;
    z[3][2] = 1;
    z[3][3] = 1;
    Field f(z);
    for (int i = 0; i <= ITER; i++) {
        f.Step();
        f.PrintFormatField();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        system("clear");
    }
}

int main(int argc, char **argv) {

    srand(time(NULL));

    SurvivorFunc func;
    Selection selection;
    Hybridization hybridization;

    int series, run;

    if (argc > 1) {
        series = std::atoi(argv[1]);
        run = std::atoi(argv[2]);
    } else {
        return 1;
    }

    if (series == -2) {
        Planer();
        return 0;
    }

    double initPmut = 1. / L;

    double Pmut = initPmut * std::powf(1.5, series);
    Mutation mutation(Pmut);

    GeneticAlgo<Selection, Hybridization, Mutation, SurvivorFunc> alg(selection, hybridization, mutation, func);
    alg.Run();
    auto sol = alg.GetSolution();

    if (run == -1) {

        for (int i = 0; i <= ITER; i++) {
            sol.Step();
            sol.PrintFormatField();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            system("clear");
        }

    } else {

        std::ofstream fout0;
        fout0.open("../results/series" + std::to_string(series) + "/series_" + std::to_string(series) + "_run_" +
                   std::to_string(run) + ".txt", std::ios::trunc);
        sol.PrintFormatField(fout0);
        fout0.close();

        std::ofstream fout1;
        fout1.open("../results/series" + std::to_string(series) + "/series_" + std::to_string(series) + "_run_" +
                   std::to_string(run) + "_sol_after100.txt", std::ios::trunc);
        for (int i = 0; i <= ITER; i++) {
            sol.Step();
        }
        sol.PrintFormatField(fout1);
        fout1.close();

        std::ofstream fout2;
        fout2.open("../results/series" + std::to_string(series) + ".csv", std::ios_base::app);
        fout2 << alg.GetSolutionValue() << ',';
        fout2.close();
        std::cout << "DONE" << std::endl;
    }

    return 0;
}
