#pragma once

#include <vector>
#include <numeric>
#include <map>
#include <iostream>

class Solution {
public:
    Solution() = default;
    virtual double getEnergy() = 0;
    virtual ~Solution() = default;
};

class Schedule : public Solution {
    int n;
    std::vector<int> duration;
    std::vector<std::vector<int>> sch;
public:
    Schedule(int nproc = 0, std::vector<int> d = {}, const std::vector<std::vector<int>>& sol = {});
    void SetSolution(const std::vector<std::vector<int>>& x);
    double getEnergy() override;
    std::vector<std::vector<int>> GetSolution();
    std::vector<int> GetDurations();
    virtual ~Schedule() = default;
};

