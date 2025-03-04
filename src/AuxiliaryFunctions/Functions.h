#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <array>
#include "SFML/System/Vector2.hpp"
#include "SFML/System/Vector3.hpp"

const sf::Vector3i light_direction = sf::Vector3i(0, 0, 1);

sf::Vector3f barycentric_coord(const std::array<sf::Vector2f, 3> &, const sf::Vector2u &);

bool is_inside(const sf::Vector3f &);

sf::Vector3f cross_product(const sf::Vector3f &, const sf::Vector3f &);

float clip_edge(const sf::Vector3f &);

#endif //FUNCTIONS_H
