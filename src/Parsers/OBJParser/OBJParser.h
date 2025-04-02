#pragma once
#include "string"
#include "vector"
#include "Parser.h"
#include "SFML/System/Vector2.hpp"
#include "Model3D.h"


class OBJParser final : public Parser {
public:
    static std::vector<sf::Vector3f> parse_vertex(std::ifstream&file);

    static std::vector<sf::Vector2f> parse_vertex_texture(std::ifstream&file);

    static std::vector<sf::Vector3f> parse_vertex_normal(std::ifstream&file);

    static std::vector<Face> parse_faces(std::ifstream&file);

    Model3D parse(std::ifstream&file) const override;
};
