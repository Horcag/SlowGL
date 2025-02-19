#pragma once
#include <array>
#include <vector>
#include <SFML/System/Vector3.hpp>

#include "SFML/System/Vector2.hpp"
#ifndef BUFFER_H
#define BUFFER_H

struct Face {
    std::array<int, 3> vertexIndices;
    std::array<int, 3> textureIndices;
    std::array<int, 3> normalIndices;
};

class Model3D {
private:
    std::vector<sf::Vector3f> vertex;
    std::vector<sf::Vector2f> vertex_texture;
    std::vector<sf::Vector3f> vertex_normal;
    std::vector<Face> faces;

public:
    // Геттеры
    std::vector<sf::Vector3f> get_vertex() const;

    std::vector<sf::Vector2f> get_vertex_texture() const;

    std::vector<sf::Vector3f> get_vertex_normal() const;

    std::vector<Face> get_faces() const;

    // Сеттеры
    void set_vertex(const std::vector<sf::Vector3f>& newBufferV);
    void set_vertex_texture(const std::vector<sf::Vector2f>& newBufferVT);
    void set_vertex_normal(const std::vector<sf::Vector3f>& newBufferVN);
    void set_faces(const std::vector<Face>& newBufferF);
};




#endif //BUFFER_H
