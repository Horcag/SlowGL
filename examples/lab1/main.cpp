#include <imgui-SFML.h>
#include <iostream>
#include <SFML/Window.hpp>

#include "SFML/Graphics/RenderWindow.hpp"
#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h
#include <SFML/Graphics.hpp>
#include "Inter-VariableFont.h"

// Функция для установки оптимального размера шрифта
void SetOptimalFontSize(ImGuiIO&io, const sf::VideoMode&desktop) {
    constexpr float baseFontSize = 18.0f; // Базовый размер шрифта
    const float scaleFactor = std::min(static_cast<float>(desktop.size.x) / 1920.0f,
                                       static_cast<float>(desktop.size.y) / 1080.0f);
    const float fontSize = baseFontSize * scaleFactor;

    // Загрузка шрифта с новым размером
    io.Fonts->Clear();
    ImFont* font = io.Fonts->AddFontFromMemoryTTF(
        Inter_VariableFont_opsz_wght_ttf,
        Inter_VariableFont_opsz_wght_ttf_len,
        fontSize);
    if (font == nullptr) {
        std::cerr << "Failed to load font!" << std::endl;
        return;
    }
    io.Fonts->Build();
    if (!ImGui::SFML::UpdateFontTexture()) {
        std::cerr << "Failed to update font texture!" << std::endl;
        return;
    }
}

void dotted_line(int x0, int y0, int x1, int y1) {
}

int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "My window");
    window.setFramerateLimit(60);

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();


    if (!ImGui::SFML::Init(window))
        return -1;

    ImGuiIO&io = ImGui::GetIO();
    SetOptimalFontSize(io, desktop);


    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock deltaClock;


    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::ShowDemoWindow();

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        sf::Vertex vertex{{10.f, 50.f}, sf::Color::Red, {100.f, 100.f}};
        // create an array of 3 vertices that define a triangle primitive
        sf::VertexArray triangle(sf::PrimitiveType::Triangles, 3);


        window.clear();
        // window.draw(shape);
        ImGui::SFML::Render(window);
        window.draw(vertex);
        window.display();
    }
    ImGui::SFML::Shutdown();
    return 0;
}
