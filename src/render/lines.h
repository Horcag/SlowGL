#ifndef LINES_H
#define LINES_H
#include "SFML/System/Vector2.hpp"
#include "../Image/SFMLImage.h"

namespace sgl::render {
    void draw_dotted_line(sgl::SFMLImage&image, sf::Vector2u, sf::Vector2u, sf::Color, int count);

    void draw_dotted_line_fix1(sgl::SFMLImage&image, sf::Vector2u, sf::Vector2u, sf::Color);

    void draw_dotted_line_loop_v1(sgl::SFMLImage&image, sf::Vector2u, sf::Vector2u, sf::Color);

    void draw_dotted_line_loop_fix1(sgl::SFMLImage&image, sf::Vector2u, sf::Vector2u, sf::Color);

    void draw_dotted_line_loop_fix2(sgl::SFMLImage&image, sf::Vector2u, sf::Vector2u, sf::Color);

    void draw_x_loop_dotted_line_v2(sgl::SFMLImage&image, sf::Vector2u, sf::Vector2u, sf::Color);

    void draw_x_loop_dotted_line_no_y(sgl::SFMLImage&image, sf::Vector2u, sf::Vector2u, sf::Color);

    void draw_x_loop_dotted_line_no_y_v2(sgl::SFMLImage&image, sf::Vector2u, sf::Vector2u, sf::Color);

    void draw_bresenham(sgl::SFMLImage&image, sf::Vector2u, sf::Vector2u, sf::Color);
}

#endif //LINES_H
