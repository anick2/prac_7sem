#include "solution.h"
#include "mutation.h"
#include "sockets.h"

Schedule::Schedule(int nproc, std::vector<int> d, const std::vector<std::vector<int>> &sol) : n(nproc),
                                                                                              duration(d) {
    if (sol.size() == 0) {
        std::vector<int> p;
        std::multimap<int, int> mmap;
        for (int i = 0; i < d.size(); i++) {
            mmap.insert({d[i], i + 1});
        }
        for (auto &x : mmap) {
            p.push_back(x.second);
        }
        sch.push_back(p);
        sch.resize(nproc);
    } else {
        SetSolution(sol);
    }
}

void Schedule::SetSolution(const std::vector<std::vector<int>> &x) {
    sch = x;
}

uint64_t Schedule::getEnergy() {
    uint64_t sum = 0;
    for (auto &x : sch) {
        uint64_t accum = 0;
        for (auto &y : x) {
            __builtin_add_overflow(accum, duration[y - 1], &accum);
            //accum += accum + duration[y - 1];
        }
         if (__builtin_add_overflow(sum, accum, &sum)){
             std::cout << 'Overflow' << std::endl;
         }
        //sum += accum;
    }
    return sum;
}

std::vector<std::vector<int>> Schedule::GetSolution() {
    return sch;
}

std::vector<int> Schedule::GetDurations() {
    return duration;
}