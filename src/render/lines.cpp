#include "../Image/SFMLImage.h"
#include "cmath"

unsigned int linear_interpolation(const unsigned int start, const unsigned int end, const double t) {
    return static_cast<unsigned int>(std::round(start * (1 - t) + end * t));
}

namespace sgl::render {
    void draw_dotted_line(sgl::SFMLImage&image, const sf::Vector2u start, const sf::Vector2u end, const sf::Color color,
                          const unsigned int count) {
        const double step = 1.0 / count;
        for (double i = 0; i < 1; i += step) {
            const sf::Vector2u a = {
                linear_interpolation(start.x, end.x, i),
                linear_interpolation(start.y, end.y, i)
            };
            image.setPixel(a, color);
        }
    }

    void draw_dotted_line_fix1(sgl::SFMLImage&image, const sf::Vector2u start, const sf::Vector2u end,
                               const sf::Color color) {
        const double count = std::sqrt(std::pow(start.x - end.x, 2) + std::pow(start.y - end.y, 2));
        const double step = 1.0 / count;
        for (double i = 0; i < 1; i += step) {
            const sf::Vector2u a = {
                linear_interpolation(start.x, end.y, i),
                linear_interpolation(start.y, end.y, i)
            };
            image.setPixel(a, color);
        }
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
