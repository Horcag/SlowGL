#pragma once
#include <array>

#include "string"
#include "vector"
#include "SFML/System/Vector2.hpp"
#ifndef PARSER_H
#define PARSER_H
#include "Buffer.h"

struct Face {
    std::array<int, 3> vertexIndices;
    std::array<int, 3> textureIndices;
    std::array<int, 3> normalIndices;
};

class OBJParser {
public:
    std::vector<sf::Vector3f> parseV(const std::string &fileName);

    std::vector<sf::Vector3f> parseV(std::ifstream &file);

    std::vector<sf::Vector2f> parseVt(std::ifstream &file);

    std::vector<Face> parseF(std::ifstream &file);
};
#endif //PARSER_H
