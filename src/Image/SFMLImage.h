#include <SFML/Graphics.hpp>

namespace sgl {

    class SFMLImage : public sf::Drawable {

    public:
        sf::Image image;
        sf::Texture texture;
        sf::Sprite sprite;

        sf::Vector2f center;
        sf::Vector2f size;

    public:
        SFMLImage(sf::Vector2u size, sf::Color clear_color);

        void setPixel(sf::Vector2u coords, sf::Color color);
        void update();
        void resize(sf::Vector2u new_size);
        void draw(sf::RenderTarget &target, sf::RenderStates states) const;
        sf::Vector2u getSize() const;

        SFMLImage(const SFMLImage&) = delete;
        SFMLImage& operator= (const SFMLImage&) = delete;

    private:
        void transformSprite();
    };

}