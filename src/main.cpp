#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "Core/ModuleManager.h"
#include "Image/SFMLImage.h"
#include "Debug/DebugView.h"
#include "Inter-VariableFont.h"
#include "Modules/Lab1/Lab1Module.h"
#include <nfd.hpp>
#include "Modules/Lab2/Lab2Module.h"

void SetOptimalFontSize(const ImGuiIO&io, const sf::VideoMode&desktop) {
    constexpr float baseFontSize = 18.0f;
    const float scaleFactor = std::min(static_cast<float>(desktop.size.x) / 1920.0f,
                                       static_cast<float>(desktop.size.y) / 1080.0f);
    const float fontSize = baseFontSize * scaleFactor;

    ImFontConfig font_cfg{};
    font_cfg.FontDataOwnedByAtlas = false;
    io.Fonts->Clear();
    const ImFont* font = io.Fonts->AddFontFromMemoryTTF(
        Inter_VariableFont_opsz_wght_ttf,
        Inter_VariableFont_opsz_wght_ttf_len,
        fontSize,
        &font_cfg,
        ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

    if (font == nullptr) {
        printf("Failed to load font!\n");
        return;
    }
    io.Fonts->Build();
    if (!ImGui::SFML::UpdateFontTexture()) {
        printf("Failed to update font texture!\n");
    }
}

int main() {
    std::setlocale(LC_ALL, "");
    NFD::Guard nfdGuard;
    // Инициализация окна SFML
    sf::Vector2u windowSize{1280, 720};
    sf::RenderWindow window(sf::VideoMode(windowSize), "SlowGL Framework");
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setFramerateLimit(60);

    // Инициализация ImGui
    if (!ImGui::SFML::Init(window)) {
        printf("ImGui SFML initialization failed\n");
        return -1;
    }

    // Настройка шрифта
    ImGuiIO&io = ImGui::GetIO();
    SetOptimalFontSize(io, desktop);

    // Создание изображения для рендеринга
    int resolution = 512;
    sgl::SFMLImage image(sf::Vector2u(resolution, resolution), sf::Color::Black);

    // Создание DebugView для масштабирования и перемещения
    sgl::DebugView debugView(sf::Vector2f(windowSize), 0.05f);

    // Инициализация менеджера модулей
    sgl::ModuleManager moduleManager;

    // Регистрация модулей
    moduleManager.registerModule(std::make_unique<sgl::Lab1Module>());
    moduleManager.registerModule(std::make_unique<sgl::Lab2Module>());

    sf::Clock deltaClock;

    // Главный цикл приложения
    while (window.isOpen()) {
        // Обработка событий
        while (auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (auto* resized = event->getIf<sf::Event::Resized>()) {
                windowSize = resized->size;


                if (auto* activeModule = moduleManager.getActiveModule()) {
                    activeModule->resize(windowSize.x, windowSize.y);
                }
            }

            // Обработка событий для DebugView если мышь не захвачена ImGui
            if (!ImGui::GetIO().WantCaptureMouse && event.has_value()) {
                debugView.ProcessEvent(event.value());
            }

            // Передача события активному модулю
            if (auto* activeModule = moduleManager.getActiveModule()) {
                activeModule->handleEvent(*event);
            }
        }

        // Обновление ImGui
        sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        // Отрисовка UI
        ImGui::Begin("SlowGL Framework");
        ImGui::Text("FPS: %.1f", 1.0f / dt.asSeconds());

        // Управление видом
        ImGui::SeparatorText("Вид");
        if (ImGui::Button("Сброс")) debugView.ResetView();
        ImGui::SameLine();
        if (ImGui::Button("-")) debugView.Zoom(1.5f);
        ImGui::SameLine();
        if (ImGui::Button("+")) debugView.Zoom(0.5f);

        // Настройки разрешения
        ImGui::SeparatorText("Настройки");
        if (ImGui::DragInt("Разрешение", &resolution, 1.f, 16, 4096)) {
            if (resolution > 0 && resolution <= 4096) {
                image.resize(sf::Vector2u(resolution, resolution));

                // Обновление размера в активном модуле
                if (auto* activeModule = moduleManager.getActiveModule()) {
                    activeModule->resize(resolution, resolution);
                }
            }
        }

        // Отображение списка модулей
        ImGui::SeparatorText("Модули");
        if (ImGui::BeginListBox("Доступные модули")) {
            for (const auto&module: moduleManager.getModules()) {
                const bool isSelected = (moduleManager.getActiveModule() == module.get());
                if (ImGui::Selectable(module->getName().c_str(), isSelected)) {
                    moduleManager.setActiveModule(module->getName());
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }

                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", module->getDescription().c_str());
                }
            }
            ImGui::EndListBox();
        }

        ImGui::End();

        // Обновление активного модуля
        if (auto* activeModule = moduleManager.getActiveModule()) {
            activeModule->update(dt.asSeconds());
            activeModule->renderUI();
            activeModule->render(image);
        }

        // Отрисовка
        window.setView(debugView.getView());
        window.clear(sf::Color(40, 40, 40));
        window.draw(image);
        ImGui::SFML::Render(window);
        window.display();
    }

    NFD_Quit();
    ImGui::SFML::Shutdown();

    return 0;
}
