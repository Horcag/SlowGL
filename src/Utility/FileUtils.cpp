#include "FileUtils.h"
#include <filesystem>
#include <cstring>

namespace sgl::utils {
    bool openOBJFileDialog(char* filename, size_t maxLen) {
        NFD::UniquePath outPath;
        constexpr nfdfilteritem_t filterItem[1] = {{"3D модель", "obj"}};

        nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);
        if (result == NFD_OKAY) {
            strncpy(filename, outPath.get(), maxLen - 1);
            filename[maxLen - 1] = '\0'; // Гарантируем нулевой терминатор
            return true;
        }
        return false;
    }

    bool saveImageDialog(char* filename, size_t maxLen, const char* defaultName) {
        NFD::UniquePath outPath;
        constexpr nfdfilteritem_t filterItem[1] = {{"Изображение", "png"}};

        nfdresult_t result = NFD::SaveDialog(outPath, filterItem, 1, nullptr, defaultName);
        if (result == NFD_OKAY) {
            strncpy(filename, outPath.get(), maxLen - 1);
            filename[maxLen - 1] = '\0';
            return true;
        }
        return false;
    }

    bool saveImage(const sgl::SFMLImage&image, const std::string&filename) {
        std::string finalFilename = filename;
        if (!std::filesystem::path(filename).has_extension()) {
            finalFilename += ".png";
        }
        return image.saveToFile(finalFilename);
    }
}
