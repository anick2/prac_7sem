#pragma once

#include "field.h"
#include <numeric>

#define L 2500
#define MAX_ITER 20
#define N_POP 100
#define ITER 100


struct Chromosome {

    std::vector<int> x;

    Chromosome() = default;

    Chromosome(const Field& f);

    Field GetField();
};

using Population = std::vector<Chromosome>;

class SurvivorFunc {
public:
    static int GetValue(Chromosome x);
};

//рулеточная селекция
class Selection {
public:
    static Population Run(Population pop, std::vector<int> &f);
};

//двухточечное скрещивание
class Hybridization {
    double Pc = 0.8;
public:
    Population Run(Population &pops) const;

    static void Hybrid(Chromosome &parent1, Chromosome &parent2, Chromosome &child1, Chromosome &child2);
};

class Mutation {
    double Pmut;
public:
    Mutation(double p = 1. / L) : Pmut(p) {};

    Population Run(Population &pop) const;
};


template<typename S, typename H, typename M, typename F>
class GeneticAlgo {
    Chromosome bestSolution;
    int bestSolutionValue;
    Population pop;
    std::vector<int> fs;

    S selection;
    H hybridization;
    M mutation;
    F survivor;
public:

    GeneticAlgo() = default;

    GeneticAlgo(S &s, H &h, M &m, F &f);

    void FineSolution();

    void Run();

    Field GetSolution();

    int GetSolutionValue();
};


Chromosome::Chromosome(const Field& f) {
    for (auto &line : f.FieldConf()) {
        x.insert(x.end(), line.begin(), line.end());
    }
}

Field Chromosome::GetField() {
    Configuration conf;
    for (int i = 0; i < SIZE; i++) {
        conf.push_back({x.begin() + SIZE * i, x.begin() + (i + 1) * SIZE});
    }
    return Field(conf);
}

int SurvivorFunc::GetValue(Chromosome x) {
    Field tmp = x.GetField();
    for (int i = 0; i <= ITER; i++) {
        tmp.Step();
    }
    return (x.GetField() != tmp) ? tmp.NumFilled() : -1;
}

Population Selection::Run(Population pop, std::vector<int> &f) {
    std::vector<double> roulette(N_POP);
    int sum = accumulate(f.begin(), f.end(), 0);
    for (int i = 0; i < N_POP; i++) {
        if (!i) {
            roulette[i] = 2 * M_PI * ((double) f[i] / sum);
        } else {
            roulette[i] = 2 * M_PI * ((double) f[i] / sum) + roulette[i - 1];
        }
    }

    Population npop;
    for (int i = 0; i < N_POP; i++) {
        double a = (double) rand() / (RAND_MAX) * (2 * M_PI);
        auto it = std::lower_bound(roulette.begin(), roulette.end(), a);
        int k = it - roulette.begin();
        npop.push_back(pop[k]);
    }
    return npop;
}

Population Hybridization::Run(Population &pops) const {
    Population npops;
    int k = 0;
    while (k < N_POP) {
        double i = rand() % N_POP;
        double j = rand() % N_POP;
        double p = (double) rand() / RAND_MAX;
        if (Pc > p) {
            Chromosome c1, c2;
            Hybrid(pops[i], pops[j], c1, c2);
            k += 2;
            npops.push_back(c1);
            npops.push_back(c2);
        }
    }
    return npops;
}

void Hybridization::Hybrid(Chromosome &parent1, Chromosome &parent2, Chromosome &child1, Chromosome &child2) {
    int i = rand() % L, j;
    child1 = parent1;
    child2 = parent2;
    do {
        j = rand() % L;
    } while (j == i);
    if (i > j) {
        std::swap(i, j);
    }
    for (int k = i; k < j; k++) {
        std::iter_swap(child1.x.begin() + k, child2.x.begin() + k);
    }
}

Population Mutation::Run(Population &pop) const {
    for (auto &chrom : pop) {
        for (int i = 0; i < L; i++) {
            double p = (double) rand() / RAND_MAX;
            if (Pmut > p) {
                chrom.x[i] = !chrom.x[i];
            }
        }
    }
    return pop;
}

template<typename S, typename H, typename M, typename F>
void GeneticAlgo<S, H, M, F>::FineSolution() {
    int n = std::count_if(fs.begin(), fs.end(), [](int i) { return i == -1; });
    if (n == 0) return;
    else {
        int fine = *std::upper_bound(fs.begin(), fs.end(), -1);
        fine = (fine) ? fine - 1 : 0;
        for (int i = 0; i < n; i++) {
            auto it = std::find_if(fs.begin(), fs.end(), [](int i) { return i == -1; });
            *it = fine;
        }
    }
}

template<typename S, typename H, typename M, typename F>
GeneticAlgo<S, H, M, F>::GeneticAlgo(S &s, H &h, M &m, F &f) : selection(s), hybridization(h), mutation(m),
                                                               survivor(f) {
    Field field;
    bestSolution = Chromosome(field);
    bestSolutionValue = survivor.GetValue(field);
    fs.push_back(survivor.GetValue(field));
    pop.push_back(Chromosome(field));

    for (int i = 1; i < N_POP; i++) {
        Field field;
        Chromosome chr(field);
        int value = survivor.GetValue(field);
        if (value > bestSolutionValue) {
            bestSolution = chr;
            bestSolutionValue = value;
        }

        fs.push_back(value);
        pop.push_back(chr);
    }
    FineSolution();
}

template<typename S, typename H, typename M, typename F>
void GeneticAlgo<S, H, M, F>::Run() {
    int badIt = 0;
    while (badIt < MAX_ITER) {
        auto pop_selection = selection.Run(pop, fs);
        auto pop_hybrid = hybridization.Run(pop_selection);
        auto pop_mutation = mutation.Run(pop_hybrid);

        int fl = 0;
        fs.clear();
        for (auto &elem : pop_mutation) {
            Field field = elem.GetField();
            int value = survivor.GetValue(field);
            fs.push_back(value);

            if (value > bestSolutionValue) {
                bestSolution = elem;
                bestSolutionValue = value;
                badIt = 0;
                fl = 1;
            }
        }

        if (!fl) {
            badIt++;
        }

        pop = pop_mutation;
        FineSolution();
    }
};

template<typename S, typename H, typename M, typename F>
Field GeneticAlgo<S, H, M, F>::GetSolution() {
    return bestSolution.GetField();
}

template<typename S, typename H, typename M, typename F>
int GeneticAlgo<S, H, M, F>::GetSolutionValue() {
    return bestSolutionValue;
}