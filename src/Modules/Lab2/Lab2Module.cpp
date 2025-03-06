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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

                constexpr sf::Vector2f center(256.0f, 256.0f); // Центр изображения
                m_triangles[i].v0 = center + sf::Vector2f(std::cos(angle) * radius, std::sin(angle) * radius);
                m_triangles[i].v1 = center + sf::Vector2f(std::cos(angle + 2.0f) * radius * 0.7f,
                                                          std::sin(angle + 2.0f) * radius * 0.7f);
                m_triangles[i].v2 = center + sf::Vector2f(std::cos(angle + 4.0f) * radius * 0.5f,
                                                          std::sin(angle + 4.0f) * radius * 0.5f);
            }
        }

        if (m_animateModel) {
            m_rotationX += 45.0f * deltaTime;
            if (m_rotationX >= 360.0f) {
                m_rotationX -= 360.0f;
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
        if (m_currentPreview >= PreviewMode::ModelPolygons) {
            ImGui::SeparatorText("Вращение модели");
            ImGui::SliderFloat("Поворот X", &m_rotationX, 0.0f, 360.0f);
            ImGui::SliderFloat("Поворот Y", &m_rotationY, 0.0f, 360.0f);
            ImGui::SliderFloat("Поворот Z", &m_rotationZ, 0.0f, 360.0f);

            if (ImGui::Button("Сбросить вращение")) {
                m_rotationX = m_rotationY = m_rotationZ = 0.0f;
            }

            ImGui::Checkbox("Автоматическое вращение", &m_animateModel);
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
        const Triangle&tri = m_triangles[index];
        render::drawTriangle(image, tri.v0, tri.v1, tri.v2, tri.color);
    }


    void Lab2Module::drawModelPolygons(sgl::SFMLImage&image, bool useLight, bool useZBuffer) const {
        image.clear();
        const int resolution = image.getSize().x;
        const sf::Vector3f resolutionCenter(resolution / 2.0f, resolution / 2.0f, resolution / 2.0f);

        std::vector<float> zBuffer;
        if (useZBuffer) {
            zBuffer.assign(resolution * resolution, std::numeric_limits<float>::max());
        }

        // Рассчитываем вершины с учётом вращения
        std::vector<sf::Vector3f> transformedVertices;
        transformedVertices.reserve(m_currentModel.get_vertex().size());

        // Подготавливаем вершины модели
        for (auto vtx = m_currentModel.beginVertices(); vtx != m_currentModel.endVertices(); ++vtx) {
            // Сначала смещаем относительно центра модели
            sf::Vector3f centered = *vtx - m_modelCenter;

            // Применяем масштабирование
            sf::Vector3f scaled = centered * m_modelScale;

            // Применяем вращение
            sf::Vector3f transformed = applyRotation(scaled + resolutionCenter);

            transformedVertices.push_back(transformed);
        }

        // Отрисовываем полигоны
        for (auto poly = m_currentModel.beginPolygons(); poly != m_currentModel.endPolygons(); ++poly) {
            const sf::Vector3f&v1 = transformedVertices[poly->vertexIndices.x];
            const sf::Vector3f&v2 = transformedVertices[poly->vertexIndices.y];
            const sf::Vector3f&v3 = transformedVertices[poly->vertexIndices.z];

            sf::Vector3f normal = calculateNormal(v1, v2, v3);
            // Определяем цвет полигона
            sf::Color color;
            if (useLight) {
                float intensity = calculateLightCosine(normal, m_lightDirection);

                if (intensity >= 0) {
                    continue;
                }
                intensity = std::abs(intensity);
                intensity = std::max(0.2f, intensity); // минимальное освещение
                color = sf::Color(255 * intensity, 255 * intensity, 255 * intensity);
            }
            else {
                // Если не используем освещение, генерируем случайный цвет на основе индекса полигона
                color = sf::Color(
                    (poly->vertexIndices.x * 73) % 200 + 55,
                    (poly->vertexIndices.y * 47) % 200 + 55,
                    (poly->vertexIndices.z * 13) % 200 + 55
                );
            }

            if (useZBuffer) {
                drawTriangleWithZBuffer(image, v1, v2, v3, color, zBuffer, resolution);
            }
            else {
                // Обычная отрисовка треугольника
                sf::Vector2f p1(v1.x, v1.y);
                sf::Vector2f p2(v2.x, v2.y);
                sf::Vector2f p3(v3.x, v3.y);
                sgl::render::drawTriangle(image, p1, p2, p3, color);
            }
        }
    }

    void Lab2Module::generateTriangles(const int count) {
        m_triangles.clear();

        // Получаем размер изображения для корректной генерации координат
        const int resolution = m_imageRef ? m_imageRef->getSize().x : 512;
        const float padding = static_cast<float>(resolution) * 0.1f; // отступ от краев

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(padding, resolution - padding);

        for (int i = 0; i < count; ++i) {
            Triangle tri;

            tri.v0 = sf::Vector2f(dist(gen), dist(gen));
            tri.v1 = sf::Vector2f(dist(gen), dist(gen));
            tri.v2 = sf::Vector2f(dist(gen), dist(gen));
            tri.color = getRandomColor();
            m_triangles.push_back(tri);
        }
        m_activeTriangleIndex = m_triangles.empty() ? 0 : 0;
    }

    sf::Color Lab2Module::getRandomColor() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
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
        if (const float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            length > 0) {
            normal.x /= length;
            normal.y /= length;
            normal.z /= length;
        }

        return normal;
    }

    float Lab2Module::calculateLightCosine(const sf::Vector3f&normal, const sf::Vector3f&light) {
        return normal.x * light.x + normal.y * light.y + normal.z * light.z;
    }

    sf::Vector3f Lab2Module::applyRotation(const sf::Vector3f&point) const {
        const int resolution = m_imageRef->getSize().x;
        // Создаем единичную матрицу
        glm::mat4 rotationMatrix = glm::mat4(1.0f);

        // Центр вращения - это центр экрана
        glm::vec3 center(resolution / 2.0f, resolution / 2.0f, resolution / 2.0f);

        // Смещаем к началу координат
        glm::mat4 translateToOrigin = glm::translate(glm::mat4(1.0f), -center);

        // Поворачиваем вокруг осей (радианы)
        glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

        // Смещаем обратно
        glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), center);

        // Комбинируем матрицы
        rotationMatrix = translateBack * rotateZ * rotateY * rotateX * translateToOrigin;

        // Применяем трансформацию к точке
        glm::vec4 glmPoint(point.x, point.y, point.z, 1.0f);
        glm::vec4 transformed = rotationMatrix * glmPoint;

        return sf::Vector3f(transformed.x, transformed.y, transformed.z);
    }


    void Lab2Module::drawTriangleWithZBuffer(SFMLImage&image, const sf::Vector3f&v0,
                                             const sf::Vector3f&v1, const sf::Vector3f&v2, const sf::Color&color,
                                             std::vector<float>&zBuffer,
                                             int resolution) {
        int minX = std::max(0, static_cast<int>(std::min({v0.x, v1.x, v2.x})));
        int maxX = std::min(resolution - 1, static_cast<int>(std::max({v0.x, v1.x, v2.x})));
        int minY = std::max(0, static_cast<int>(std::min({v0.y, v1.y, v2.y})));
        int maxY = std::min(resolution - 1, static_cast<int>(std::max({v0.y, v1.y, v2.y})));

        // Обходим все пиксели в ограничивающем прямоугольнике
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                sf::Vector2i point(x, y);

                const sf::Vector3f bary = sgl::render::barycentric(
                    point,
                    sf::Vector2f(v0.x, v0.y),
                    sf::Vector2f(v1.x, v1.y),
                    sf::Vector2f(v2.x, v2.y)
                );

                const float lambda0 = bary.x;
                const float lambda1 = bary.y;
                const float lambda2 = bary.z;


                // Проверяем, находится ли точка внутри треугольника
                if (lambda0 >= 0 && lambda1 >= 0 && lambda2 >= 0) {
                    float z = lambda0 * v0.z + lambda1 * v1.z + lambda2 * v2.z;

                    int index = y * resolution + x;
                    if (z < zBuffer[index]) {
                        zBuffer[index] = z;
                        image.setPixel({static_cast<unsigned int>(x), static_cast<unsigned int>(y)}, color);
                    }
                }
            }
        }
    }
}
