#include "gtest/gtest.h"
#include "../src/OBJParser/OBJParser.h"
TEST(OBJParser_test,Parser_test) {
    const auto parser = new OBJParser;
    const auto res = parser->parseV("model_1.obj");
    std::tuple t{-0.031718,0.002147, 0.015501};
    ASSERT_NEAR((res[res.size() - 1]).x, std::get<0>(t), 1e-6);
    ASSERT_NEAR((res[res.size() - 1]).y, std::get<1>(t), 1e-6);
    ASSERT_NEAR((res[res.size() - 1]).z, std::get<2>(t), 1e-6);
    ASSERT_EQ(res.size(), 15258);
}