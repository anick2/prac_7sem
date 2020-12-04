#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
#pragma once

#include <utility>
#include <vector>
#include <string>
#include <cmath>

#define eps 1e-5

class TFunction : public std::enable_shared_from_this<TFunction> {
    std::vector<TFunction *> func;
    std::vector<std::string> operations;
public:
    TFunction() = default;

    TFunction(std::vector<TFunction *> f, std::vector<std::string> op) : func(f), operations(op) {}

    virtual std::string ToString() const;

    virtual double operator()(double x);

    virtual double GetDeriv(double x);

    friend std::shared_ptr<TFunction> operator+(TFunction &f1, TFunction &f2);

    friend std::shared_ptr<TFunction> operator-(TFunction &f1, TFunction &f2);

    friend std::shared_ptr<TFunction> operator*(TFunction &f1, TFunction &f2);

    friend std::shared_ptr<TFunction> operator/(TFunction &f1, TFunction &f2);

};

class TIdent : public TFunction {
public:
    TIdent(double x = 0) {}

    std::string ToString() const override;

    double operator()(double x) override;

    double GetDeriv(double x) override;
};


class TConst : public TFunction {
    int val;
public:
    explicit TConst(const int &value = 0) : val(value) {}

    std::string ToString() const override;

    double operator()(double x) override;

    double GetDeriv(double x) override;
};

class TPower : public TFunction {
    int power;
public:
    TPower(int p = 0) : power(p) {}

    std::string ToString() const override;

    double operator()(double x) override;

    double GetDeriv(double x) override;
};

class TExp : public TFunction {
    double power;
public:
    TExp(double p = 0) : power(p) {}

    std::string ToString() const override;

    double operator()(double x) override;

    double GetDeriv(double x) override;
};

class TPolynomial : public TFunction {
    std::vector<int> powers;
public:
    TPolynomial(std::vector<int> ps = {}) : powers(std::move(ps)) {
    }

    std::string ToString() const override;

    double operator()(double x) override;

    double GetDeriv(double x) override;
};

// ToString

std::string TIdent::ToString() const {
    return "x";
}

std::string TConst::ToString() const {
    return std::to_string(val);
}

std::string TPower::ToString() const {
    return "x^" + std::to_string(power);
}

std::string TExp::ToString() const {
    return "e^x";
}

std::string TPolynomial::ToString() const {
    std::string res = std::to_string(powers[0]);
    for (int i = 1; i < powers.size(); i++) {
        if (!powers[i]) continue;
        auto tmp = powers[i];
        if (powers[i] > 0) {
            res += " + ";
        } else {
            res += " - ";
            tmp *= -1;
        }
        res += std::to_string(tmp) + "*x";
        if (i != 1) res += "^" + std::to_string(i);
    }
    return res;
}

//  ()

double TIdent::operator()(double x) {
    return x;
}

double TConst::operator()(double x) {
    return val;
}

double TPower::operator()(double x) {
    return pow(x, power);
}

double TExp::operator()(double x) {
    return exp(x);
}

double TPolynomial::operator()(double x) {
    double res = powers[0];
    for (int i = 1; i < powers.size(); i++) {
        res += powers[i] * pow(x, i);
    }
    return res;
}


// GetDeriv

double TIdent::GetDeriv(double x) {
    return 1;
}

double TConst::GetDeriv(double x) {
    return 0;
}

double TPower::GetDeriv(double x) {
    return power * pow(x, power - 1);
}

double TExp::GetDeriv(double x) {
    return exp(x);
}

double TPolynomial::GetDeriv(double x) {
    double res = 0;
    for (int i = 1; i < powers.size(); i++) {
        res += powers[i] * i * pow(x, i - 1);
    }
    return res;
}

std::shared_ptr<TFunction> operator+(TFunction &f1, TFunction &f2) {
    std::vector<TFunction *> tmpFunc;
    std::vector<std::string> tmpOp;
    tmpFunc.push_back(&f1);
    tmpFunc.push_back(&f2);
    tmpOp.push_back("+");
    return std::make_shared<TFunction>(tmpFunc, tmpOp);
}

std::shared_ptr<TFunction> operator-(TFunction &f1, TFunction &f2) {
    std::vector<TFunction *> tmpFunc;
    std::vector<std::string> tmpOp;
    tmpFunc.push_back(&f1);
    tmpFunc.push_back(&f2);
    tmpOp.push_back("-");
    return std::make_shared<TFunction>(tmpFunc, tmpOp);
}

std::shared_ptr<TFunction> operator*(TFunction &f1, TFunction &f2) {
    std::vector<TFunction *> tmpFunc;
    std::vector<std::string> tmpOp;
    tmpFunc.push_back(&f1);
    tmpFunc.push_back(&f2);
    tmpOp.push_back("*");
    return std::make_shared<TFunction>(tmpFunc, tmpOp);
}

std::shared_ptr<TFunction> operator/(TFunction &f1, TFunction &f2) {
    std::vector<TFunction *> tmpFunc;
    std::vector<std::string> tmpOp;
    tmpFunc.push_back(&f1);
    tmpFunc.push_back(&f2);
    tmpOp.push_back("/");
    return std::make_shared<TFunction>(tmpFunc, tmpOp);
}

std::string TFunction::ToString() const {
    std::string res;
    if (operations[0] == "+" || typeid(*func[0]) == typeid(TIdent) ||
        typeid(*func[0]) == typeid(TConst) || typeid(*func[0]) == typeid(TExp)) {
        res += (*func[0]).ToString();
    } else {
        res += "(" + (*func[0]).ToString() + ")";
    }
    for (int i = 1; i < func.size(); i++) {
        res += ' ' + operations[i - 1] + ' ';
        if (operations[i - 1] == "+" || typeid(*func[i]) == typeid(TIdent) ||
            typeid(*func[i]) == typeid(TConst) || typeid(*func[i]) == typeid(TExp)) {
            res += (*func[i]).ToString();
        } else {
            res += "(" + (*func[i]).ToString() + ")";
        }
    }
    return res;
}

double TFunction::operator()(double x) {
    double val = (*func[0])(x);
    for (int i = 0; i < func.size(); i++) {
        if (operations[i - 1] == "+") {
            val += (*func[i])(x);
        } else if (operations[i - 1] == "-") {
            val -= (*func[i])(x);
        } else if (operations[i - 1] == "*") {
            val *= (*func[i])(x);
        } else if (operations[i - 1] == "/") {
            val /= (*func[i])(x);
        }
    }
    return val;
}

double TFunction::GetDeriv(double x) {
    double val = func[0]->GetDeriv(x);
    double k = (*func[0])(x);
    for (int i = 0; i < func.size(); i++) {
        if (operations[i - 1] == "+") {
            val += func[i]->GetDeriv(x);
            k += (*func[i])(x);
        } else if (operations[i - 1] == "-") {
            val -= func[i]->GetDeriv(x);
            k -= (*func[i])(x);
        } else if (operations[i - 1] == "*") {
            val = k * func[i]->GetDeriv(x) + val * (*func[i])(x);
            k *= (*func[i])(x);
        } else if (operations[i - 1] == "/") {
            val = (val * (*func[i])(x) - k * func[i]->GetDeriv(x)) / pow((*func[i])(x), 2);
            k /= (*func[i])(x);
        }
    }
    return val;
}

template<typename T>
std::enable_if_t<!std::is_base_of<TFunction, std::decay_t<T>>::value, TFunction>
operator+ (T&& f, TFunction&& g){
    throw std::logic_error("bad argument");
}

template<typename T>
std::enable_if_t<!std::is_base_of<TFunction, std::decay_t<T>>::value, TFunction>
operator- (T&& f, TFunction&& g){
    throw std::logic_error("bad argument");
}

template<typename T>
std::enable_if_t<!std::is_base_of<TFunction, std::decay_t<T>>::value, TFunction>
operator* (T&& f, TFunction&& g){
    throw std::logic_error("bad argument");
}

template<typename T>
std::enable_if_t<!std::is_base_of<TFunction, std::decay_t<T>>::value, TFunction>
operator/ (T&& f, TFunction&& g){
    throw std::logic_error("bad argument");
}

template<typename T>
std::enable_if_t<!std::is_base_of<TFunction, std::decay_t<T>>::value, TFunction>
operator+ (TFunction& g, T&& f){
    throw std::logic_error("bad argument");
}

template<typename T>
std::enable_if_t<!std::is_base_of<TFunction, std::decay_t<T>>::value, TFunction>
operator- (TFunction& g, T&& f){
    throw std::logic_error("bad argument");
}

template<typename T>
std::enable_if_t<!std::is_base_of<TFunction, std::decay_t<T>>::value, TFunction>
operator* (TFunction& g, T&& f){
    throw std::logic_error("bad argument");
}

template<typename T>
std::enable_if_t<!std::is_base_of<TFunction, std::decay_t<T>>::value, TFunction>
operator/ (TFunction& g, T&& f){
    throw std::logic_error("bad argument");
}




#pragma clang diagnostic pop