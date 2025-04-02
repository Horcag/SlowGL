#pragma once

#include "Core/IModule.h"
#include "Parsers/OBJParser/Model3D.h"
#include "Parsers/OBJParser/OBJParser.h"
#include "Image/SFMLImage.h"
#include <string>

namespace sgl {
    /**
    * @class Lab1Module
    * @brief Модуль для демонстрации работы с изображениями и 3D-моделями
    *
    * Данный модуль реализует основные функции для работы с изображениями
    * и 3D-моделями формата OBJ, включая отрисовку линий различными алгоритмами,
    * визуализацию вершин, полигонов и рёбер 3D-моделей.
    */
    class Lab1Module : public IModule {
    private:
        /**
         * @brief Режимы предварительного просмотра
         */
        enum class PreviewMode {
            ImageManipulation = 0, ///< Работа с изображениями
            StraightLines, ///< Прямые линии
            Model3D, ///< 3D-модель
            ModelVertices, ///< Вершины 3D-модели
            ModelPolygons, ///< Полигоны 3D-модели
            ModelEdges ///< Рёбра 3D-модели
        };

        PreviewMode m_currentPreview = PreviewMode::ImageManipulation; ///< Текущий режим просмотра
        int m_rgb[3] = {100, 100, 100}; ///< RGB значения для цвета фона

        /**
         * @brief Настройки линий
         */
        struct {
            int current_method = 0; ///< Текущий метод отрисовки
            int dotted_line_dots = 128; ///< Количество точек для пунктирной линии
        } m_lineOptions;

        OBJParser m_parser; ///< Парсер для OBJ файлов
        Model3D m_currentModel; ///< Текущая 3D-модель
        sf::Vector3f m_modelCenter; ///< Центр модели
        float m_modelScale = 1.0f; ///< Масштаб модели
        char m_filename[256] = {0}; ///< Имя файла
        SFMLImage* m_imageRef = nullptr; ///< Указатель на изображение

    public:
        Lab1Module();

        ~Lab1Module() override = default;

        std::string getName() const override;

        std::string getDescription() const override;

        void init() override;

        void update(float deltaTime) override;

        void render(sgl::SFMLImage&image) override;

        void renderUI() override;

        void handleEvent(const sf::Event&event) override;

        void resize(unsigned int width, unsigned int height) override;

    private:
        /**
                 * @brief Вычислить масштаб модели
                 * @param model Модель для масштабирования
                 * @param center Центр модели
                 * @param factor Коэффициент масштаба
                 * @param resolution Разрешение экрана
                 */
        static void calculateModelScale(const Model3D&model, sf::Vector3f&center, float&factor, int resolution);

        /**
         * @brief Отрисовать линии
         * @param image Изображение для отрисовки
         */
        void drawLines(SFMLImage&image);

        /**
         * @brief Отрисовать вершины модели
         * @param image Изображение для отрисовки
         */
        void drawModelVertices(SFMLImage&image);

        /**
         * @brief Отрисовать полигоны модели
         * @param image Изображение для отрисовки
         */
        void drawModelPolygons(SFMLImage&image);

        /**
         * @brief Выбор 3D-модели через диалог
         * @return true если модель была выбрана успешно
         */
        bool selectModel();
    };
}
