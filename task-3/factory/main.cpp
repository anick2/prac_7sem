#include <iostream>
#include "factory.h"
#include "roots.h"
#include "gtest/gtest.h"


TEST(FactoryTest, ToStringBase) {
    TFactory funcFactory;

    auto identFunc = funcFactory.CreateObject("ident"); //IdentFunc x
    auto constFunc = funcFactory.CreateObject("const", 5); // ConstFunc 5
    auto powerFunc = funcFactory.CreateObject("power", 2); // PowerFunc x^2
    auto polynomFunc = funcFactory.CreateObject("polynomial", {6, -2, 13, 14}); // TPolynomial 6 - 2*x + 13*x^2 + 14*x^3
    auto expFunc = funcFactory.CreateObject("exp"); // TExp e^x

    EXPECT_EQ(identFunc->ToString(), "x");
    EXPECT_EQ(constFunc->ToString(), "5");
    EXPECT_EQ(powerFunc->ToString(), "x^2");
    EXPECT_EQ(polynomFunc->ToString(), "6 - 2*x + 13*x^2 + 14*x^3");
    EXPECT_EQ(expFunc->ToString(), "e^x");
}

TEST(FunctionTest, ToStringComplex) {
    TFactory funcFactory;

    auto identFunc = funcFactory.CreateObject("ident"); //IdentFunc x
    auto polynomFunc = funcFactory.CreateObject("polynomial", {5, 3, 100, -2}); // TPolynomial 6 - 2*x + 13*x^2 + 14*x^3
    auto expFunc = funcFactory.CreateObject("exp"); // TExp e^x

    auto complexFunc1 = *expFunc + *polynomFunc; // e^x + 5 + 3*x + 100*x^2 - 2*x^3
    auto complexFunc2 = *identFunc / *polynomFunc; // x / (5 + 3*x + 100*x^2 - 2*x^3)
    auto complexFunc3 = *expFunc * *complexFunc2; // e^x * (x / (5 + 3*x + 100*x^2 - 2*x^3))

    EXPECT_EQ(complexFunc1->ToString(), "e^x + 5 + 3*x + 100*x^2 - 2*x^3");
    EXPECT_EQ(complexFunc2->ToString(), "x / (5 + 3*x + 100*x^2 - 2*x^3)");
    EXPECT_EQ(complexFunc3->ToString(), "e^x * (x / (5 + 3*x + 100*x^2 - 2*x^3))");
}

TEST(FunctionTest, FuncValues) {
    TFactory funcFactory;

    auto identFunc = funcFactory.CreateObject("ident"); //IdentFunc x
    auto constFunc = funcFactory.CreateObject("const", 12); // ConstFunc 12
    auto powerFunc = funcFactory.CreateObject("power", 3); // PowerFunc x^3
    auto polynomFunc = funcFactory.CreateObject("polynomial", {1, 3, -2}); // TPolynomial 1 + 3*x - 2*x^2
    auto expFunc = funcFactory.CreateObject("exp"); // TExp e^x

    EXPECT_EQ((*identFunc)(124), 124);
    EXPECT_EQ((*constFunc)(40), 12); // const = 12
    EXPECT_EQ((*powerFunc)(2), 8); // (2)^3 = 8
    EXPECT_EQ((*polynomFunc)(2), -1); // 1 + 3*(2) - 2*(2)^2 = -1
    EXPECT_EQ((*expFunc)(3), exp(3)); //exp(3)
}

TEST(FunctionTest, FuncValuesComplex) {
    TFactory funcFactory;

    auto identFunc = funcFactory.CreateObject("ident"); //IdentFunc x
    auto polynomFunc = funcFactory.CreateObject("polynomial", {5, 3, 100, -2}); // TPolynomial 6 - 2*x + 13*x^2 + 14*x^3
    auto expFunc = funcFactory.CreateObject("exp"); // TExp e^x

    auto complexFunc1 = *expFunc + *polynomFunc; // e^x + 5 + 3*x + 100*x^2 - 2*x^3
    auto complexFunc2 = *identFunc / *polynomFunc; // x / (5 + 3*x + 100*x^2 - 2*x^3)
    auto complexFunc3 = *expFunc * *complexFunc2; // e^x * (x / (5 + 3*x + 100*x^2 - 2*x^3))

    EXPECT_DOUBLE_EQ((*complexFunc1)(1), 106 + exp(1));
    EXPECT_DOUBLE_EQ((*complexFunc2)(1), 1. / 106);
    EXPECT_DOUBLE_EQ((*complexFunc3)(1), exp(1) * (1. / 106));
}

TEST(FunctionTest, FuncDeriv) {
    TFactory funcFactory;

    auto identFunc = funcFactory.CreateObject("ident"); //IdentFunc x
    auto constFunc = funcFactory.CreateObject("const", 8); // ConstFunc 5
    auto powerFunc = funcFactory.CreateObject("power", 5); // PowerFunc x^5
    auto polynomFunc = funcFactory.CreateObject("polynomial", {3, 2, 1, -4}); // TPolynomial 3 + 2*x + x^2 - 4*x^3
    auto expFunc = funcFactory.CreateObject("exp"); // TExp e^x

    EXPECT_EQ(identFunc->GetDeriv(99), 1);
    EXPECT_EQ(constFunc->GetDeriv(192), 0);
    EXPECT_EQ(powerFunc->GetDeriv(2), 16 * 5);
    EXPECT_EQ(polynomFunc->GetDeriv(42), 2 + 2 * 42 - 12 * 42 * 42);
    EXPECT_DOUBLE_EQ(expFunc->GetDeriv(12), exp(12));
}

TEST(FunctionTest, FuncDerivComplex) {
    TFactory funcFactory;

    auto identFunc = funcFactory.CreateObject("ident"); //IdentFunc x
    auto polynomFunc = funcFactory.CreateObject("polynomial", {5, 3, 100, -2}); // TPolynomial 6 - 2*x + 13*x^2 + 14*x^3
    auto expFunc = funcFactory.CreateObject("exp"); // TExp e^x

    auto complexFunc1 = *expFunc + *polynomFunc; // e^x + 5 + 3*x + 100*x^2 - 2*x^3
    auto complexFunc2 = *identFunc / *polynomFunc; // x / (5 + 3*x + 100*x^2 - 2*x^3)
    auto complexFunc3 = *expFunc * *complexFunc2; // e^x * (x / (5 + 3*x + 100*x^2 - 2*x^3))

    EXPECT_DOUBLE_EQ(complexFunc1->GetDeriv(1), 197 + exp(1));
    EXPECT_DOUBLE_EQ(complexFunc2->GetDeriv(1), -91./11236);
    EXPECT_DOUBLE_EQ(complexFunc3->GetDeriv(1), exp(1) * 15./11236);
}

TEST(FunctionTest, FuncTypeError) {
    TFactory funcFactory;

    auto powerFunc = funcFactory.CreateObject("power", 2); // PowerFunc x^2

    try {
        auto f = *powerFunc + "abc";
    } catch (std::exception &e) {
        EXPECT_EQ(e.what(), std::string("bad argument"));
    }

    try {
        auto f = *powerFunc + 2;
    } catch (std::exception &e) {
        EXPECT_EQ(e.what(), std::string("bad argument"));
    }

    try {
        auto f = *powerFunc + true;
    } catch (std::exception &e) {
        EXPECT_EQ(e.what(), std::string("bad argument"));
    }
}

TEST(GradientMethod, FindRoots) {
    TFactory funcFactory;

    auto func1 = funcFactory.CreateObject("polynomial", {4, 1}); // TPolynomial 4 + x
    auto func2 = funcFactory.CreateObject("polynomial", {-12, 4}); // TPolynomial -8 + 4x
    auto func3 = funcFactory.CreateObject("polynomial", {1, 2, 1}); // TPolynomial 1 + 2x + x^2


    EXPECT_NEAR(getRoot(*func1), -4, eps);
    EXPECT_NEAR(getRoot(*func2), 3, eps);
    EXPECT_NEAR(getRoot(*func3), -1, eps);
}


int main(int argc, char **argv) {

    TFactory funcFactory;
    auto p = funcFactory.GetAvailableFunctions();
    std::cout << "Available Functions:" << std::endl;
    for (int i = 1; i <= p.size(); i++) {
        std::cout << '\t' << i << ". " << p[i - 1] << std::endl;
    }

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
