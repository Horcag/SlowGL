#include "sgl_textures.h"
#include "../Render/utils/render_utils.h"

namespace sgl::utils {
    template<typename T>
    IBaseTexture<T>::IBaseTexture(uint32_t width, uint32_t height) : width(width), height(height) {
        data.resize(width * height);
    }

    template<typename T>
    void IBaseTexture<T>::clear(T value) {
        std::fill(data.begin(), data.end(), value);
    }

    template <typename T>
    void IBaseTexture<T>::resize(uint32_t width, uint32_t height)
    {
        this->width = width;
        this->height = height;
        data.resize(width * height);
    }

    template<typename T>
    void IBaseTexture<T>::setPixel(uint32_t x, uint32_t y, T value) {
        data[x + y * height] = value;
    }

    template<typename T>
    T IBaseTexture<T>::getPixel(uint32_t x, uint32_t y) {
        return data[x + y * height];
    }

    template class IBaseTexture<float>;
    template class IBaseTexture<uint32_t>;
}

void sgl::DepthTexture::drawTo(sf::Image image) {
    float max;
    float min = max = data[0];
    const auto begin = data.begin();

    for (auto p = begin; p < data.end(); ++p) {
        if (*p < min) min = *p;
        if (*p > max) max = *p;
    }

    max += 0.1f;

    for (auto p = begin; p < data.end(); ++p) {
        uint32_t index = p - begin;
        float color = 255.f * (*p - min) / (max - min);
        image.setPixel(sf::Vector2u(index % width, index / width), sf::Color(color, color, color));
    }
}

void sgl::ColorTexture::setPixel(uint32_t x, uint32_t y, sf::Color color) {
    if(x > width || y > height) return;
    data[x + y * width] = sgl::utils::colorToInteger(color);
}

void sgl::ColorTexture::drawTo(sf::Texture& image) {
    image.update((const uint8_t*)data.data());
}

