#include "gtest/gtest.h"
#include "OBJParser/OBJParser.h"
#include <fstream>

TEST(OBJParser_test, ParserV_test_filename) {
    const auto parser = new OBJParser;
    const auto res = parser->parseV("model_1.obj");
    const std::tuple t{-0.031718, 0.002147, 0.015501};
    ASSERT_NEAR(res[res.size() - 1].x, std::get<0>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].y, std::get<1>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].z, std::get<2>(t), 1e-6);
    ASSERT_EQ(res.size(), 15258);
}

TEST(OBJParser_test, ParserV_test_ifstream) {
    const auto parser = new OBJParser;
    std::ifstream file("model_1.obj");
    const auto res = parser->parseV(file);
    const std::tuple t{-0.031718, 0.002147, 0.015501};
    ASSERT_NEAR(res[res.size() - 1].x, std::get<0>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].y, std::get<1>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].z, std::get<2>(t), 1e-6);
    ASSERT_EQ(res.size(), 15258);
}

TEST(OBJParser_test, ParserVt_test_ifstream) {
    const auto parser = new OBJParser;
    std::ifstream file("model_1.obj");
    const auto res = parser->parseVt(file);
    std::tuple t{0.562237, 0.853435};
    ASSERT_NEAR(res[res.size() - 1].x, std::get<0>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].y, std::get<1>(t), 1e-6);
    ASSERT_EQ(res.size(), 16323);
}

TEST(OBJParser_test, ParserF_test_ifstream) {
    const auto parser = new OBJParser;
    std::ifstream file("model_1.obj");
    const auto res = parser->parseF(file);
    const std::array<int,3> ver{15257,1685,1675};
    const std::array<int,3> norm{16314,16320,16323};
    const std::array<int,3> text{15247,1685,1675};
    ASSERT_EQ(res[res.size()-1].vertexIndices,ver);
    ASSERT_EQ(res[res.size()-1].normalIndices,text);
    ASSERT_EQ(res[res.size()-1].textureIndices,norm);
    ASSERT_EQ(res.size(), 30338);
}
