#pragma once

#include <cmath>

class TemperatureLaw {
protected:
    double T0;
public:
    TemperatureLaw() = default;
    explicit TemperatureLaw(double t = 0);
    double getT() ;
    virtual double DropT(int i) = 0;
    virtual ~TemperatureLaw() = default;
};

// Boltzmann Law
class BoltzmannLaw : public TemperatureLaw {
public:
    BoltzmannLaw() = default;
    explicit BoltzmannLaw(double t );
    double DropT(int i) override;
    virtual ~BoltzmannLaw() = default;
};

// Cauchy Law
class CauchyLaw : public TemperatureLaw  {
public:
    CauchyLaw() = default;
    explicit CauchyLaw(double t);
    double DropT(int i) override;
    virtual ~CauchyLaw() = default;
};

// Base Law
class BaseLaw : public TemperatureLaw {
public:
    BaseLaw() = default;
    explicit BaseLaw(double t);
    double DropT(int i) override;
    virtual ~BaseLaw() = default;
};
