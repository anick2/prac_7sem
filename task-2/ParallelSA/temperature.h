#pragma once

#include <cmath>

class TemperatureLaw {
protected:
    double T0;
public:
    TemperatureLaw() = default;
    explicit TemperatureLaw(double t = 0);
    double getT() ;
    double setT(double t);
    virtual double DropT(int i) = 0;
    virtual ~TemperatureLaw() = default;
};

// Boltzmann Law
class BoltzmannLaw : public TemperatureLaw {
public:
    explicit BoltzmannLaw(double t = 0);
    double DropT(int i) override;
    virtual ~BoltzmannLaw() = default;
};

// Cauchy Law
class CauchyLaw : public TemperatureLaw  {
public:
    explicit CauchyLaw(double t = 0);
    double DropT(int i) override;
    virtual ~CauchyLaw() = default;
};

// Base Law
class BaseLaw : public TemperatureLaw {
public:
    explicit BaseLaw(double t = 0);
    double DropT(int i) override;
    virtual ~BaseLaw() = default;
};
