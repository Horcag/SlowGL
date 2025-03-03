#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics.hpp>
#include "render/lines.h"
#include "Image/SFMLImage.h"
#include "Debug/DebugView.h"
#include "Inter-VariableFont.h"
#include "Parsers/OBJParser/Model3D.h"
#include "Parsers/OBJParser/OBJParser.h"
#include <fstream>
#include <string>
#include <nfd.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <thread>
#include <future>
#include <algorithm> // std::min

void draw(sgl::SFMLImage &image, int64_t time) {
    image.clear();
    for (unsigned int i = 0; i < image.getSize().x; i++) {
        for (unsigned int j = 0; j < image.getSize().y; j++) {
            if ((i + time) % (j + 1) == 0) image.setPixel({i, j}, sf::Color::Blue);
        }
    }
    image.update();
}

struct LineMethodOptions {
    int dotted_line_dots = 100;
    int current_method = 0;
};

bool line_method_combo(LineMethodOptions &options) {
    static const char *methods[] = {
        "dotted_line", "dotted_line_v2", "x_loop_line", "x_loop_line_hotfix_1", "x_loop_line_hotfix_2",
        "x_loop_line_v2", "x_loop_line_v2_no_y_calc", "x_loop_line_v2_no_y_calc_for_some_unknown_reason",
        "bresenham line", "EFLA"
    };


    bool res = ImGui::Combo("Line method", &options.current_method, methods, IM_ARRAYSIZE(methods));

    if (options.current_method == 0) {
        if (ImGui::DragInt("Dots", &options.dotted_line_dots, 1.f, 16, 512))
            if (options.dotted_line_dots <= 0 || options.dotted_line_dots >= 2048)
                options.dotted_line_dots = 100;
    }

    return res;
}

void draw_line(sgl::SFMLImage &image, const LineMethodOptions &opts, const sf::Vector2i &start, const sf::Vector2i &end,
               sf::Color color) {
    switch (opts.current_method) {
        case 0:
            sgl::render::draw_dotted_line(image, start, end, color, opts.dotted_line_dots);
            break;
        case 1:
            sgl::render::draw_dotted_line_fix1(image, start, end, color);
            break;
        case 2:
            sgl::render::draw_dotted_line_loop_v1(image, start, end, color);
            break;
        case 3:
            sgl::render::draw_dotted_line_loop_fix1(image, start, end, color);
            break;
        case 4:
            sgl::render::draw_dotted_line_loop_fix2(image, start, end, color);
            break;
        case 5:
            sgl::render::draw_x_loop_dotted_line_v2(image, start, end, color);
            break;
        case 6:
            sgl::render::draw_x_loop_dotted_line_no_y(image, start, end, color);
            break;
        case 7:
            sgl::render::draw_x_loop_dotted_line_no_y_v2(image, start, end, color);
            break;
        case 8:
            sgl::render::draw_bresenham(image, start, end, color);
            break;
        case 9:
            sgl::render::draw_efla(image, start, end, color);
            break;
        default:
            break;
    }
}

// Функция для предварительного вычисления трансформаций вершин
inline std::vector<sf::Vector3f> compute_transformed_vertices(const Model3D &model, const sf::Vector3f &center,
                                                              const float scale,
                                                              const sf::Vector3f &resol) {
    std::vector<sf::Vector3f> transformed_vertices;
    transformed_vertices.reserve(model.get_vertex().size());
    for (auto vtx = model.beginVertices(); vtx != model.endVertices(); ++vtx) {
        transformed_vertices.push_back((*vtx - center) * scale + resol);
    }
    return transformed_vertices;
}

// Функция для рисования линий
void draw_polygon_lines(sgl::SFMLImage &image, const LineMethodOptions &opts,
                        const std::vector<sf::Vector3f> &transformed_vertices, const Model3D &model) {
    for (Model3D::PolyIterator poly = model.beginPolygons(); poly != model.endPolygons(); ++poly) {
        const sf::Vector3f &v1_tr = transformed_vertices[(&poly->vertexIndices.x)[0]];
        const sf::Vector3f &v2_tr = transformed_vertices[(&poly->vertexIndices.x)[1]];
        const sf::Vector3f &v3_tr = transformed_vertices[(&poly->vertexIndices.x)[2]];

        draw_line(image, opts, sf::Vector2i(v1_tr.x, v1_tr.y), sf::Vector2i(v2_tr.x, v2_tr.y), sf::Color::Green);
        draw_line(image, opts, sf::Vector2i(v2_tr.x, v2_tr.y), sf::Vector2i(v3_tr.x, v3_tr.y), sf::Color::Green);
        draw_line(image, opts, sf::Vector2i(v1_tr.x, v1_tr.y), sf::Vector2i(v3_tr.x, v3_tr.y), sf::Color::Green);
    }
}

void draw_polygon_chunk(
    sgl::SFMLImage &image,
    const LineMethodOptions &opts,
    const std::vector<sf::Vector3f> &tverts,
    const Model3D::PolyIterator beginPoly,
    const Model3D::PolyIterator endPoly) {
    for (auto poly = beginPoly; poly != endPoly; ++poly) {
        const auto &v1 = tverts[(&poly->vertexIndices.x)[0]];
        const auto &v2 = tverts[(&poly->vertexIndices.x)[1]];
        const auto &v3 = tverts[(&poly->vertexIndices.x)[2]];

        draw_line(image, opts, sf::Vector2i(v1.x, v1.y), sf::Vector2i(v2.x, v2.y), sf::Color::Green);
        draw_line(image, opts, sf::Vector2i(v2.x, v2.y), sf::Vector2i(v3.x, v3.y), sf::Color::Green);
        draw_line(image, opts, sf::Vector2i(v1.x, v1.y), sf::Vector2i(v3.x, v3.y), sf::Color::Green);
    }
}

void draw_polygons_multithreaded(
    sgl::SFMLImage &image,
    const LineMethodOptions &opts,
    const std::vector<sf::Vector3f> &transformed_vertices,
    const Model3D &model,
    const unsigned int threadCount) {
    const auto totalPolys = std::distance(model.beginPolygons(), model.endPolygons());
    if (totalPolys == 0) return;

    // Определяем размер чанка
    const unsigned int chunkSize = (totalPolys + threadCount - 1) / threadCount;

    std::vector<std::future<void> > tasks;
    tasks.reserve(threadCount);

    auto beginIt = model.beginPolygons();
    const auto endIt = model.endPolygons();

    for (unsigned int i = 0; i < threadCount && beginIt != endIt; i++) {
        auto chunkEnd = beginIt;
        std::advance(chunkEnd, std::min<unsigned int>(chunkSize, std::distance(chunkEnd, endIt)));

        tasks.push_back(std::async(
            std::launch::async,
            [&, beginIt, chunkEnd]() {
                draw_polygon_chunk(image, opts, transformed_vertices, beginIt, chunkEnd);
            }
        ));

        beginIt = chunkEnd;
    }

    for (auto &t: tasks) {
        t.get();
    }
}

bool model_select(char *filename) {
    ImGui::InputText("3D model", filename, 255, ImGuiInputTextFlags_CharsNoBlank);
    if (ImGui::Button("Select file")) {
        NFD::UniquePath outPath;
        constexpr nfdfilteritem_t filterItem[1] = {{"3D model", "obj"}};

        if (const nfdresult_t save_result = NFD::OpenDialog(outPath, filterItem, 1); save_result == NFD_OKAY) {
            strncpy(filename, outPath.get(), 255);
            return true;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Open")) return true;

    return false;
}

bool model_selector(Model3D &model) {
    static OBJParser parser;
    static char filename[255]{};
    if (model_select(filename)) {
        std::ifstream ifs(filename);
        model = parser.parse(ifs);
        ifs.close();
        return true;
    }
    return false;
}

void calc_model_scale(const Model3D &model, sf::Vector3f &center, float &factor, const int resolution) {
    sf::Vector3f mins{-FLT_MAX, -FLT_MAX, -FLT_MAX};
    sf::Vector3f maxs{FLT_MAX, FLT_MAX, FLT_MAX};
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
    factor = static_cast<float>(resolution) / std::max(std::max(std::abs(sz.x), std::abs(sz.y)), std::abs(sz.z)) / 2.f *
             0.7f;
}

const char *previews[] = {
    "1. Image manipulation", "2. Straight lines", "3. 3D Model", "4. 3D Model vertices", "5. 3D Model (Polygons)",
    "6. 3D Model edges"
};

// Функция для установки оптимального размера шрифта
void SetOptimalFontSize(ImGuiIO&io, const sf::VideoMode&desktop) {
    constexpr float baseFontSize = 15.0f; // Базовый размер шрифта
    const float scaleFactor = static_cast<float>(desktop.size.y) / 1080.0f;
    const float fontSize = baseFontSize * scaleFactor;

    ImFontConfig font_cfg{};
    font_cfg.FontDataOwnedByAtlas = false;

    // Загрузка шрифта с новым размером
    io.Fonts->Clear();
    const ImFont *font = io.Fonts->AddFontFromMemoryTTF(
        Inter_VariableFont_opsz_wght_ttf,
        Inter_VariableFont_opsz_wght_ttf_len,
        fontSize,
        &font_cfg);
    if (font == nullptr) {
        printf("Failed to load font!\n");
        return;
    }
    io.Fonts->Build();
    if (!ImGui::SFML::UpdateFontTexture()) {
        printf("Failed to update font texture!\n");
        return;
    }
}

int main() {
    NFD::Guard nfdGuard;

    sf::Vector2u windowSize{1280, 640};
    sf::RenderWindow window(sf::VideoMode(windowSize), "Lab 1");
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window)) {
        printf("Imgui SFML startup failed\n");
        return -1;
    }

    LineMethodOptions lineOptions;
    Model3D current_model;
    sf::Vector3f model_center{};
    float model_scale = 0;

    ImGuiIO &io = ImGui::GetIO();
    SetOptimalFontSize(io, desktop);

    int resolution = 16;
    sgl::SFMLImage image(sf::Vector2u(resolution, resolution), sf::Color::Black);
    sgl::DebugView debugView(sf::Vector2f(windowSize), 0.05f);

    sf::Clock deltaClock;
    //sf::Clock perfClock;
    //sf::Time perfTime;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto *resized = event->getIf<sf::Event::Resized>())
                windowSize = resized->size;
            if (!ImGui::GetIO().WantCaptureMouse && event.has_value()) {
                debugView.ProcessEvent(event.value());
            }
        }


        const sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        ImGui::ShowDemoWindow();

        ImGui::Begin("Lab 1!");
        ImGui::Text("FPS: %.1f", 1.0f / dt.asSeconds());
        //ImGui::Text("Render time: %lluus", perfTime.asMicroseconds());

        ImGui::SeparatorText("View");
        if (ImGui::Button("Reset view")) debugView.ResetView();
        ImGui::SameLine();
        if (ImGui::Button("-")) debugView.Zoom(1.5f);
        ImGui::SameLine();
        if (ImGui::Button("+")) debugView.Zoom(0.5f);
        ImGui::SeparatorText("Render");
        if (ImGui::DragInt("Resolution", &resolution, 1.f, 16, 512)) {
            if (resolution > 0 && resolution <= 4096) {
                image.resize(sf::Vector2u(resolution, resolution));
                calc_model_scale(current_model, model_center, model_scale, resolution);
            }
        }

        ImGui::SeparatorText("Lab 1");

        static int current_preview = 0;
        if (ImGui::Combo("Preview", &current_preview, previews, IM_ARRAYSIZE(previews)) && current_preview == 4) {
            lineOptions.current_method = 9;
        };

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        switch (current_preview) {
            case 0: {
                //1. Работа с изображениями.

                static int rgb[3] = {0, 0, 0};

                for (int i = 0; i < 3; i++) {
                    //if (i > 0) ImGui::SameLine();
                    ImGui::PushID(i);
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, static_cast<ImVec4>(ImColor::HSV(i / 3.0f, 0.5f, 0.5f)));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,
                                          static_cast<ImVec4>(ImColor::HSV(i / 3.0f, 0.6f, 0.5f)));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,
                                          static_cast<ImVec4>(ImColor::HSV(i / 3.0f, 0.7f, 0.5f)));
                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, static_cast<ImVec4>(ImColor::HSV(i / 3.0f, 0.9f, 0.9f)));

                    const char *labels[] = {"R", "G", "B"};
                    ImGui::SliderInt(labels[i], &rgb[i], 0, 255);

                    ImGui::PopStyleColor(4);
                    ImGui::PopID();
                }

                image.clear(sf::Color(rgb[0], rgb[1], rgb[2]));
                image.update();
            }
            break;
            case 1: {
                //2. Отрисовка прямых линий
                line_method_combo(lineOptions);

                image.clear();
                sf::Vector2f center(resolution / 2, resolution / 2);
                for (int i = 0; i <= 13; i++) {
                    double ang = (static_cast<double>(i) / 13. * 2.) * M_PI;
                    const sf::Vector2f offset = sf::Vector2f(cos(ang), sin(ang)) * static_cast<float>(resolution) *
                                                0.45f;
                    draw_line(image, lineOptions, sf::Vector2i(center), sf::Vector2i(center + offset),
                              sf::Color::White);
                }
                image.update();
                break;
            }
            case 2: {
                //3. Работа с трёхмерной моделью (вершины)
            }
            case 3: {
                //4. Отрисовка вершин трёхмерной модели
                if (model_selector(current_model)) {
                    calc_model_scale(current_model, model_center, model_scale, resolution);
                }

                image.clear();
                for (auto vtx = current_model.beginVertices(); current_model.endVertices() != vtx; ++vtx) {
                    sf::Vector3f transformed = (*vtx - model_center) * model_scale;
                    //printf("%f %f %f\n", transformed.x, transformed.y, transformed.z);
                    image.setPixel(sf::Vector2u(transformed.x + resolution / 2, transformed.z + resolution / 2),
                                   sf::Color::Magenta);
                }
                image.update();

                break;
            }
            case 4: {
                //5. Работа с трёхмерной моделью (полигоны)
                line_method_combo(lineOptions);

                if (model_selector(current_model)) {
                    calc_model_scale(current_model, model_center, model_scale, resolution);
                }

                sf::Vector3f resol(resolution / 2, resolution / 2, resolution / 2);
                image.clear();

                std::vector<sf::Vector3f> transformed_vertices = compute_transformed_vertices(
                    current_model, model_center, model_scale, resol);

                // Рисование линий
                // draw_polygon_lines(image, lineOptions, transformed_vertices, current_model);
                draw_polygons_multithreaded(image, lineOptions, transformed_vertices, current_model, 16);

                image.update();
                break;
            }
            case 5: //6. Отрисовка рёбер трёхмерной модели
            default:
                break;
        }

        ImGui::Spacing();

        ImGui::SeparatorText("Image");

        static char filename[255]{};
        ImGui::InputText("File", filename, 255, ImGuiInputTextFlags_CharsNoBlank);
        if (ImGui::Button("Save As")) {
            NFD::UniquePath outPath;
            nfdfilteritem_t filterItem[1] = {{"Image/png", "png"}};

            nfdresult_t save_result = NFD::SaveDialog(outPath, filterItem, 1, nullptr, "test.png");

            if (save_result == NFD_OKAY) {
                strncpy(filename, outPath.get(), 255);
                image.saveToFile(std::string(filename));
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            image.saveToFile(std::string(filename) + ".png");
        }

        ImGui::End();

        /*perfClock.restart();
        draw(image, mainClock.getElapsedTime().asMilliseconds());
        perfTime = perfClock.getElapsedTime();*/

        window.setView(debugView.getView());

        window.clear();
        window.draw(image);
        ImGui::SFML::Render(window);
        window.display();
    }

    NFD_Quit();
    ImGui::SFML::Shutdown();
    return 0;
}
