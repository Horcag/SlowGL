#include "../Image/SFMLImage.h"
#include "cmath"
namespace sgl::render {

void draw_dotted_line(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color, uint count) {
  double step = 1.0 / count;
  for (double i = 0; i < 1; i += step) {
    sf::Vector2u a = {
        static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
        static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
    };
    image.setPixel(a, color);
  }
}
void draw_dotted_line_fix1(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    double distance = std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2));
    double step = 1.0 / distance;
    for (double i = 0; i < 1; i += step * 2) {
        sf::Vector2u a = {
            static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
            static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
        };
        image.setPixel(a, color);
    }
}

void draw_dotted_line_fix2(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    double distance = std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2));
    double step = 1.0 / distance;
    for (double i = 0; i < 1; i += step * 3) {
        sf::Vector2u a = {
            static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
            static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
        };
        image.setPixel(a, color);
    }
}

void draw_dotted_line_loop_v1(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    double distance = std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2));
    double step = 1.0 / distance;
    for (double i = 0; i < 1; i += step) {
        if (static_cast<int>(i * 10) % 2 == 0) {
            sf::Vector2u a = {
                static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
                static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
            };
            image.setPixel(a, color);
        }
    }
}

void draw_dotted_line_loop_fix1(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    double distance = std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2));
    double step = 1.0 / distance;
    for (double i = 0; i < 1; i += step) {
        if (static_cast<int>(i * 20) % 2 == 0) {
            sf::Vector2u a = {
                static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
                static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
            };
            image.setPixel(a, color);
        }
    }
}

void draw_dotted_line_loop_fix2(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    double distance = std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2));
    double step = 1.0 / distance;
    for (double i = 0; i < 1; i += step) {
        if (static_cast<int>(i * 30) % 2 == 0) {
            sf::Vector2u a = {
                static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
                static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
            };
            image.setPixel(a, color);
        }
    }
}

void draw_x_loop_dotted_line_v2(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    double distance = std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2));
    double step = 1.0 / distance;
    for (double i = 0; i < 1; i += step) {
        if (static_cast<int>(i * 40) % 2 == 0) {
            sf::Vector2u a = {
                static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
                static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
            };
            image.setPixel(a, color);
        }
    }
}

void draw_x_loop_dotted_line_no_y(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    double distance = std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2));
    double step = 1.0 / distance;
    for (double i = 0; i < 1; i += step) {
        if (static_cast<int>(i * 50) % 2 == 0) {
            sf::Vector2u a = {
                static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
                static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
            };
            image.setPixel(a, color);
        }
    }
}

void draw_x_loop_dotted_line_no_y_v2(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    double distance = std::sqrt(std::pow(end.x - start.x, 2) + std::pow(end.y - start.y, 2));
    double step = 1.0 / distance;
    for (double i = 0; i < 1; i += step) {
        if (static_cast<int>(i * 60) % 2 == 0) {
            sf::Vector2u a = {
                static_cast<unsigned int>(std::round(start.x * (1 - i) + end.x * i)),
                static_cast<unsigned int>(std::round(start.y * (1 - i) + end.y * i))
            };
            image.setPixel(a, color);
        }
    }
}

void draw_bresenham(sgl::SFMLImage& image, sf::Vector2u start, sf::Vector2u end, sf::Color color) {
    int dx = std::abs(static_cast<int>(end.x) - static_cast<int>(start.x));
    int dy = std::abs(static_cast<int>(end.y) - static_cast<int>(start.y));
    int sx = start.x < end.x ? 1 : -1;
    int sy = start.y < end.y ? 1 : -1;
    int err = dx - dy;

    while (true) {
        image.setPixel(start, color);
        if (start.x == end.x && start.y == end.y) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            start.x += sx;
        }
        if (e2 < dx) {
            err += dx;
            start.y += sy;
        }
    }
}

}
