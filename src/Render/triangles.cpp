#include "SFML/System/Vector2.hpp"
#include "SFML/System/Vector3.hpp"
#include <algorithm>
#include "../Image/SFMLImage.h"


namespace sgl::render {
    sf::Vector3f barycentric(const sf::Vector2i&point, const sf::Vector2f&v0, const sf::Vector2f&v1,
                             const sf::Vector2f&v2) {
        const float denominator = ((v0.x - v2.x) * (v1.y - v2.y)) - ((v1.x - v2.x) * (v0.y - v2.y));

        // Проверка деления не ноль
        if (std::abs(denominator) < std::numeric_limits<float>::epsilon()) {
            return {-1.0f, -1.0f, -1.0f}; // Индикатор ошибки
        }

        float lambda0 = ((static_cast<float>(point.x) - v2.x) * (v1.y - v2.y) - (static_cast<float>(point.y) - v2.y) * (
                             v1.x - v2.x)) / denominator;
        float lambda1 = ((static_cast<float>(point.y) - v2.y) * (v0.x - v2.x) - (static_cast<float>(point.x) - v2.x) * (
                             v0.y - v2.y)) / denominator;
        float lambda2 = 1.0f - lambda0 - lambda1;

        return {lambda0, lambda2, lambda1};
    }

    void drawTriangle(sgl::SFMLImage&image, const sf::Vector2f&v0, const sf::Vector2f&v1, const sf::Vector2f&v2,
                      const sf::Color&color) {
        // 1. Определяем границы треугольника
        float xmin = std::min({v0.x, v1.x, v2.x});
        float xmax = std::max({v0.x, v1.x, v2.x});
        float ymin = std::min({v0.y, v1.y, v2.y});
        float ymax = std::max({v0.y, v1.y, v2.y});

        // 2. Проверяем границы изображения
        const sf::Vector2u imageSize = image.getSize();
        xmin = std::max(0.0f, xmin);
        ymin = std::max(0.0f, ymin);
        xmax = std::min(static_cast<float>(imageSize.x - 1), xmax);
        ymax = std::min(static_cast<float>(imageSize.y - 1), ymax);

        // 3. Проходим по всем пикселям в границах прямоугольника
        for (int x = static_cast<int>(xmin); x <= static_cast<int>(xmax); ++x) {
            for (int y = static_cast<int>(ymin); y <= static_cast<int>(ymax); ++y) {
                // 4. Вычисляем барицентрические координаты
                sf::Vector3f bc = barycentric({x, y}, v0, v1, v2);

                // 5. Проверяем, попадает ли точка внутрь треугольника
                if (bc.x >= 0 && bc.y >= 0 && bc.z >= 0) {
                    image.setPixel({static_cast<unsigned>(x), static_cast<unsigned>(y)}, color);
                }
            }
        }
    }
}
