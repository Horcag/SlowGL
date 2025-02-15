#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics.hpp>

#include "Image/SFMLImage.h"
#include "Debug/DebugView.h"

void draw(sgl::SFMLImage& image){
    for(unsigned int i = 0; i < image.getSize().x; i++) {
        for(unsigned int j = 0; j < image.getSize().y; j++) {
            if((i+1) % (j+1) == 0) image.setPixel({i,j}, sf::Color::Blue);
        }
    }
    image.update();
}

int main()
{
    // Надо будет все тут менять
    
    sf::Vector2u windowSize{1280, 640};
    sf::RenderWindow window(sf::VideoMode(windowSize), "Lab 1");
    window.setFramerateLimit(60);
    if (!ImGui::SFML::Init(window)){
        printf("Imgui SFML startup failed\n");
        return -1;
    }

    int resolution = 16;
    sgl::SFMLImage image(sf::Vector2u(resolution, resolution), sf::Color::Black);
    sgl::DebugView debugView(sf::Vector2f(windowSize), 0.05f);

    sf::Clock deltaClock;
    sf::Clock perfClock;
    sf::Time perfTime;
    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                windowSize = resized->size;
            }

            if(!ImGui::GetIO().WantCaptureMouse && event.has_value()) debugView.ProcessEvent(event.value());
        }

        const sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        ImGui::ShowDemoWindow();

        ImGui::Begin("Lab 1!");
        ImGui::Text("FPS: %.1f", 1.0f/dt.asSeconds());
        ImGui::Text("Render time: %lluus", perfTime.asMicroseconds());
        
        ImGui::SeparatorText("View");
        if(ImGui::Button("Reset view")) debugView.ResetView();
        ImGui::SeparatorText("Render");
        if(ImGui::SliderInt("Resoultion", &resolution, 16, 512)) {
            image.resize(sf::Vector2u(resolution, resolution));
        }
        ImGui::End();

        perfClock.restart();
        draw(image);
        perfTime = perfClock.getElapsedTime();

        window.setView(debugView.getView());

        window.clear();
        window.draw(image);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}