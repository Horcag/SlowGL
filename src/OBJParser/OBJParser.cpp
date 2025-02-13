#include "OBJParser.h"
#include <iostream>
#include <vector>

OBJParser::OBJParser(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << fileName << std::endl;
        return;
    }
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        std::cout << "Файл успешно прочитан в массив char." << std::endl;
    } else {
        std::cerr << "Ошибка при чтении файла!" << std::endl;
    }
    file.close();
}
