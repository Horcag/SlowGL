#include "../Image/SFMLImage.h"
#include "cmath"

namespace sgl::render {
    void draw_dotted_line(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color,
                          unsigned int count) {
        double step = 1.0 / count;
        for (double i = 0; i < 1; i += step) {
            sf::Vector2u a = {
                static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
                static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
            };
            image.setPixel(a, color);
        }
    }

    void draw_dotted_line_fix1(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    }

    void draw_dotted_line_fix2(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    }

    void draw_dotted_line_loop_v1(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    }

    void draw_dotted_line_loop_fix1(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    }

    void draw_dotted_line_loop_fix2(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    }

    void draw_x_loop_dotted_line_v2(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    }

    void draw_x_loop_dotted_line_no_y(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    }

    void draw_x_loop_dotted_line_no_y_v2(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    }

    void draw_bresenham(sgl::SFMLImage&image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    }
}
