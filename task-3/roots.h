#pragma once

#include "functions.h"

double getRoot(TFunction &f, int it_num = 1000, double x = -1) {
    double val = f(x);
    double step = 0.1;
    for (int i = 1; i <= it_num && std::fabs(val) > eps; i++) {
        x = (val > 0) ? x + step * -f.GetDeriv(x) : x + step * f.GetDeriv(x);
        val = f(x);
    }
    return x;
}