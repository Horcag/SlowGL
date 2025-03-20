//
// Created by nikit on 3/19/25.
//
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <glm/gtc/matrix_transform.hpp>
#include "ModelViewController.h"


void ModelViewController::setModelBounds(const glm::vec3&mins, const glm::vec3&maxs) {
    modelCenter = (mins + maxs) * 0.5f;

    // Вычисляем масштаб для правильного отображения
    glm::vec3 size = maxs - mins;
    float maxSize = std::max(std::max(size.x, size.y), size.z);

    scale = viewportSize * 0.7f / maxSize;

    // Сброс вращение и центрирования
    rotation = glm::vec3(0.0f);
    translation = {viewportSize * 0.5f, viewportSize * 0.5f, 0.0f};
}

void ModelViewController::setViewportSize(float size) {
    viewportSize = size;
}

void ModelViewController::handleMousePress(const sf::Event&event, const bool ctrlPressed) {
    if (const auto* mouseButtonPressed = event.getIf<sf::Event::MouseButtonPressed>();
        mouseButtonPressed->button == sf::Mouse::Button::Left) {
        isRotating = !ctrlPressed;
        isTranslating = ctrlPressed;
        lastMousePos = {
            static_cast<float>(mouseButtonPressed->position.x),
            static_cast<float>(mouseButtonPressed->position.y)
        };
    }
}

void ModelViewController::handleMouseRelease(const sf::Event&event) {
    if (event.getIf<sf::Event::MouseButtonReleased>()) {
        isRotating = false;
        isTranslating = false;
    }
}

void ModelViewController::handleMouseMove(const sf::Event&event) {
    if (!isRotating && !isTranslating) return;

    const auto* mouseMove = event.getIf<sf::Event::MouseMoved>();
    glm::vec2 currentPos = {
        static_cast<float>(mouseMove->position.x),
        static_cast<float>(mouseMove->position.y)
    };
    glm::vec2 delta = currentPos - lastMousePos;

    if (isRotating) {
        // Вращение модели (движение по X меняет Y-ось, по Y - X-ось)
        rotation.y += delta.y * 0.5f;
        rotation.x += delta.y * 0.5f;

        // Ограничиваем углы для предотвращения переполнения
        while (rotation.y > 360.0f) rotation.y -= 360.0f;
        while (rotation.y < -360.0f) rotation.y += 360.0f;
        while (rotation.x > 360.0f) rotation.x -= 360.0f;
        while (rotation.x < -360.0f) rotation.x += 360.0f;
    }

    if (isTranslating) {
        // Перемещение модели
        translation.x += delta.x;
        translation.y += delta.y;
    }

    lastMousePos = currentPos;
}

void ModelViewController::zoom(float factor) {
    scale *= factor;
}

glm::mat4 ModelViewController::getTransformMatrix() const {
    glm::mat4 transform = glm::mat4(1.0f);

    // Модель находится в начале координат
    transform = glm::translate(transform, glm::vec3(translation.x, translation.y, 0.0f));

    // Затем применяем вращения
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // Масштабируем
    transform = glm::scale(transform, glm::vec3(scale));

    // Перемещаем центр модели в начало координат
    transform = glm::translate(transform, -modelCenter);

    return transform;
}

void ModelViewController::reset() {
    rotation = {0.0f, 0.0f, 0.0f};
    translation = {viewportSize * 0.5f, viewportSize * 0.5f, 0.0f};
}
