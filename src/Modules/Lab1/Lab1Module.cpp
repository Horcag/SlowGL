#include "Lab1Module.h"
#include "Render/lines.h"
#include "Utility/FileUtils.h"
#include "imgui.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <thread>
#include <future>
#define _USE_MATH_DEFINES
#include <math.h>

namespace sgl {
    Lab1Module::Lab1Module() = default;

    std::string Lab1Module::getName() const {
        return "Лабораторная работа 1";
    }

    std::string Lab1Module::getDescription() const {
        return "Работа с изображениями и 3D-моделями";
    }

    void Lab1Module::init() {
        // Инициализация модуля
    }

    void Lab1Module::update(float deltaTime) {
        // Логика обновления модуля
    }

    void Lab1Module::render(sgl::SFMLImage&image) {
        m_imageRef = &image;
        switch (m_currentPreview) {
            case PreviewMode::ImageManipulation:
                image.clear(sf::Color(m_rgb[0], m_rgb[1], m_rgb[2]));
                break;

            case PreviewMode::StraightLines:
                drawLines(image);
                break;

            case PreviewMode::ModelVertices:
                drawModelVertices(image);
                break;

            case PreviewMode::ModelPolygons:
                drawModelPolygons(image);
                break;

            case PreviewMode::ModelEdges:
                // TODO: реализация отрисовки рёбер
                break;

            default:
                break;
        }

        image.update();
    }

    void Lab1Module::renderUI() {
        static const char* previews[] = {
            "1. Работа с изображениями",
            "2. Прямые линии",
            "3. 3D Модель",
            "4. Вершины 3D-модели",
            "5. Полигоны 3D-модели",
            "6. Рёбра 3D-модели"
        };

        int currentPreview = static_cast<int>(m_currentPreview);
        if (ImGui::Combo("Режим отображения", &currentPreview, previews, IM_ARRAYSIZE(previews))) {
            m_currentPreview = static_cast<PreviewMode>(currentPreview);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        switch (m_currentPreview) {
            case PreviewMode::ImageManipulation: {
                for (int i = 0; i < 3; i++) {
                    ImGui::PushID(i);
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, static_cast<ImVec4>(ImColor::HSV(i / 3.0f, 0.5f, 0.5f)));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,
                                          static_cast<ImVec4>(ImColor::HSV(i / 3.0f, 0.6f, 0.5f)));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,
                                          static_cast<ImVec4>(ImColor::HSV(i / 3.0f, 0.7f, 0.5f)));
                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, static_cast<ImVec4>(ImColor::HSV(i / 3.0f, 0.9f, 0.9f)));

                    const char* labels[] = {"R", "G", "B"};
                    ImGui::SliderInt(labels[i], &m_rgb[i], 0, 255);

                    ImGui::PopStyleColor(4);
                    ImGui::PopID();
                }
                break;
            }
            case PreviewMode::StraightLines: {
                static const char* methods[] = {
                    "dotted_line", "dotted_line_v2", "x_loop_line", "x_loop_line_hotfix_1", "x_loop_line_hotfix_2",
                    "x_loop_line_v2", "x_loop_line_v2_no_y_calc", "x_loop_line_v2_no_y_calc_v2", "bresenham line",
                    "EFLA"
                };

                ImGui::Combo("Метод отрисовки линий", &m_lineOptions.current_method, methods, IM_ARRAYSIZE(methods));

                if (m_lineOptions.current_method == 0) {
                    ImGui::DragInt("Количество точек", &m_lineOptions.dotted_line_dots, 1.0f, 16, 512);
                }
                break;
            }
            case PreviewMode::Model3D:
            case PreviewMode::ModelVertices:
            case PreviewMode::ModelPolygons:
            case PreviewMode::ModelEdges: {
                if (selectModel()) {
                    int resolution = 512; // Получить текущее разрешение из изображения
                    calculateModelScale(m_currentModel, m_modelCenter, m_modelScale, resolution);
                }

                if (m_currentPreview == PreviewMode::ModelPolygons) {
                    static const char* methods[] = {
                        "dotted_line", "dotted_line_v2", "x_loop_line", "x_loop_line_hotfix_1", "x_loop_line_hotfix_2",
                        "x_loop_line_v2", "x_loop_line_v2_no_y_calc", "x_loop_line_v2_no_y_calc_v2", "bresenham line",
                        "EFLA"
                    };

                    ImGui::Combo("Метод отрисовки линий", &m_lineOptions.current_method, methods,
                                 IM_ARRAYSIZE(methods));

                    if (m_lineOptions.current_method == 0) {
                        ImGui::DragInt("Количество точек", &m_lineOptions.dotted_line_dots, 1.0f, 16, 512);
                    }
                }
                break;
            }
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

    void Lab1Module::handleEvent(const sf::Event&event) {
        // Обработка событий
    }

    void Lab1Module::resize(unsigned int width, unsigned int height) {
        calculateModelScale(m_currentModel, m_modelCenter, m_modelScale, width);
    }

    void Lab1Module::calculateModelScale(const Model3D&model, sf::Vector3f&center, float&factor, int resolution) {
        sf::Vector3f mins{
            std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest()
        };
        sf::Vector3f maxs{
            std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()
        };

        for (auto vtx = model.beginVertices(); vtx != model.endVertices(); ++vtx) {
            mins.x = std::max(mins.x, vtx->x);
            mins.y = std::max(mins.y, vtx->y);
            mins.z = std::max(mins.z, vtx->z);
            maxs.x = std::min(maxs.x, vtx->x);
            maxs.y = std::min(maxs.y, vtx->y);
            maxs.z = std::min(maxs.z, vtx->z);
        }

        center = (mins + maxs) / 2.f;
        const sf::Vector3f sz = (maxs - mins) / 2.f;
        factor = static_cast<float>(resolution) / std::max(std::max(std::abs(sz.x), std::abs(sz.y)), std::abs(sz.z)) /
                 2.f * 0.7f;
    }

    void Lab1Module::drawLines(sgl::SFMLImage&image) {
        image.clear();
        int resolution = image.getSize().x;
        sf::Vector2f center(resolution / 2, resolution / 2);

        for (int i = 0; i <= 13; i++) {
            double ang = (static_cast<double>(i) / 13. * 2.) * M_PI;
            const sf::Vector2f offset = sf::Vector2f(cos(ang), sin(ang)) * static_cast<float>(resolution) * 0.45f;

            sf::Vector2i start = sf::Vector2i(center);
            sf::Vector2i end = sf::Vector2i(center + offset);

            switch (m_lineOptions.current_method) {
                case 0:
                    render::draw_dotted_line(image, start, end, sf::Color::White, m_lineOptions.dotted_line_dots);
                    break;
                case 1:
                    render::draw_dotted_line_fix1(image, start, end, sf::Color::White);
                    break;
                case 2:
                    render::draw_dotted_line_loop_v1(image, start, end, sf::Color::White);
                    break;
                case 3:
                    render::draw_dotted_line_loop_fix1(image, start, end, sf::Color::White);
                    break;
                case 4:
                    render::draw_dotted_line_loop_fix2(image, start, end, sf::Color::White);
                    break;
                case 5:
                    render::draw_x_loop_dotted_line_v2(image, start, end, sf::Color::White);
                    break;
                case 6:
                    render::draw_x_loop_dotted_line_no_y(image, start, end, sf::Color::White);
                    break;
                case 7:
                    render::draw_x_loop_dotted_line_no_y_v2(image, start, end, sf::Color::White);
                    break;
                case 8:
                    render::draw_bresenham(image, start, end, sf::Color::White);
                    break;
                case 9:
                    render::draw_efla(image, start, end, sf::Color::White);
                    break;
                default:
                    break;
            }
        }
    }

    void Lab1Module::drawModelVertices(sgl::SFMLImage&image) {
        image.clear();
        int resolution = image.getSize().x;

        for (auto vtx = m_currentModel.beginVertices(); vtx != m_currentModel.endVertices(); ++vtx) {
            sf::Vector3f transformed = (*vtx - m_modelCenter) * m_modelScale;
            image.setPixel(sf::Vector2u(transformed.x + resolution / 2, transformed.z + resolution / 2),
                           sf::Color::Magenta);
        }
    }

    void Lab1Module::drawModelPolygons(sgl::SFMLImage&image) {
        image.clear();
        int resolution = image.getSize().x;
        sf::Vector3f resol(resolution / 2, resolution / 2, resolution / 2);

        std::vector<sf::Vector3f> transformed_vertices;
        transformed_vertices.reserve(m_currentModel.get_vertex().size());

        for (auto vtx = m_currentModel.beginVertices(); vtx != m_currentModel.endVertices(); ++vtx) {
            transformed_vertices.push_back((*vtx - m_modelCenter) * m_modelScale + resol);
        }

        // Многопоточная отрисовка полигонов
        const int threadCount = 16;
        const auto totalPolys = std::distance(m_currentModel.beginPolygons(), m_currentModel.endPolygons());
        if (totalPolys == 0) return;

        const unsigned int chunkSize = (totalPolys + threadCount - 1) / threadCount;

        std::vector<std::future<void>> tasks;
        tasks.reserve(threadCount);

        auto beginIt = m_currentModel.beginPolygons();
        const auto endIt = m_currentModel.endPolygons();

        for (int i = 0; i < threadCount && beginIt != endIt; i++) {
            auto chunkEnd = beginIt;
            std::advance(chunkEnd, std::min<int>(chunkSize, std::distance(chunkEnd, endIt)));

            tasks.push_back(std::async(
                std::launch::async,
                [this, &image, &transformed_vertices, beginIt, chunkEnd]() {
                    for (auto poly = beginIt; poly != chunkEnd; ++poly) {
                        const sf::Vector3f&v1 = transformed_vertices[(&poly->vertexIndices.x)[0]];
                        const sf::Vector3f&v2 = transformed_vertices[(&poly->vertexIndices.x)[1]];
                        const sf::Vector3f&v3 = transformed_vertices[(&poly->vertexIndices.x)[2]];

                        // Отрисовка линий с использованием выбранного метода
                        switch (m_lineOptions.current_method) {
                            case 0:
                                render::draw_dotted_line(image, sf::Vector2i(v1.x, v1.y), sf::Vector2i(v2.x, v2.y),
                                                         sf::Color::Green, m_lineOptions.dotted_line_dots);
                                render::draw_dotted_line(image, sf::Vector2i(v2.x, v2.y), sf::Vector2i(v3.x, v3.y),
                                                         sf::Color::Green, m_lineOptions.dotted_line_dots);
                                render::draw_dotted_line(image, sf::Vector2i(v3.x, v3.y), sf::Vector2i(v1.x, v1.y),
                                                         sf::Color::Green, m_lineOptions.dotted_line_dots);
                                break;
                            case 9: // EFLA, самый эффективный метод
                                render::draw_efla(image, sf::Vector2i(v1.x, v1.y), sf::Vector2i(v2.x, v2.y),
                                                  sf::Color::Green);
                                render::draw_efla(image, sf::Vector2i(v2.x, v2.y), sf::Vector2i(v3.x, v3.y),
                                                  sf::Color::Green);
                                render::draw_efla(image, sf::Vector2i(v3.x, v3.y), sf::Vector2i(v1.x, v1.y),
                                                  sf::Color::Green);
                                break;
                            default:
                                render::draw_bresenham(image, sf::Vector2i(v1.x, v1.y), sf::Vector2i(v2.x, v2.y),
                                                       sf::Color::Green);
                                render::draw_bresenham(image, sf::Vector2i(v2.x, v2.y), sf::Vector2i(v3.x, v3.y),
                                                       sf::Color::Green);
                                render::draw_bresenham(image, sf::Vector2i(v3.x, v3.y), sf::Vector2i(v1.x, v1.y),
                                                       sf::Color::Green);
                                break;
                        }
                    }
                }
            ));

            beginIt = chunkEnd;
        }

        for (auto&task: tasks) {
            task.get();
        }
    }

    bool Lab1Module::selectModel() {
        if (ImGui::Button("Выбрать модель")) {
            char filename[256];
            if (sgl::utils::openOBJFileDialog(filename, sizeof(filename))) {
                std::ifstream file(filename);
                if (file.is_open()) {
                    m_currentModel = m_parser.parse(file);
                    file.close();
                    return true;
                }
            }
        }
        return false;
    }
} // namespace sgl
