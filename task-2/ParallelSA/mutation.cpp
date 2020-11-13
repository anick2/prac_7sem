#include "mutation.h"

Mutation::Mutation(int nproc, int ntasks) : n(nproc), m(ntasks) {
    srand((unsigned) time(NULL) ^ (getpid()<<16));
}



Schedule Mutation::MutateSolution(Schedule& x) {

    std::vector<std::vector<int>> s = x.GetSolution();
    if (n == 1){
        return x;
    }

    int proc1, proc2;
    do {
        proc1 = rand() % n;
    } while (s[proc1].size() == 0);

    int task = rand() % s[proc1].size();

    do {
        proc2 = rand() % n;
    } while (proc1 == proc2);

    std::vector<int> durations = x.GetDurations();

    std::multimap<int, int> mmap;
    for (int i = 0; i < s[proc2].size(); i++) {
        mmap.insert({ durations[s[proc2][i] - 1], s[proc2][i] });
    }
    mmap.insert({durations[s[proc1][task] - 1], s[proc1][task]});

    s[proc2].clear();
    for(auto &x : mmap){
        s[proc2].push_back(x.second);
    }

    s[proc1].erase(s[proc1].begin() + task);

    Schedule NewSolution(n, durations, s);

    return NewSolution;
}