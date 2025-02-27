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

void Model3D::set_vertex(const std::vector<sf::Vector3f>&newBufferV) {
    this->vertex = newBufferV;
}

void Model3D::set_vertex_texture(const std::vector<sf::Vector2f>&newBufferVT) {
    this->vertex_texture = newBufferVT;
}

void Model3D::set_vertex_normal(const std::vector<sf::Vector3f>&newBufferVN) {
    this->vertex_normal = newBufferVN;
}

void Model3D::set_faces(const std::vector<Face>&newBufferF) {
    this->faces = newBufferF;
}

//const sf::Vector3f &Model3D::PolyIterator::getVertex(int i) const


const sf::Vector2f& Model3D::PolyIterator::getUv(const int i) const {
    return m_model->vertex_texture[(&m_ptr->textureIndices.x)[i]];
}

const sf::Vector3f& Model3D::PolyIterator::getNormal(const int i) const {
    return m_model->vertex_normal[(&m_ptr->normalIndices.x)[i]];
}
