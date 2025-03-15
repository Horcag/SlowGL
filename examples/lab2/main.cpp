#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics.hpp>
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
#include "Texture/sgl_textures.h"
#include <glm/ext.hpp>
#include <glm/common.hpp>
#include "Utility/glm_converters.h"
#include "Render/triangles.h"
#include <iostream>
#include <gl/glew.h>
#include "Surface/BaseSurface.h"

void draw(sgl::SFMLImage&image, int64_t time) {
    image.clear();
    for (unsigned int i = 0; i < image.getSize().x; i++) {
        for (unsigned int j = 0; j < image.getSize().y; j++) {
            if ((i + time) % (j + 1) == 0) image.setPixel({i, j}, sf::Color::Blue);
        }
    }
    image.update();
}

// Функция для предварительного вычисления трансформаций вершин
inline std::vector<sf::Vector3f> compute_transformed_vertices(const Model3D&model, const sf::Vector3f&center,
                                                              const float scale,
                                                              const sf::Vector3f&resol) {
    std::vector<sf::Vector3f> transformed_vertices;
    transformed_vertices.reserve(model.get_vertex().size());
    for (auto vtx = model.beginVertices(); vtx != model.endVertices(); ++vtx) {
        transformed_vertices.push_back((*vtx - center) * scale + resol);
    }
    return transformed_vertices;
}


bool model_select(char* filename) {
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

void decompose_model(std::vector<glm::vec3>& vertexes, std::vector<glm::uvec3>& indices, const Model3D& model){
    const auto& model_vtx = model.get_vertex();
    vertexes.resize(model_vtx.size());
    for(size_t i = 0; i < model_vtx.size(); i++){
        vertexes[i] = glm_vec(model_vtx[i]);
    }
    const auto& model_faces = model.get_faces();
    indices.resize(model_faces.size());
    for(size_t i = 0; i < model_faces.size(); i++){
        indices[i] = glm_ivec(model_faces[i].vertexIndices);
    }
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

void calc_model_size(const Model3D&model, glm::vec3&mins, glm::vec3&maxs) {
    maxs = glm::vec3{-FLT_MAX, -FLT_MAX, -FLT_MAX};
    mins = glm::vec3{FLT_MAX, FLT_MAX, FLT_MAX};
    for (auto vtx = model.beginVertices(); vtx != model.endVertices(); ++vtx) {
        glm::vec3 vec = glm_vec(*vtx);
        mins = glm::min(mins, vec);
        maxs = glm::max(maxs, vec);
    }
}

const char* previews[] = {
    "1. Image manipulation", "2. Straight lines", "3. 3D Model", "4. 3D Model vertices", "5. 3D Model (Polygons)",
    "6. 3D Model edges"
};

// Функция для установки оптимального размера шрифта
void SetOptimalFontSize(const ImGuiIO&io, const sf::VideoMode&desktop) {
    constexpr float baseFontSize = 18.0f; // Базовый размер шрифта
    const float scaleFactor = std::min(static_cast<float>(desktop.size.x) / 1920.0f,
                                       static_cast<float>(desktop.size.y) / 1080.0f);
    const float fontSize = baseFontSize * scaleFactor;

    ImFontConfig font_cfg{};
    font_cfg.FontDataOwnedByAtlas = false;

    // Загрузка шрифта с новым размером
    io.Fonts->Clear();
    const ImFont* font = io.Fonts->AddFontFromMemoryTTF(
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

void drawTrianglesBuffer(sgl::BaseSurface& target, const glm::mat4 transform, const std::vector<glm::vec3> vertex, const std::vector<glm::uvec3> indices)
{
    glm::vec2 image_size = target.getSize();
    static std::vector<glm::vec2> transformed;
    transformed.resize(vertex.size());
    for(size_t i = 0; i < vertex.size(); i++){
        transformed[i] = (glm::vec2(transform * glm::vec4(vertex[i], 1.f)) + glm::vec2(1.f, 1.f)) * image_size / 2.f;
        //target.setPixel(*transformed, sf::Color::Blue);
    }
    
    for(size_t i= 0; i < indices.size(); i++){
        const glm::uvec3 index = indices[i];
        const glm::vec2 v0t = transformed[index.x];
        const glm::vec2 v1t = transformed[index.y];
        const glm::vec2 v2t = transformed[index.z];
        
        target.drawLine(v0t, v1t, sf::Color::Red);
        target.drawLine(v1t, v2t, sf::Color::Green);
        target.drawLine(v0t, v2t, sf::Color::Blue);
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

    glewInit();

    /*sf::ContextSettings settings = window.getSettings();

    std::cout << "depth bits:" << settings.depthBits << std::endl;
    std::cout << "stencil bits:" << settings.stencilBits << std::endl;
    //std::cout << "antialiasing level:" << settings.antialiasingLevel << std::endl;
    std::cout << "version:" << settings.majorVersion << "." << settings.minorVersion << std::endl;*/

    Model3D current_model;
    std::vector<glm::vec3> model_vtx;
    std::vector<glm::uvec3> model_vtx_indices;
    glm::mat4 transform_matrix = glm::ortho(-1.f, 1.f, -1.f, 1.f);

    ImGuiIO&io = ImGui::GetIO();
    SetOptimalFontSize(io, desktop);

    int resolution = 128;

    sgl::BaseSurface surface(resolution, resolution);

    //sf::Texture texture(sf::Vector2u{(uint32_t)resolution, (uint32_t)resolution});
    sf::Sprite sprite(surface.getTexture());
    sgl::ColorTexture diffuseColor(resolution, resolution);
    sgl::DepthTexture depthTexture(resolution, resolution);

    sgl::DebugView debugView(sf::Vector2f(windowSize), 0.05f);

    sf::Clock deltaClock;
    sf::Clock timeClock;
    timeClock.start();
    //sf::Time perfTime;
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
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
            if (resolution > 0 && resolution <= 8192) {
                //texture.resize(sf::Vector2u(resolution, resolution));
                surface.resize(resolution, resolution);
                diffuseColor.resize(resolution, resolution);
                depthTexture.resize(resolution, resolution);
                sprite.setTexture(surface.getTexture(), true);
                //sprite.setTexture(texture, true);
                //image.resize(sf::Vector2u(resolution, resolution));
                //calc_model_scale(current_model, model_center, model_scale, resolution);
            }
        }

        ImGui::SeparatorText("Lab 2");

        if (model_selector(current_model)) {
            glm::vec3 mins, maxs;
            calc_model_size(current_model, mins, maxs);
            printf("%f %f %f\n", mins.x, mins.y, mins.z);
            printf("%f %f %f\n", maxs.x, maxs.y, maxs.z);
            mins *= 1.3f;
            maxs *= 1.3f;
            transform_matrix = glm::ortho(mins.x, maxs.x, mins.y, maxs.y);
            decompose_model(model_vtx, model_vtx_indices, current_model);
            //calc_model_scale(current_model, model_center, model_scale, resolution);
        }

        ImGui::End();

        surface.clear(sf::Color::Black);

        drawTrianglesBuffer(surface, transform_matrix, model_vtx, model_vtx_indices);
        surface.flush();

        /*glm::mat4 rot = glm::rotate(glm::mat4(1.f), glm::radians(timeClock.getElapsedTime().asSeconds() * 10), glm::vec3(0,1,0));
        glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(100,0,0));

        glm::mat4 mv = transform_matrix * rot;

        diffuseColor.clear(0xff090909u);
        sgl::render::drawTrianglesZBuffer(diffuseColor, depthTexture, mv, model_vtx, model_vtx_indices);

        diffuseColor.drawTo(texture);*/

        window.setView(debugView.getView());

        window.clear();
        window.draw(sprite);
        ImGui::SFML::Render(window);
        window.display();
    }

    //glDeleteFramebuffers(1, &framebuffer);

    NFD_Quit();
    ImGui::SFML::Shutdown();
    return 0;
}
