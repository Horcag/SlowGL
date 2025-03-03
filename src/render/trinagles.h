#ifndef TRINAGLES_H
#define TRINAGLES_H
#include "SFML/System/Vector2.hpp"
#include "../Image/SFMLImage.h"
#include "../AuxiliaryFunctions/Functions.h"
void trinagle_draw(sgl::SFMLImage&, const std::array<sf::Vector2f,3> &, sf::Color);

#endif //TRINAGLES_H
