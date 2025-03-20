#include "Lab3Module.h"
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
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


namespace sgl {
    Lab3Module::Lab3Module() {
        //
    }

    std::string Lab3Module::getName() const {
        return "Лабораторная работа 3";
    }

    std::string Lab3Module::getDescription() const {
        return "Отрисовка модели";
    }

    void Lab3Module::init() {
        if (!m_modelController) {
            std::cerr << "ModelViewController не инициализирован!" << std::endl;
            return;
        }
        m_modelController->setViewportSize(512);
    }


    void Lab3Module::render(sgl::SFMLImage&image) {
        m_imageRef = &image;
        image.clear();
        drawModelPolygons(image, true, true);
        image.update();
    }

    void Lab3Module::renderUI() {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (selectModel()) {
            const int resolution = m_imageRef ? m_imageRef->getSize().x : 512;
            calculateModelScale(m_currentModel, m_modelCenter, m_modelScale, resolution);
        }
        ImGui::Text("Направление света:");
        ImGui::SliderFloat("X##light", &m_lightDirection.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Y##light", &m_lightDirection.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Z##light", &m_lightDirection.z, -1.0f, 1.0f);

        if (ImGui::Button("Сбросить")) {
            m_lightDirection = {0.0f, 0.0f, 1.0f};
        }
        if (ImGui::Button("Сбросить вращение")) {
            m_modelController->reset();
        }

        ImGui::SliderFloat("FOV", &m_fov, 30.f, 120.f);
        ImGui::Spacing();
        ImGui::SeparatorText("Изображение");

        ImGui::InputText("Файл", m_filename, sizeof (m_filename), ImGuiInputTextFlags_CharsNoBlank);
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

    void Lab3Module::handleEvent(const sf::Event&event) {
        //
    }


    void Lab3Module::update(float deltaTime) {
        //
    }

    void Lab3Module::resize(unsigned int width, unsigned int height) {
        calculateModelScale(m_currentModel, m_modelCenter, m_modelScale, width);
    }

    bool Lab3Module::selectModel() {
        if (ImGui::Button("Выбрать модель")) {
            if (char filename[256]; sgl::utils::openOBJFileDialog(filename, sizeof(filename))) {
                if (std::ifstream file(filename); file.is_open()) {
                    m_currentModel = m_parser.parse(file);
                    file.close();

                    glm::vec3 mins(FLT_MAX), maxs(-FLT_MAX);

                    for (auto vtx = m_currentModel.beginVertices(); vtx != m_currentModel.endVertices(); ++vtx) {
                        mins.x = std::min(mins.x, vtx->x);
                        mins.y = std::min(mins.y, vtx->y);
                        mins.z = std::min(mins.z, vtx->z);

                        maxs.x = std::max(maxs.x, vtx->x);
                        maxs.y = std::max(maxs.y, vtx->y);
                        maxs.z = std::max(maxs.z, vtx->z);
                    }

                    // Явный вызов setModelBounds после загрузки модели
                    if (m_modelController) {
                        m_modelController->setModelBounds(mins, maxs);
                    }
                    return true;
                }
            }
        }
        return false;
    }

    void Lab3Module::calculateModelScale(const Model3D&model, sf::Vector3f&center, float&factor, int resolution) {
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

    void Lab3Module::drawModelPolygons(sgl::SFMLImage&image, bool useLight, bool useZBuffer) const {
        image.clear();
        const int resolution = image.getSize().x;

        std::vector<float> zBuffer;
        if (useZBuffer) {
            zBuffer.assign(resolution * resolution, std::numeric_limits<float>::max());
        }

        // Рассчитываем вершины с учётом вращения
        std::vector<sf::Vector3f> transformedVertices;
        transformedVertices.reserve(m_currentModel.get_vertex().size());

        const glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 5.0f), // Позиция камеры
            glm::vec3(0.0f, 0.0f, 0.0f), // Куда смотрит камера
            glm::vec3(0.0f, 1.0f, 0.0f) // Вектор "вверх"
        );
        const float aspectRatio = 1.0f;
        const glm::mat4 projection = glm::perspective(glm::radians(m_fov), aspectRatio, 0.1f, 100.0f);

        // Получаем матрицу трансформации от контроллера
        const glm::mat4 model = m_modelController->getTransformMatrix();
        const glm::mat4 mvp = projection * view * model;

        // Подготавливаем вершины модели
        for (auto vtx = m_currentModel.beginVertices(); vtx != m_currentModel.endVertices(); ++vtx) {
            glm::vec4 pos(vtx->x, vtx->y, vtx->z, 1.0f);
            const glm::vec4 transformed = mvp * pos;

            // Преобразование из однородных координат в NDC (разделить на w)
            // float ndcX = transformed.x / transformed.w;
            // float ndcY = transformed.y / transformed.w;
            // float ndcZ = transformed.z / transformed.w;
            float ndcX = transformed.x;
            float ndcY = transformed.y;
            float ndcZ = transformed.z;

            transformedVertices.emplace_back(ndcX, ndcY, ndcZ);
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

                if (intensity < 0) {
                    intensity = std::abs(intensity);
                    intensity = std::max(0.2f, intensity);
                    color = sf::Color(255 * intensity, 255 * intensity, 255 * intensity);
                }
                else {
                    // Для задней части модели используем более тёмный цвет или пропускаем
                    continue; // или используйте более тёмный цвет
                }
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
        }
    }

    sf::Vector3f Lab3Module::calculateNormal(const sf::Vector3f&v0, const sf::Vector3f&v1, const sf::Vector3f&v2) {
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

    float Lab3Module::calculateLightCosine(const sf::Vector3f&normal, const sf::Vector3f&light) {
        return normal.x * light.x + normal.y * light.y + normal.z * light.z;
    }


    void Lab3Module::drawTriangleWithZBuffer(SFMLImage&image, const sf::Vector3f&v0,
                                             const sf::Vector3f&v1, const sf::Vector3f&v2, const sf::Color&color,
                                             std::vector<float>&zBuffer,
                                             const int resolution) {
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
                    float z = -(lambda0 * v0.z + lambda1 * v1.z + lambda2 * v2.z);

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
