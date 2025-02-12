#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics.hpp>

#include "Image/SFMLImage.h"

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
    image.setCenter(sf::Vector2f(windowSize / 2u));
    image.setPixelSize({512,512});

    sf::Clock deltaClock;
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
                image.setCenter(sf::Vector2f(windowSize / 2u));
            }
        }

        const sf::Time dt = deltaClock.restart();
        ImGui::SFML::Update(window, dt);

        ImGui::ShowDemoWindow();

        ImGui::Begin("Lab 1!");
        ImGui::Text("FPS: %.1f", 1.0f/dt.asSeconds());
        if(ImGui::SliderInt("Resoultion", &resolution, 16, 512)) {
            image.resize(sf::Vector2u(resolution, resolution));
        }
        ImGui::Button("Look at this pretty button");
        ImGui::End();

        draw(image);

        window.clear();
        window.draw(image);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}