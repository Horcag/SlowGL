#pragma once

#include "Core/IModule.h"
#include "Parsers/OBJParser/Model3D.h"
#include "Parsers/OBJParser/OBJParser.h"
#include "Image/SFMLImage.h"
#include <string>


namespace sgl {
    class Lab2Module : public IModule {
    public:
        Lab2Module();

        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] std::string getDescription() const override;

        void init() override;

        void update(float deltaTime) override;

        void render(sgl::SFMLImage&image) override;

        void renderUI() override;

        void handleEvent(const sf::Event&event) override;

        void resize(unsigned int width, unsigned int height) override;

    private:
        enum class PreviewMode {
            SingleTriangle = 0,
            MultipleTriangles,
            ModelPolygons,
            ModelPolygonsWithLight,
            ModelPolygonsWithZBuffer
        };

        struct Triangle {
            sf::Vector2f v0, v1, v2;
            sf::Color color;
        };

        bool selectModel();

        void calculateModelScale(const Model3D&model, sf::Vector3f&center, float&factor, int resolution);

        void drawTriangle(sgl::SFMLImage&image, int index = -1);

        void drawModelPolygons(sgl::SFMLImage&image, bool useLight = false, bool useZBuffer = false);

        void generateTriangles(int count);

        sf::Color getRandomColor();

        sf::Vector3f calculateNormal(const sf::Vector3f&v0, const sf::Vector3f&v1, const sf::Vector3f&v2);

        float calculateLightCosine(const sf::Vector3f&normal, const sf::Vector3f&light);

        PreviewMode m_currentPreview = PreviewMode::SingleTriangle;
        OBJParser m_parser;
        Model3D m_currentModel;
        sf::Vector3f m_modelCenter;
        float m_modelScale = 1.0f;
        char m_filename[256] = "";

        std::vector<Triangle> m_triangles;
        int m_activeTriangleIndex = 0;
        int m_triangleCount = 3;

        bool m_animateTriangles = false;
        float m_animationTime = 0.0f;

        sf::Vector3f m_lightDirection = {0.0f, 0.0f, 1.0f};

        SFMLImage* m_imageRef = nullptr;
    };
}
