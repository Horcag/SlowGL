#pragma once
#include <string>
#include <nfd.hpp>
#include "../Image/SFMLImage.h"

namespace sgl::utils {
    // Открыть диалог выбора файла .obj
    bool openOBJFileDialog(char* filename, size_t maxLen);

    // Открыть диалог сохранения изображения
    bool saveImageDialog(char* filename, size_t maxLen, const char* defaultName = "image.png");

    // Сохранить изображение в файл
    bool saveImage(const sgl::SFMLImage&image, const std::string&filename);
}
