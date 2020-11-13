#pragma once

#include <vector>
#include "solution.h"
#include <unistd.h>

template<typename T>
class BaseMutation {
public:
    BaseMutation() = default;
    virtual T MutateSolution(T& x) = 0;
    virtual ~BaseMutation() = default;
};


class Mutation : public BaseMutation<Schedule> {
    int n, m;
public :
    Mutation(int nproc = 0, int ntasks = 0);
    Schedule MutateSolution(Schedule& x);
    virtual ~Mutation() = default;
};
