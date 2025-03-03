#include "trinagles.h"
namespace sgl::render{
void triangle_draw(sgl::SFMLImage& image, const std::array<sf::Vector2f, 3>& triangle_coord, sf::Color) {
    double x_min = std::min(std::min(triangle_coord[0].x, triangle_coord[1].x), triangle_coord[2].x);
    x_min *= (x_min > 0);
    double y_min = std::min(std::min(triangle_coord[0].y, triangle_coord[1].y), triangle_coord[2].y);
    y_min *= (y_min > 0);
    double x_max = std::max(std::max(triangle_coord[0].x, triangle_coord[1].x), triangle_coord[2].x);
    double y_max = std::max(std::max(triangle_coord[0].y, triangle_coord[1].y), triangle_coord[2].y);

    for (auto x = x_min; x < x_max; ++x) {
        for (auto y = y_min; y < y_max; ++y) {
            sf::Vector2u coord = {static_cast<unsigned int>(x), static_cast<unsigned int>(y)};
            if (is_inside(barycentric_coord(triangle_coord, coord))) {
                image.setPixel(coord, sf::Color::Blue);
            }
        }
    }
}
}