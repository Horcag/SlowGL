#include "SFML/System/Vector2.hpp"
#include "SFML/System/Vector3.hpp"
#include <algorithm>
#include "../Image/SFMLImage.h"
#include "utils/render_utils.h"
#include "triangles.h"
#include <glm/ext/vector_relational.hpp>
#include "../Utility/glm_converters.h"

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

glm::vec3 barycentric_(const glm::vec2&point, const glm::vec2&v0, const glm::vec2&v1, const glm::vec2&v2) {
    const float denominator = ((v0.x - v2.x) * (v1.y - v2.y)) - ((v1.x - v2.x) * (v0.y - v2.y));

    // Проверка деления не ноль
    if (std::abs(denominator) < std::numeric_limits<float>::epsilon()) {
        return {-1.0f, -1.0f, -1.0f}; // Индикатор ошибки
    }

    float lambda0 = ((static_cast<float>(point.x) - v2.x) * (v1.y - v2.y) - (static_cast<float>(point.y) - v2.y) * (v1.x - v2.x)) / denominator;
    float lambda1 = ((static_cast<float>(point.y) - v2.y) * (v0.x - v2.x) - (static_cast<float>(point.x) - v2.x) * (v0.y - v2.y)) / denominator;
    float lambda2 = 1.0f - lambda0 - lambda1;

    return {lambda0, lambda2, lambda1};
}

void sgl::render::drawTriangleTransform(sgl::ColorTexture &target, const glm::mat4 transform, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, sf::Color color)
{
    glm::vec2 size = target.getSize();
    glm::vec2 v0t = (glm::vec2(transform * glm::vec4(v0, 1.f)) + glm::vec2(1.f, 1.f)) * size / 2.f;
    glm::vec2 v1t = (glm::vec2(transform * glm::vec4(v1, 1.f)) + glm::vec2(1.f, 1.f)) * size / 2.f;
    glm::vec2 v2t = (glm::vec2(transform * glm::vec4(v2, 1.f)) + glm::vec2(1.f, 1.f)) * size / 2.f;

    glm::uvec2 mins = glm::clamp(glm::min(v0t, glm::min(v1t, v2t)) - glm::vec2(1.f), glm::vec2(0.f), size);
    glm::uvec2 maxs = glm::clamp(glm::max(v0t, glm::max(v1t, v2t)) + glm::vec2(1.f), glm::vec2(0.f), size);
    glm::vec3 bayc;

    for(uint32_t x = mins.x; x < maxs.x; x++){
        for(uint32_t y = mins.y; y < maxs.y; y++){
            bayc = barycentric_(glm::vec2(x,y), v0t, v1t, v2t);
            if(glm::all(glm::greaterThanEqual(bayc, glm::vec3(0.f)))){
                target.setPixel(glm::uvec2(x,y), color);
            }
        }
    }
}

void sgl::render::drawTrianglesBuffer(sgl::ColorTexture & target, const glm::mat4 transform, const std::vector<glm::vec3> vertex, const std::vector<glm::uvec3> indices)
{
    glm::vec2 image_size = target.getSize();
    std::vector<glm::vec2> transformed(vertex.size(), glm::vec2());
    for(size_t i = 0; i < transformed.size(); i++){
        transformed[i] = (glm::vec2(transform * glm::vec4(vertex[i], 1.f)) + glm::vec2(1.f, 1.f)) * image_size / 2.f;
        //target.setPixel(*transformed, sf::Color::Blue);
    }
    
    const glm::vec3 camera = glm::normalize(glm::vec3(transform[0][2],transform[1][2],transform[2][2]));

    glm::uvec2 mins, maxs;
    
    glm::vec3 bayc;

    for(size_t i= 0; i < indices.size(); i++){
        const glm::uvec3 index = indices[i];

        const glm::vec3 v0 = vertex[index.x];
        const glm::vec3 v1 = vertex[index.y];
        const glm::vec3 v2 = vertex[index.z];

        const glm::vec3 normal = glm::normalize(glm::cross(v1-v2,v1-v0));
        float dot = glm::dot(camera, normal);
        if(dot >= 0) continue;

        const glm::vec2 v0t = transformed[index.x];
        const glm::vec2 v1t = transformed[index.y];
        const glm::vec2 v2t = transformed[index.z];
        
        sf::Color color(-dot*255,0,0);

        mins = glm::clamp(glm::min(v0t, glm::min(v1t, v2t)) - glm::vec2(1.f), glm::vec2(0.f), image_size);
        maxs = glm::clamp(glm::max(v0t, glm::max(v1t, v2t)) + glm::vec2(1.f), glm::vec2(0.f), image_size);

        for(uint32_t x = mins.x; x < maxs.x; x++){
            for(uint32_t y = mins.y; y < maxs.y; y++){
                bayc = barycentric_(glm::vec2(x,y), v0t, v1t, v2t);
                if(glm::all(glm::greaterThanEqual(bayc, glm::vec3(0.f)))){
                    target.setPixel(glm::uvec2(x,y), color);
                }
            }
        }
    }
}

void sgl::render::drawTrianglesZBuffer(sgl::ColorTexture & target, sgl::DepthTexture& depth, const glm::mat4 transform, const std::vector<glm::vec3> vertex, const std::vector<glm::uvec3> indices)
{
    depth.clear(-FLT_MAX);
    glm::vec2 image_size = target.getSize();
    std::vector<glm::vec3> transformed(vertex.size(), glm::vec3());
    for(size_t i = 0; i < transformed.size(); i++){
        transformed[i] = (glm::vec3(transform * glm::vec4(vertex[i], 1.f)) + glm::vec3(1.f, 1.f, 0.f)) * glm::vec3(image_size,2.f) / 2.f;
        //target.setPixel(*transformed, sf::Color::Blue);
    }
    
    const glm::vec3 camera = glm::normalize(glm::vec3(transform[0][2],transform[1][2],transform[2][2]));

    glm::uvec2 mins, maxs;
    
    glm::vec3 bayc;

    for(size_t i= 0; i < indices.size(); i++){
        const glm::uvec3 index = indices[i];

        const glm::vec3 v0 = vertex[index.x];
        const glm::vec3 v1 = vertex[index.y];
        const glm::vec3 v2 = vertex[index.z];

        const glm::vec3 normal = glm::normalize(glm::cross(v1-v2,v1-v0));
        float dot = glm::dot(camera, normal);
        if(dot >= 0) continue;

        const glm::vec3 v0t = transformed[index.x];
        const glm::vec3 v1t = transformed[index.y];
        const glm::vec3 v2t = transformed[index.z];
        
        sf::Color color(-dot*255,0,0);

        mins = glm::clamp(glm::vec2(glm::min(v0t, glm::min(v1t, v2t))) - glm::vec2(1.f), glm::vec2(0.f), image_size);
        maxs = glm::clamp(glm::vec2(glm::max(v0t, glm::max(v1t, v2t))) + glm::vec2(1.f), glm::vec2(0.f), image_size);

        for(uint32_t x = mins.x; x < maxs.x; x++){
            for(uint32_t y = mins.y; y < maxs.y; y++){
                bayc = barycentric_(glm::vec2(x,y), v0t, v1t, v2t);
                if(glm::all(glm::greaterThanEqual(bayc, glm::vec3(0.f)))){
                    const glm::uvec2 impos(x,y);
                    const float dist = -(v0t * bayc.x + v1t * bayc.y + v2t * bayc.z).z;
                    if(dist > depth.getPixel(impos)){
                        depth.setPixel(impos, dist);
                        target.setPixel(impos, color);
                    }
                }
            }
        }
    }
}

