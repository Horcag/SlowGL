#include "render_utils.h"

uint32_t sgl::utils::colorToInteger(sf::Color color)
{
    return static_cast<std::uint32_t>((color.a << 24) | (color.b << 16) | (color.g << 8) | color.r);
}
