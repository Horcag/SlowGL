#include "gtest/gtest.h"
#include "../src/OBJParser/OBJParser.h"

TEST(OBJParser_test,Parser_test) {
    const auto parser = new OBJParser;
    auto res = parser->parseV("model_1.bin");
    std::tuple<double,double,double> t{-0.031718,0.002147, 0.015501};
    ASSERT_NEAR(std::get<0>(res[res.size() - 1]), std::get<0>(t), 1e-6);
    ASSERT_NEAR(std::get<1>(res[res.size() - 1]), std::get<1>(t), 1e-6);
    ASSERT_NEAR(std::get<2>(res[res.size() - 1]), std::get<2>(t), 1e-6);
    ASSERT_EQ(res.size(), 15258);
}