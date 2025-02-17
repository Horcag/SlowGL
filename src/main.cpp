#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/Graphics.hpp>

#include "Image/SFMLImage.h"
#include "Debug/DebugView.h"

void draw(sgl::SFMLImage& image, int64_t time){
    image.clear();
    for(unsigned int i = 0; i < image.getSize().x; i++) {
        for(unsigned int j = 0; j < image.getSize().y; j++) {
            if((i+time) % (j+1) == 0) image.setPixel({i,j}, sf::Color::Blue);
        }
    }
    image.update();
}

int line_method_combo(){
    static const char* methods[] = { "dotted_line", "dotted_line_v2", "x_loop_line", "x_loop_line_hotfix_1", "x_loop_line_hotfix_2", "x_loop_line_v2", "x_loop_line_v2_no_y_calc", "x_loop_line_v2_no_y_calc_for_some_unknown_reason", "bresenham line" };
    static int current_method = 0;

    ImGui::Combo("Line method", &current_method, methods, IM_ARRAYSIZE(methods));

    return current_method;
}

const char* previews[] = { "1. Image manipulation", "2. Straight lines", "3. 3D Model", "4. 3D Model vertices", "5. 3D Model (Polygons)", "6. 3D Model edges" };

int main()
{
    //Ладно наврал не будем менять

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

    sf::Clock mainClock;
    mainClock.start();
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
        if(ImGui::DragInt("Resoultion", &resolution, 1.f, 16, 512)) {
            if(resolution > 0 && resolution < 2048)
                image.resize(sf::Vector2u(resolution, resolution));
        }

        ImGui::SeparatorText("Lab 1");

        static int current_preview = 0;
        ImGui::Combo("Preview", &current_preview, previews, IM_ARRAYSIZE(previews));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        switch (current_preview)
        {
        case 0: //1. Работа с изображениями.
        {
            static int rgb[3] = { 0, 0, 0 };

            for (int i = 0; i < 3; i++)
            {
                //if (i > 0) ImGui::SameLine();
                ImGui::PushID(i);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(i / 3.0f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(i / 3.0f, 0.6f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(i / 3.0f, 0.7f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(i / 3.0f, 0.9f, 0.9f));
                ImGui::SliderInt("R\0G\0B\0" + i*2, &rgb[i], 0, 255);
                ImGui::PopStyleColor(4);
                ImGui::PopID();
            }

            image.clear(sf::Color(rgb[0], rgb[1], rgb[2]));
            image.update();
        }
        break;
        case 1: //2. Отрисовка прямых линий 
        {
            line_method_combo();
        }
        break;
        case 2: //3. Работа с трёхмерной моделью (вершины) 

        break;

        case 3: //4. Отрисовка вершин трёхмерной модели 

        break;
        case 4: //5. Работа с трёхмерной моделью (полигоны)

        break;
        case 5: //6. Отрисовка рёбер трёхмерной модели

        break;
        default:
            break;
        }

        ImGui::Spacing();

        ImGui::SeparatorText("File");

        static char filename[255]{};
        ImGui::InputText("Filename", filename, 255, ImGuiInputTextFlags_CharsNoBlank);
        ImGui::SameLine();
        if(ImGui::Button("Save")){
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

    ImGui::SFML::Shutdown();
    return 0;
}