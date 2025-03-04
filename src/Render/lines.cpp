#include <deque>

#include "../Image/SFMLImage.h"
#include "cmath"

unsigned int linear_interpolation(const int start, const int end, const double t) {
    return static_cast<int>(std::round(start * (1 - t) + end * t));
}

namespace sgl::render {
    void draw_dotted_line(sgl::SFMLImage &image, const sf::Vector2i start, const sf::Vector2i end,
                          const sf::Color color,
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

    void draw_dotted_line_fix1(sgl::SFMLImage &image, const sf::Vector2i start, const sf::Vector2i end,
                               const sf::Color color) {
        // const double count = sf::Vector2f(start - end).length();
        const double count = std::sqrt(std::pow(start.x - end.x, 2) + std::pow(start.y - end.y, 2));
        const double step = 1.0 / count;
        for (double i = 0; i < 1; i += step) {
            const sf::Vector2u a = {
                linear_interpolation(start.x, end.x, i),
                linear_interpolation(start.y, end.y, i)
            };
            image.setPixel(a, color);
        }
    }

    void draw_dotted_line_loop_v1(sgl::SFMLImage &image, const sf::Vector2i start, const sf::Vector2i end,
                                  const sf::Color color) {
        for (int x = start.x; x < end.x; ++x) {
            const double t = (x - start.x) / static_cast<double>(end.x - start.x);
            const sf::Vector2u a = {static_cast<unsigned>(x), linear_interpolation(start.y, end.y, t)};
            image.setPixel(a, color);
        }
    }

    void draw_dotted_line_loop_fix1(sgl::SFMLImage &image, sf::Vector2i start, sf::Vector2i end, sf::Color color) {
        if (start.x > end.x) {
            std::swap(start.x, end.x);
            std::swap(start.y, end.y);
        }
        for (int x = start.x; x < end.x; ++x) {
            const double t = (x - start.x) / static_cast<double>(end.x - start.x);
            const sf::Vector2u a = {static_cast<unsigned>(x), linear_interpolation(start.y, end.y, t)};
            image.setPixel(a, color);
        }
    }

    void draw_dotted_line_loop_fix2(sgl::SFMLImage &image, sf::Vector2i start, sf::Vector2i end, sf::Color color) {
        bool xChange = false;
        if (std::abs(start.x - end.x) < std::abs(start.y - end.y)) {
            std::swap(start.x, start.y);
            std::swap(end.x, end.y);
            xChange = true;
        }
        for (int x = start.x; x < end.x; ++x) {
            const double t = (x - start.x) / static_cast<double>(end.x - start.x);
            const sf::Vector2u a = {static_cast<unsigned>(x), linear_interpolation(start.y, end.y, t)};
            xChange ? image.setPixel({a.y, a.x}, color) : image.setPixel(a, color);
        }
    }

    void draw_x_loop_dotted_line_v2(sgl::SFMLImage &image, sf::Vector2i start, sf::Vector2i end, sf::Color color) {
        bool xChange = false;
        if (std::abs(start.x - end.x) < std::abs(start.y - end.y)) {
            std::swap(start.x, start.y);
            std::swap(end.x, end.y);
            xChange = true;
        }
        if (start.x > end.x) {
            std::swap(start.x, end.x);
            std::swap(start.y, end.y);
        }
        for (int x = start.x; x < end.x; ++x) {
            const double t = (x - start.x) / static_cast<double>(end.x - start.x);
            const sf::Vector2u a = {static_cast<unsigned>(x), linear_interpolation(start.y, end.y, t)};
            xChange ? image.setPixel({a.y, a.x}, color) : image.setPixel(a, color);
        }
    }

    void draw_x_loop_dotted_line_no_y(sgl::SFMLImage &image, sf::Vector2i start, sf::Vector2i end, sf::Color color) {
        bool xChange = false;
        if (std::abs(start.x - end.x) < std::abs(start.y - end.y)) {
            std::swap(start.x, start.y);
            std::swap(end.x, end.y);
            xChange = true;
        }
        if (start.x > end.x) {
            std::swap(start.x, end.x);
            std::swap(start.y, end.y);
        }
        int y = start.y;
        double dy = std::abs(end.y - start.y) / static_cast<double>(end.x - start.x);
        double d_error = 0.0;
        const int y_update = end.y > start.y ? 1 : -1;
        for (int x = start.x; x < end.x; ++x) {
            xChange
                ? image.setPixel({static_cast<unsigned>(y), static_cast<unsigned>(x)}, color)
                : image.setPixel({static_cast<unsigned>(x), static_cast<unsigned>(y)}, color);
            d_error += dy;
            if (d_error > 0.5) {
                d_error -= 1.0;
                y += y_update;
            }
        }
    }

    void draw_x_loop_dotted_line_no_y_v2(sgl::SFMLImage &image, sf::Vector2i start, sf::Vector2i end, sf::Color color) {
        bool xChange = false;
        if (std::abs(start.x - end.x) < std::abs(start.y - end.y)) {
            std::swap(start.x, start.y);
            std::swap(end.x, end.y);
            xChange = true;
        }
        if (start.x > end.x) {
            std::swap(start.x, end.x);
            std::swap(start.y, end.y);
        }
        int y = start.y;
        double dy = 2 * (end.x - start.x) * std::abs(end.y - start.y) / static_cast<double>(end.x - start.x);
        double d_error = 0;
        const int y_update = end.y > start.y ? 1 : -1;
        for (int x = start.x; x < end.x; ++x) {
            xChange
                ? image.setPixel({static_cast<unsigned>(y), static_cast<unsigned>(x)}, color)
                : image.setPixel({static_cast<unsigned>(x), static_cast<unsigned>(y)}, color);
            d_error += dy;
            if (d_error > 2 * (end.x - start.x) * 0.5) {
                d_error -= 2 * (end.x - start.x);
                y += y_update;
            }
        }
    }

    void draw_bresenham(sgl::SFMLImage &image, sf::Vector2i start, sf::Vector2i end, sf::Color color) {
        bool xChange = false;
        if (std::abs(start.x - end.x) < std::abs(start.y - end.y)) {
            std::swap(start.x, start.y);
            std::swap(end.x, end.y);
            xChange = true;
        }
        if (start.x > end.x) {
            std::swap(start.x, end.x);
            std::swap(start.y, end.y);
        }
        int y = start.y;
        int dy = 2 * std::abs(end.y - start.y);
        int d_error = 0;
        const int y_update = end.y > start.y ? 1 : -1;
        for (int x = start.x; x < end.x; ++x) {
            xChange
                ? image.setPixel({static_cast<unsigned>(y), static_cast<unsigned>(x)}, color)
                : image.setPixel({static_cast<unsigned>(x), static_cast<unsigned>(y)}, color);
            d_error += dy;
            if (d_error > end.x - start.x) {
                d_error -= 2 * (end.x - start.x);
                y += y_update;
            }
        }
    }

    //http://www.edepot.com/linea.html
    void draw_efla(sgl::SFMLImage &image, const sf::Vector2i &start, const sf::Vector2i &end, sf::Color color) {
        bool yLonger = false;
        int incrementVal;

        int shortLen = end.y - start.y;
        int longLen = end.x - start.x;
        if (abs(shortLen) > abs(longLen)) {
            int swap = shortLen;
            shortLen = longLen;
            longLen = swap;
            yLonger = true;
        }

        if (longLen < 0) incrementVal = -1;
        else incrementVal = 1;

        double divDiff;
        if (shortLen == 0) divDiff = longLen;
        else divDiff = static_cast<double>(longLen) / static_cast<double>(shortLen);
        if (yLonger) {
            for (int i = 0; i != longLen; i += incrementVal) {
                image.setPixel({
                                   static_cast<unsigned>(start.x + static_cast<int>(static_cast<double>(i) / divDiff)),
                                   static_cast<unsigned>(start.y + i)
                               }, color);
            }
        } else {
            for (int i = 0; i != longLen; i += incrementVal) {
                image.setPixel({
                                   static_cast<unsigned>(start.x + i),
                                   static_cast<unsigned>(start.y + static_cast<int>(static_cast<double>(i) / divDiff))
                               }, color);
            }
        }
    }
}
