#include "Model3D.h"

std::vector<sf::Vector3f> Model3D::get_vertex() const {
    return vertex;
}

std::vector<sf::Vector2f> Model3D::get_vertex_texture() const {
    return vertex_texture;
}

std::vector<sf::Vector3f> Model3D::get_vertex_normal() const {
    return vertex_normal;
}

std::vector<Face> Model3D::get_faces() const {
    return faces;
}

void Model3D::set_vertex(const std::vector<sf::Vector3f>& vertex) {
    this->vertex = vertex;
}

void Model3D::set_vertex_texture(const std::vector<sf::Vector2f>& vertex_texture) {
    this->vertex_texture = vertex_texture;
}

void Model3D::set_vertex_normal(const std::vector<sf::Vector3f>& vertex_normal) {
    this->vertex_normal = vertex_normal;
}

void Model3D::set_faces(const std::vector<Face>& faces) {
    this->faces  = faces;
}

std::vector<sf::Vector3f>::const_iterator Model3D::beginVertices() const
{
    return vertex.begin();
}

std::vector<sf::Vector3f>::const_iterator Model3D::endVertices() const
{
    return vertex.end();
}

std::array<sf::Vector3f, 3> Model3D::get_vertexes(int index) const {
    const auto f = faces[index];
    std::array<sf::Vector3f, 3> res;
    res[0] = vertex[f.vertexIndices[0] - 1];
    res[1] = vertex[f.vertexIndices[1] - 1];
    res[2] = vertex[f.vertexIndices[2] - 1];
    return res;
}

