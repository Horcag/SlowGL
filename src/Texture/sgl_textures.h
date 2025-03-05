#pragma once
#include <memory>
#include <vector>
#include <stdint.h>
#include <glm/vec2.hpp>
#include <SFML/Graphics.hpp>

namespace sgl {
    namespace utils {
        template<typename T>
        class IBaseTexture {
        protected:
            std::vector<T> data;
            uint32_t width, height;

        public:
            IBaseTexture(uint32_t width, uint32_t height);

            void clear(T value);

            void setPixel(uint32_t x, uint32_t y, T value);

            void setPixel(glm::uvec2 pos, T value) { setPixel(pos.x, pos.y, value); }

            T getPixel(uint32_t x, uint32_t y);

            T getPixel(glm::uvec2 pos) { return getPixel(pos.x, pos.y); }
            [[nodiscard]] uint32_t getWidth() const { return width; };
            [[nodiscard]] uint32_t getHeight() const { return height; };
            glm::uvec2 getSize() { return glm::uvec2(width, height); };
        };
    }

    class DepthTexture : public utils::IBaseTexture<float> {
        DepthTexture(uint32_t width, uint32_t height) : IBaseTexture(width, height) {
        };

        void drawTo(sf::Image image);
    };

    class ColorTexture : public utils::IBaseTexture<uint32_t> {
        void setPixel(uint32_t x, uint32_t y, sf::Color color);

        inline void setPixel(glm::uvec2 pos, sf::Color color) { setPixel(pos.x, pos.y, color); }

        void drawTo(sf::Image image);
    };
}
