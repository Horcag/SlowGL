#pragma once
#include "string"
#include "vector"
#include "Parser.h"
#include "SFML/System/Vector2.hpp"
#include "Model3D.h"


class OBJParser:public Parser {
public:

    std::vector<sf::Vector3f> parse_vertex(std::ifstream &file) const;

    std::vector<sf::Vector2f> parse_vertex_texture(std::ifstream &file) const;

    std::vector<sf::Vector3f> parse_vertex_normal(std::ifstream &file) const;

    std::vector<Face> parse_faces(std::ifstream &file) const;

    Model3D parse(std::ifstream& file) const override;
};
