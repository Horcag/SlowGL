#include "sgl_textures.h"
#include <glm/common.hpp>

namespace sgl::utils {
    template<typename T>
    IBaseTexture<T>::IBaseTexture(uint32_t width, uint32_t height) : width(width), height(height)
    {
        data.resize(width, height);
    }

    template <typename T>
    void IBaseTexture<T>::clear(T value)
    {
        memset(data.data(), value, width*height*sizeof(T));
    }

    template <typename T>
    void IBaseTexture<T>::setPixel(uint32_t x, uint32_t y, T value)
    {
        data[x + y*height] = value;
    }

    template <typename T>
    T IBaseTexture<T>::getPixel(uint32_t x, uint32_t y)
    {
        return data[x + y*height];
    }
}

void sgl::DepthTexture::drawTo(sf::Image image)
{
    float min, max;
    min = max = data[0];
    auto begin = data.begin();

    for(auto p = begin; p < data.end(); p++){
        if(*p < min) min = *p;
        if(*p > max) max = *p;
    }

    max += 0.1f;

    for(auto p = begin; p < data.end(); p++){
        uint32_t index = p - begin;
        float color = 255.f * (*p - min) / (max - min);
        image.setPixel(sf::Vector2u(index % width, index / width), sf::Color(color,color,color));
    }
}

void sgl::ColorTexture::setPixel(uint32_t x, uint32_t y, sf::Color color)
{
    data[x + y*width] = color.toInteger();
}

void sgl::ColorTexture::drawTo(sf::Image image)
{
    for(auto p = data.begin(); p < data.end(); p++){
        uint32_t index = p - data.begin();
        image.setPixel(sf::Vector2u(index % width, index / width), sf::Color(*p));
    }
}
