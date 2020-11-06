#pragma once

#include "temperature.h"
#include "mutation.h"
#include "solution.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include "sockets.h"

#define MAX_ITER_IN 100
#define K 10

template <typename T, typename S, typename M>
class SimulatedAnnealing {
    T temperature;
    S solution;
    M mutation;
public:
    SimulatedAnnealing(T& t = T(), S& s = S(), M& m = M());
    static bool MakeTransit(double p);
    void Annealing();
    void SetState(const Schedule& x);
    Schedule GetState();
};

template<typename T, typename S, typename M>
SimulatedAnnealing<T, S, M>::SimulatedAnnealing(T& t, S& s, M& m) : temperature(t), solution(s), mutation(m) {
}

template<typename T, typename S, typename M>
bool SimulatedAnnealing<T, S, M>::MakeTransit(double p) {
    double probability = (double) (rand()) / RAND_MAX;
    return probability <= p;
}

template<typename T, typename S, typename M>
void SimulatedAnnealing<T, S, M>::Annealing() {

    double currTemp = temperature.getT();
    S currSolution = solution;
    double currEnergy = solution.getEnergy();

    int iteration = 0;
    int K_in = 0;
    while(K_in <= MAX_ITER_IN) {
        for(int i = 0; i < K; i++){
            S tempSolution = mutation.MutateSolution(currSolution);
            double tempEnergy = tempSolution.getEnergy();

            if(tempEnergy < solution.getEnergy()){
                solution = tempSolution;
                iteration = 0;
            }

            double dF = tempEnergy - currEnergy;

            if (dF <= 0){
                currSolution = tempSolution;
                currEnergy = tempEnergy;
            } else {
                double p = exp(-dF / currTemp);
                if (MakeTransit(p)) {
                    currSolution = tempSolution;
                    currEnergy = tempEnergy;
                }
            }
        }

        iteration++;
        K_in++;

        currTemp = temperature.DropT(iteration);
    }
    std::vector<std::vector<int>> v = solution.GetSolution();
}

template<typename T, typename S, typename M>
Schedule SimulatedAnnealing<T, S, M>::GetState() {
    return solution;
}

template<typename T, typename S, typename M>
void SimulatedAnnealing<T, S, M>::SetState(const Schedule& x){
    solution = x;
}