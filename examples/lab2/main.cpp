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
void SetOptimalFontSize(const ImGuiIO &io, const sf::VideoMode &desktop) {
    constexpr float baseFontSize = 18.0f; // Базовый размер шрифта
    const float scaleFactor = std::min(static_cast<float>(desktop.size.x) / 1920.0f,
                                       static_cast<float>(desktop.size.y) / 1080.0f);
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
    sf::RenderWindow window(sf::VideoMode(windowSize), "Lab 2");
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window)) {
        printf("Imgui SFML startup failed\n");
        return -1;
    }

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

        ImGui::Begin("Lab 2!");
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

        ImGui::SeparatorText("Lab 2");

        static int current_preview = 0;

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();


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
