#pragma once
#include <SFML/Graphics.hpp>

namespace sgl {
    class SFMLImage : public sf::Drawable {
    public:
        sf::Image image;
        sf::Texture texture;
        sf::Sprite sprite;
        sf::Color clear_color;

        sf::Vector2f center;
        sf::Vector2f size;

    public:
        SFMLImage(sf::Vector2u size, sf::Color clear_color);

        void setPixel(sf::Vector2u coords, sf::Color color);

        void clear();

        void clear(sf::Color color);

        void update();

        void resize(sf::Vector2u new_size);

        void draw(sf::RenderTarget&target, sf::RenderStates states) const override;

        sf::Vector2u getSize() const;

        bool saveToFile(const std::filesystem::path&path) const;

        SFMLImage(const SFMLImage&) = delete;

        SFMLImage& operator=(const SFMLImage&) = delete;

    private:
        void transformSprite();
    };
}
