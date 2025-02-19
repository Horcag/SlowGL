#include "gtest/gtest.h"
#include "Parsers/OBJParser/OBJParser.h"

#include <fstream>

TEST(OBJParser_test, ParserV_test_ifstream) {
    const auto parser = new OBJParser;
    std::ifstream file("model_1.obj");
    const auto res = parser->parse_vertex(file);
    const std::tuple t{-0.031718, 0.002147, 0.015501};
    ASSERT_NEAR(res[res.size() - 1].x, std::get<0>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].y, std::get<1>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].z, std::get<2>(t), 1e-6);
    ASSERT_EQ(res.size(), 15258);
}

TEST(OBJParser_test, ParserVt_test_ifstream) {
    const auto parser = new OBJParser;
    std::ifstream file("model_1.obj");
    const auto res = parser->parse_vertex_texture(file);
    std::tuple t{0.562237, 0.853435};
    ASSERT_NEAR(res[res.size() - 1].x, std::get<0>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].y, std::get<1>(t), 1e-6);
    ASSERT_EQ(res.size(), 16323);
}

TEST(OBJParser_test, ParserF_test_ifstream) {
    const auto parser = new OBJParser;
    std::ifstream file("model_1.obj");
    const auto res = parser->parse_faces(file);
    const std::array<int,3> ver{15257,1685,1675};
    const std::array<int,3> norm{16314,16320,16323};
    const std::array<int,3> text{15247,1685,1675};
    ASSERT_EQ(res[res.size()-1].vertexIndices,ver);
    ASSERT_EQ(res[res.size()-1].normalIndices,text);
    ASSERT_EQ(res[res.size()-1].textureIndices,norm);
    ASSERT_EQ(res.size(), 30338);
}

TEST(OBJParser_test, ParserVN_test_ifstream) {
    const auto parser = new OBJParser;
    std::ifstream file("model_1.obj");
    const auto res = parser->parse_vertex_normal(file);
    std::tuple t{-0.924700, -0.132900, 0.356800};
    ASSERT_NEAR(res[res.size() - 1].x, std::get<0>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].y, std::get<1>(t), 1e-6);
    ASSERT_NEAR(res[res.size() - 1].z, std::get<2>(t), 1e-6);
    ASSERT_EQ(res.size(), 15248);
}

TEST(OBJParser_test, Parser_test_ifstream) {
    const auto parser = new OBJParser;
    std::ifstream file("model_1.obj");
    const auto res = parser->parse(file);
    const auto res1 = res.get_vertex();
    const auto res2 = res.get_faces();
    const auto res3 = res.get_vertex_normal();
    const auto res4 = res.get_vertex_texture();
    const std::tuple t1{-0.031718, 0.002147, 0.015501};
    ASSERT_NEAR(res1[res1.size() - 1].x, std::get<0>(t1), 1e-6);
    ASSERT_NEAR(res1[res1.size() - 1].y, std::get<1>(t1), 1e-6);
    ASSERT_NEAR(res1[res1.size() - 1].z, std::get<2>(t1), 1e-6);
    ASSERT_EQ(res1.size(), 15258);
    std::tuple t4{0.562237, 0.853435};
    ASSERT_NEAR(res4[res4.size() - 1].x, std::get<0>(t4), 1e-6);
    ASSERT_NEAR(res4[res4.size() - 1].y, std::get<1>(t4), 1e-6);
    ASSERT_EQ(res4.size(), 16323);
    const std::array<int,3> ver{15257,1685,1675};
    const std::array<int,3> norm{16314,16320,16323};
    const std::array<int,3> text{15247,1685,1675};
    ASSERT_EQ(res.get_faces()[res.get_faces().size()-1].vertexIndices,ver);
    ASSERT_EQ(res.get_faces()[res.get_faces().size()-1].normalIndices,text);
    ASSERT_EQ(res.get_faces()[res.get_faces().size()-1].textureIndices,norm);
    ASSERT_EQ(res.get_faces().size(), 30338);
    std::tuple t2{-0.924700, -0.132900, 0.356800};
    ASSERT_NEAR(res.get_vertex_normal()[res.get_vertex_normal().size() - 1].x, std::get<0>(t2), 1e-6);
    ASSERT_NEAR(res.get_vertex_normal()[res.get_vertex_normal().size() - 1].y, std::get<1>(t2), 1e-6);
    ASSERT_NEAR(res.get_vertex_normal()[res.get_vertex_normal().size() - 1].z, std::get<2>(t2), 1e-6);
    ASSERT_EQ(res.get_vertex_normal().size(), 15248);
}