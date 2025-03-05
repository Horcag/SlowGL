//
// Created by nikit on 3/4/25.
//

#ifndef TRIANGLES_H
#define TRIANGLES_H
#include "SFML/System/Vector2.hpp"
#include "SFML/System/Vector3.hpp"
#include "../Image/SFMLImage.h"
#include "../Texture/sgl_textures.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace sgl::render {
    /**
     * Вычисляет барицентрические координаты точки относительно треугольника.
     * @param point Целочисленная точка с экранными координатами (x, y)
     * @param v0 Первая вершина треугольника (вещественные координаты)
     * @param v1 Вторая вершина треугольника (вещественные координаты)
     * @param v2 Третья вершина треугольника (вещественные координаты)
     * @return Вектор (lambda0, lambda1, lambda2), где каждая координата показывает "вес"
     *         соответствующей вершины. Если точка внутри треугольника, все значения >= 0.
     *         Для вырожденного треугольника возвращает (-1, -1, -1).
     */
    sf::Vector3f barycentric(const sf::Vector2i&point, const sf::Vector2f&v0, const sf::Vector2f&v1,
                             const sf::Vector2f&v2);

    /**
     * Рисует закрашенный треугольник на изображении.
     * @param image Изображение для отрисовки
     * @param v0 Первая вершина треугольника (вещественные координаты)
     * @param v1 Вторая вершина треугольника (вещественные координаты)
     * @param v2 Третья вершина треугольника (вещественные координаты)
     * @param color Цвет заливки треугольника
     *
     * Функция определяет ограничивающий прямоугольник треугольника, проверяет его
     * пересечение с границами изображения и закрашивает только те пиксели,
     * которые лежат внутри треугольника (используя барицентрические координаты).
     */
    void drawTriangle(sgl::SFMLImage&image, const sf::Vector2f&v0, const sf::Vector2f&v1, const sf::Vector2f&v2,
                      const sf::Color&color);


    void drawTriangleTransform(sgl::ColorTexture& target, const glm::mat4 transform, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, sf::Color color);
}
#endif //TRIANGLES_H
