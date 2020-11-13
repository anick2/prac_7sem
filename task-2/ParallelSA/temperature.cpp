#include "temperature.h"

TemperatureLaw::TemperatureLaw(double t) : T0(t) {};

double TemperatureLaw::getT() {
    return T0;
}

double TemperatureLaw::setT(double t) {
    T0 = t;
}

BoltzmannLaw::BoltzmannLaw(double t): TemperatureLaw(t) {}

double BoltzmannLaw::DropT(int i) {
    return T0 / log(i + 1);
}


CauchyLaw::CauchyLaw(double t): TemperatureLaw(t) {}

double CauchyLaw::DropT(int i) {
    return T0 / (i + 1);
}


BaseLaw::BaseLaw(double t): TemperatureLaw(t) {}

double BaseLaw::DropT(int i) {
    return T0  * log(i + 1) / (i + 1);
}
