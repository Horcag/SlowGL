#pragma once
#include <SFML/Graphics/Color.hpp>
#include "../../Texture/sgl_textures.h"

namespace sgl::utils {
    uint32_t colorToInteger(sf::Color color);

    
    inline void setUnitPixel(sgl::ColorTexture &texture, glm::vec2 pos, sf::Color color)
    {
        pos += glm::vec2(1.f, 1.f);
        pos *= glm::vec2(texture.getSize()) / 2.f;

        texture.setPixel(glm::uvec2(pos), color);
    }

    inline glm::uvec2 fromUnitPixel(const sgl::ColorTexture &texture, glm::vec2 pos)
    {
        pos += glm::vec2(1.f, 1.f);
        pos *= glm::vec2(texture.getSize()) / 2.f;

        return pos;
    }
}