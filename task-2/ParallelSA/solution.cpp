#include "solution.h"
#include "mutation.h"
#include "sockets.h"

Schedule::Schedule(int nproc, std::vector<int> d, const std::vector<std::vector<int>> &sol) : n(nproc),
                                                                                              duration(d) {
    if (sol.size() == 0) {
        if (nproc == 0){
            return;
        }
        std::vector<int> p;
        std::multimap<int, int> mmap;
        for (int i = 0; i < d.size(); i++) {
            mmap.insert({d[i], i + 1});
        }
        for (auto &x : mmap) {
            p.push_back(x.second);
        }
        sch.resize(nproc);
        int j = d.size() / nproc;
        int size = d.size();
        for (int i = 0; i < nproc; i++) {
            sch[i].insert(sch[i].end(), p.begin() + i * j, p.begin() + (i + 1) * j);
            size -= j;
        }
        if (size) {
            auto it = p.end() - size;
            int k = 0;
            while (it != p.end()) {
                sch[k].insert(sch[k].end(), *it);
                k = (k + 1) % nproc;
                it++;
            }
        }
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
            accum += duration[y - 1];
            sum += accum;
        }
    }
    return sum;
}

std::vector<std::vector<int>> Schedule::GetSolution() {
    return sch;
}

std::vector<int> Schedule::GetDurations() {
    return duration;
}