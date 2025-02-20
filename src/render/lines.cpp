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
}}
