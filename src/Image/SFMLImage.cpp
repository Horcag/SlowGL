#include "SFMLImage.h"

sgl::SFMLImage::SFMLImage(sf::Vector2u size, sf::Color clear_color) : image(size, clear_color), texture(size), sprite(texture)
{
    this->size.x = size.x;
    this->size.y = size.y;
    center = {0, 0};
    transformSprite();
}

void sgl::SFMLImage::setPixel(sf::Vector2u coords, sf::Color color)
{
    image.setPixel(coords, color);
}

void sgl::SFMLImage::update()
{
    texture.update(image);
    sprite.setTexture(texture, true);
}

void sgl::SFMLImage::resize(sf::Vector2u new_size)
{
    image.resize(new_size);
    texture.resize(new_size);
    transformSprite();
}

void sgl::SFMLImage::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(sprite, states);
}

sf::Vector2u sgl::SFMLImage::getSize() const
{
    return image.getSize();
}

void sgl::SFMLImage::transformSprite()
{
    sf::Vector2u txSize = texture.getSize();
    sprite.setOrigin(sf::Vector2f(txSize) / 2.f);
    //sprite.setPosition(center);
    //sprite.setScale({size.x / txSize.x, size.y / txSize.y});
}
