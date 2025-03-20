//
// Created by nikit on 3/19/25.
//

#pragma once


#include <SFML/Graphics.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ModelViewController {
private:
    glm::vec3 rotation{0.0f};
    glm::vec3 translation{0.0f};
    float scale = 1.0f;

    glm::vec3 modelCenter{0.0f};

    // Текущее состояние взаимодействия
    bool isRotating = false;
    bool isTranslating = false;
    glm::vec2 lastMousePos{0.0f};

    float viewportSize = 512.0f; // Размер области просмотра (по умолчанию 512x512)

public:
    void setModelBounds(const glm::vec3&mins, const glm::vec3&maxs);

    void setViewportSize(float size);

    void handleMousePress(const sf::Event&event, const bool ctrlPressed);

    void handleMouseRelease(const sf::Event&event);

    void handleMouseMove(const sf::Event&event);

    void zoom(float factor);

    glm::mat4 getTransformMatrix() const;

    void reset();
};
