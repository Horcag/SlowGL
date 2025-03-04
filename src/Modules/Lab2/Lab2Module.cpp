#include "Lab2Module.h"
#include "Render/triangles.h"
#include "Utility/FileUtils.h"
#include "imgui.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include <future>

namespace sgl {
    Lab2Module::Lab2Module() = default;

    std::string Lab2Module::getName() const {
        return "Лабораторная работа 2";
    }

    std::string Lab2Module::getDescription() const {
        return "Барицентрические координаты и отрисовка треугольников";
    }

    void Lab2Module::init() {
        generateTriangles(m_triangleCount);
    }

    void Lab2Module::update(float deltaTime) {
        if (m_animateTriangles) {
            m_animationTime += deltaTime;

            // Обновляем положение треугольников для анимации
            for (size_t i = 0; i < m_triangles.size(); ++i) {
                float angle = m_animationTime + i * 0.5f;
                float radius = 100.0f;

                Triangle&t = m_triangles[i];
                sf::Vector2f center = {250.0f, 250.0f};
                sf::Vector2f offset = {
                    std::cos(angle) * radius,
                    std::sin(angle) * radius
                };

                sf::Vector2f originalSize = (t.v1 - t.v0) * 0.5f;

                t.v0 = center + offset - originalSize;
                t.v1 = center + offset + originalSize;
                t.v2 = {t.v0.x + 2 * originalSize.x, t.v1.y};
            }
        }
    }

    void Lab2Module::render(sgl::SFMLImage&image) {
        m_imageRef = &image;
        image.clear();

        switch (m_currentPreview) {
            case PreviewMode::SingleTriangle:
                drawTriangle(image, m_activeTriangleIndex);
                break;

            case PreviewMode::MultipleTriangles:
                for (size_t i = 0; i < m_triangles.size(); ++i) {
                    drawTriangle(image, i);
                }
                break;

            case PreviewMode::ModelPolygons:
                drawModelPolygons(image, false, false);
                break;

            case PreviewMode::ModelPolygonsWithLight:
                drawModelPolygons(image, true, false);
                break;

            case PreviewMode::ModelPolygonsWithZBuffer:
                drawModelPolygons(image, true, true);
                break;
        }

        image.update();
    }

    void Lab2Module::renderUI() {
        static const char* previews[] = {
            "1. Один треугольник",
            "2. Несколько треугольников",
            "3. Полигоны 3D-модели (случайные цвета)",
            "4. Полигоны 3D-модели (с освещением)",
            "5. Полигоны 3D-модели (с Z-буфером)"
        };

        int currentPreview = static_cast<int>(m_currentPreview);
        if (ImGui::Combo("Режим отображения", &currentPreview, previews, IM_ARRAYSIZE(previews))) {
            m_currentPreview = static_cast<PreviewMode>(currentPreview);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        switch (m_currentPreview) {
            case PreviewMode::SingleTriangle:
                if (ImGui::Button("Сгенерировать новый треугольник")) {
                    generateTriangles(1);
                }
                break;

            case PreviewMode::MultipleTriangles:
                ImGui::SliderInt("Количество треугольников", &m_triangleCount, 1, 20);
                if (ImGui::Button("Сгенерировать треугольники")) {
                    generateTriangles(m_triangleCount);
                }

                ImGui::Checkbox("Анимировать", &m_animateTriangles);
                break;

            case PreviewMode::ModelPolygons:
            case PreviewMode::ModelPolygonsWithLight:
            case PreviewMode::ModelPolygonsWithZBuffer:
                if (selectModel()) {
                    int resolution = 512;
                    calculateModelScale(m_currentModel, m_modelCenter, m_modelScale, resolution);
                }

                if (m_currentPreview == PreviewMode::ModelPolygonsWithLight ||
                    m_currentPreview == PreviewMode::ModelPolygonsWithZBuffer) {
                    ImGui::Text("Направление света:");
                    ImGui::SliderFloat("X##light", &m_lightDirection.x, -1.0f, 1.0f);
                    ImGui::SliderFloat("Y##light", &m_lightDirection.y, -1.0f, 1.0f);
                    ImGui::SliderFloat("Z##light", &m_lightDirection.z, -1.0f, 1.0f);

                    if (ImGui::Button("Сбросить")) {
                        m_lightDirection = {0.0f, 0.0f, 1.0f};
                    }
                }
                break;
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Изображение");

        ImGui::InputText("Файл", m_filename, sizeof(m_filename), ImGuiInputTextFlags_CharsNoBlank);
        if (ImGui::Button("Сохранить как")) {
            if (sgl::utils::saveImageDialog(m_filename, sizeof(m_filename), "test.png") && m_imageRef) {
                sgl::utils::saveImage(*m_imageRef, m_filename);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Сохранить")) {
            if (m_filename[0] != '\0' && m_imageRef) {
                sgl::utils::saveImage(*m_imageRef, m_filename);
            }
        }
    }

    void Lab2Module::handleEvent(const sf::Event&event) {
        // Пусто для этого модуля
    }

    void Lab2Module::resize(unsigned int width, unsigned int height) {
        calculateModelScale(m_currentModel, m_modelCenter, m_modelScale, width);
    }

    bool Lab2Module::selectModel() {
    }

    void Lab2Module::calculateModelScale(const Model3D&model, sf::Vector3f&center, float&factor, int resolution) {
        if (model.get_vertex().empty()) return;

        sf::Vector3f mins{
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        };
        sf::Vector3f maxs{
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest()
        };

        for (auto vtx = model.beginVertices(); vtx != model.endVertices(); ++vtx) {
            mins.x = std::min(mins.x, vtx->x);
            mins.y = std::min(mins.y, vtx->y);
            mins.z = std::min(mins.z, vtx->z);
            maxs.x = std::max(maxs.x, vtx->x);
            maxs.y = std::max(maxs.y, vtx->y);
            maxs.z = std::max(maxs.z, vtx->z);
        }

        center = (mins + maxs) / 2.f;
        const sf::Vector3f sz = (maxs - mins) / 2.f;
        factor = static_cast<float>(resolution) / std::max(std::max(std::abs(sz.x), std::abs(sz.y)), std::abs(sz.z)) /
                 2.f * 0.7f;
    }

    void Lab2Module::drawTriangle(sgl::SFMLImage&image, int index) {
        if (index < 0 || index >= static_cast<int>(m_triangles.size())) {
            if (!m_triangles.empty()) {
                const Triangle&t = m_triangles[0];
                render::drawTriangle(image, t.v0, t.v1, t.v2, t.color);
            }
            return;
        }

        const Triangle&t = m_triangles[index];
        render::drawTriangle(image, t.v0, t.v1, t.v2, t.color);
    }

    void Lab2Module::drawModelPolygons(sgl::SFMLImage&image, bool useLight, bool useZBuffer) {
    }

    void Lab2Module::generateTriangles(int count) {
    }

    sf::Color Lab2Module::getRandomColor() {
    }

    sf::Vector3f Lab2Module::calculateNormal(const sf::Vector3f&v0, const sf::Vector3f&v1, const sf::Vector3f&v2) {
    }

    float Lab2Module::calculateLightCosine(const sf::Vector3f&normal, const sf::Vector3f&light) {
    }
}
