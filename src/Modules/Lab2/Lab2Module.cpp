#include "Lab2Module.h"
#include "Render/triangles.h"
#include "Utility/FileUtils.h"
#include "imgui.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <random>
#include <vector>
#include <future>

namespace sgl {
    Lab2Module::Lab2Module() {
        generateTriangles(m_triangleCount);
    }

    std::string Lab2Module::getName() const {
        return "Лабораторная работа 2";
    }

    std::string Lab2Module::getDescription() const {
        return "Барицентрические координаты и отрисовка треугольников";
    }

    void Lab2Module::init() {
        generateTriangles(m_triangleCount);
    }

    void Lab2Module::update(const float deltaTime) {
        if (m_animateTriangles && m_currentPreview == PreviewMode::MultipleTriangles) {
            m_animationTime += deltaTime;

            // Обновляем положение треугольников для анимации
            for (size_t i = 0; i < m_triangles.size(); ++i) {
                const float angle = m_animationTime + (0.5f + static_cast<float>(i) * 0.2f);
                const float radius = 100.0f + static_cast<float>(i) * 20.0f;

                const sf::Vector2f center(256.0f, 256.0f); // Центр изображения
                m_triangles[i].v0 = center + sf::Vector2f(std::cos(angle) * radius, std::sin(angle) * radius);
                m_triangles[i].v1 = center + sf::Vector2f(std::cos(angle + 2.0f) * radius * 0.7f,
                                                          std::sin(angle + 2.0f) * radius * 0.7f);
                m_triangles[i].v2 = center + sf::Vector2f(std::cos(angle + 4.0f) * radius * 0.5f,
                                                          std::sin(angle + 4.0f) * radius * 0.5f);
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
                    drawTriangle(image, static_cast<int>(i));
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
                if (!m_triangles.empty()) {
                    ImGui::DragFloat2("Вершина 1", &m_triangles[m_activeTriangleIndex].v0.x, 1.0f, 0.0f, 512.0f);
                    ImGui::DragFloat2("Вершина 2", &m_triangles[m_activeTriangleIndex].v1.x, 1.0f, 0.0f, 512.0f);
                    ImGui::DragFloat2("Вершина 3", &m_triangles[m_activeTriangleIndex].v2.x, 1.0f, 0.0f, 512.0f);

                    float color[3] = {
                        m_triangles[m_activeTriangleIndex].color.r / 255.0f,
                        m_triangles[m_activeTriangleIndex].color.g / 255.0f,
                        m_triangles[m_activeTriangleIndex].color.b / 255.0f
                    };

                    if (ImGui::ColorEdit3("Цвет треугольника", color)) {
                        m_triangles[m_activeTriangleIndex].color.r = static_cast<uint8_t>(color[0] * 255.0f);
                        m_triangles[m_activeTriangleIndex].color.g = static_cast<uint8_t>(color[1] * 255.0f);
                        m_triangles[m_activeTriangleIndex].color.b = static_cast<uint8_t>(color[2] * 255.0f);
                    }
                }
                break;

            case PreviewMode::MultipleTriangles:
                ImGui::SliderInt("Количество треугольников", &m_triangleCount, 1, 100);
                if (ImGui::Button("Сгенерировать треугольники")) {
                    generateTriangles(m_triangleCount);
                }

                ImGui::Checkbox("Анимировать", &m_animateTriangles);
                break;

            case PreviewMode::ModelPolygons:
            case PreviewMode::ModelPolygonsWithLight:
            case PreviewMode::ModelPolygonsWithZBuffer:
                if (selectModel()) {
                    const int resolution = m_imageRef ? m_imageRef->getSize().x : 512;
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
        if (ImGui::Button("Выбрать модель")) {
            if (char filename[256]; sgl::utils::openOBJFileDialog(filename, sizeof(filename))) {
                if (std::ifstream file(filename); file.is_open()) {
                    m_currentModel = m_parser.parse(file);
                    file.close();
                    return true;
                }
            }
        }
        return false;
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

    void Lab2Module::drawTriangle(sgl::SFMLImage&image, const int index) const {
        if (m_triangles.empty() || index < 0 || index >= static_cast<int>(m_triangles.size())) return;
        const Triangle&tri = m_triangles[0];
        render::drawTriangle(image, tri.v0, tri.v1, tri.v2, tri.color);
    }

    void Lab2Module::drawModelPolygons(sgl::SFMLImage&image, bool useLight, bool useZBuffer) const {
        const int resolution = image.getSize().x;
        sf::Vector3f resol(resolution / 2, resolution / 2, resolution / 2);

        std::vector<sf::Vector3f> transformed_vertices;
        transformed_vertices.reserve(m_currentModel.get_vertex().size());

        // Преобразование вершин
        for (auto vtx = m_currentModel.beginVertices(); vtx != m_currentModel.endVertices(); ++vtx) {
            transformed_vertices.push_back((*vtx - m_modelCenter) * m_modelScale + resol);
        }

        // Z-буфер
        std::vector<float> zBuffer;
        if (useZBuffer) {
            zBuffer.resize(resolution * resolution, std::numeric_limits<float>::lowest());
        }

        // Отрисовка полигонов
        for (auto poly = m_currentModel.beginPolygons(); poly != m_currentModel.endPolygons(); ++poly) {
            const sf::Vector3f&v1 = transformed_vertices[(&poly->vertexIndices.x)[0]];
            const sf::Vector3f&v2 = transformed_vertices[(&poly->vertexIndices.x)[1]];
            const sf::Vector3f&v3 = transformed_vertices[(&poly->vertexIndices.x)[2]];

            sf::Color color = sf::Color::Green;

            if (useLight) {
                // Вычисление нормали к полигону
                sf::Vector3f normal = calculateNormal(v1, v2, v3);

                // Вычисление интенсивности освещения
                float intensity = calculateLightCosine(normal, m_lightDirection);

                // Ограничение интенсивности
                intensity = std::max(0.2f, intensity);

                // Применение интенсивности к цвету
                color.r = static_cast<uint8_t>(color.r * intensity);
                color.g = static_cast<uint8_t>(color.g * intensity);
                color.b = static_cast<uint8_t>(color.b * intensity);
            }

            // Отрисовка полигона
            sf::Vector2f v2d1(v1.x, v1.y);
            sf::Vector2f v2d2(v2.x, v2.y);
            sf::Vector2f v2d3(v3.x, v3.y);

            if (useZBuffer) {
                // Реализация отрисовки с Z-буфером
                float xmin = std::min({v2d1.x, v2d2.x, v2d3.x});
                float xmax = std::max({v2d1.x, v2d2.x, v2d3.x});
                float ymin = std::min({v2d1.y, v2d2.y, v2d3.y});
                float ymax = std::max({v2d1.y, v2d2.y, v2d3.y});

                // Проверка границ изображения
                xmin = std::max(0.0f, xmin);
                ymin = std::max(0.0f, ymin);
                xmax = std::min(static_cast<float>(resolution - 1), xmax);
                ymax = std::min(static_cast<float>(resolution - 1), ymax);

                for (int x = static_cast<int>(xmin); x <= static_cast<int>(xmax); ++x) {
                    for (int y = static_cast<int>(ymin); y <= static_cast<int>(ymax); ++y) {
                        sf::Vector3f bc = render::barycentric({x, y}, v2d1, v2d2, v2d3);

                        if (bc.x >= 0 && bc.y >= 0 && bc.z >= 0) {
                            // Интерполяция z-координаты
                            float z = v1.z * bc.x + v2.z * bc.y + v3.z * bc.z;

                            int bufferIndex = y * resolution + x;
                            if (bufferIndex >= 0 && bufferIndex < static_cast<int>(zBuffer.size()) && z > zBuffer[
                                    bufferIndex]) {
                                zBuffer[bufferIndex] = z;
                                image.setPixel({static_cast<unsigned>(x), static_cast<unsigned>(y)}, color);
                            }
                        }
                    }
                }
            }
            else {
                // Обычная отрисовка треугольника
                render::drawTriangle(image, v2d1, v2d2, v2d3, color);
            }
        }
    }

    void Lab2Module::generateTriangles(const int count) {
        m_triangles.clear();
        m_triangles.reserve(count);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distX(50.0f, 462.0f);
        std::uniform_real_distribution<float> distY(50.0f, 462.0f);
        std::uniform_int_distribution<int> distSize(20.0f, 100.0f);

        for (int i = 0; i < count; ++i) {
            Triangle tri;
            const float centerX = distX(gen);
            const float centerY = distY(gen);
            const float size = distSize(gen);

            tri.v0 = sf::Vector2f(centerX, centerY - size);
            tri.v1 = sf::Vector2f(centerX - size * 0.866f, centerY + size * 0.5f);
            tri.v2 = sf::Vector2f(centerX + size * 0.866f, centerY + size * 0.5f);
            tri.color = getRandomColor();

            m_triangles.push_back(tri);
        }
        m_animationTime = 0.0f;
    }

    sf::Color Lab2Module::getRandomColor() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(50, 255);

        return {
            static_cast<std::uint8_t>(dist(gen)), static_cast<std::uint8_t>(dist(gen)),
            static_cast<std::uint8_t>(dist(gen))
        };
    }

    sf::Vector3f Lab2Module::calculateNormal(const sf::Vector3f&v0, const sf::Vector3f&v1, const sf::Vector3f&v2) {
        const sf::Vector3f edge1 = v1 - v0;
        const sf::Vector3f edge2 = v2 - v0;

        // Векторное произведение
        sf::Vector3f normal;
        normal.x = edge1.y * edge2.z - edge1.z * edge2.y;
        normal.y = edge1.z * edge2.x - edge1.x * edge2.z;
        normal.z = edge1.x * edge2.y - edge1.y * edge2.x;

        // Нормализация
        if (float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z); length > 0) {
            normal.x /= length;
            normal.y /= length;
            normal.z /= length;
        }

        return normal;
    }

    float Lab2Module::calculateLightCosine(const sf::Vector3f&normal, const sf::Vector3f&light) {
        return normal.x * light.x + normal.y * light.y + normal.z * light.z;
    }
}
