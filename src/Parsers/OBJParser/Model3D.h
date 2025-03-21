#pragma once
#include <array>
#include <vector>
#include <iterator>
#include <cstddef>

#include <SFML/System/Vector3.hpp>
#include "SFML/System/Vector2.hpp"

#ifndef BUFFER_H
#define BUFFER_H

struct Face {
    sf::Vector3i vertexIndices;
    sf::Vector3i textureIndices;
    sf::Vector3i normalIndices;
};

class Model3D {
public:
    struct PolyIterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const sf::Vector3f;
        using pointer = const Face *;
        using reference = const Face &;

        PolyIterator(const Model3D* model, pointer ptr) : m_model(model), m_ptr(ptr) {
        }

        [[nodiscard]] inline const sf::Vector3f& getVertex(int i) const {
            return m_model->vertex[(&m_ptr->vertexIndices.x)[i]];
        }

        [[nodiscard]] const sf::Vector2f& getUv(int i) const;

        [[nodiscard]] const sf::Vector3f& getNormal(int i) const;

        reference operator*() const { return *m_ptr; }
        pointer operator->() { return m_ptr; }

        PolyIterator& operator++() {
            m_ptr++;
            return *this;
        }

        PolyIterator operator++(int) {
            PolyIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        difference_type operator-(const PolyIterator&a) { return a.m_ptr - m_ptr; };

        friend bool operator==(const PolyIterator&a, const PolyIterator&b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!=(const PolyIterator&a, const PolyIterator&b) { return a.m_ptr != b.m_ptr; };

    private:
        const Model3D* m_model;
        pointer m_ptr;
    };

private:
    std::vector<sf::Vector3f> vertex;
    std::vector<sf::Vector2f> vertex_texture;
    std::vector<sf::Vector3f> vertex_normal;
    std::vector<Face> faces;

public:
    // Геттеры
    [[nodiscard]] std::vector<sf::Vector3f> get_vertex() const;

    [[nodiscard]] std::vector<sf::Vector2f> get_vertex_texture() const;

    [[nodiscard]] std::vector<sf::Vector3f> get_vertex_normal() const;

    [[nodiscard]] std::vector<Face> get_faces() const;

    // Сеттеры
    void set_vertex(const std::vector<sf::Vector3f>&newBufferV);

    void set_vertex_texture(const std::vector<sf::Vector2f>&newBufferVT);

    void set_vertex_normal(const std::vector<sf::Vector3f>&newBufferVN);

    void set_faces(const std::vector<Face>&newBufferF);

    [[nodiscard]] std::vector<sf::Vector3f>::const_iterator beginVertices() const { return vertex.begin(); }
    [[nodiscard]] std::vector<sf::Vector3f>::const_iterator endVertices() const { return vertex.end(); };

    [[nodiscard]] PolyIterator beginPolygons() const { return {this, faces.data()}; }
    [[nodiscard]] PolyIterator endPolygons() const { return {this, faces.data() + faces.size()}; }
};

#endif //BUFFER_H
