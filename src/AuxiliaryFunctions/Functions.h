#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <array>
#include "SFML/System/Vector2.hpp"
#include "SFML/System/Vector3.hpp"

sf::Vector3f barycentric_coord(const std::array<sf::Vector2f,3> &, const sf::Vector2u&);
bool is_inside(const sf::Vector3f&);

#endif //FUNCTIONS_H
