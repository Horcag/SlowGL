#include "OBJParser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


/*OBJParser::OBJParser(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << fileName << std::endl;
        return;
    }
    file.seekg(0, std::ios::end);
    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        std::cout << "Файл успешно прочитан в массив char." << std::endl;
        setBuff(buffer);
    } else {
        std::cerr << "Ошибка при чтении файла!" << std::endl;
    }
    file.close();
}*/

OBJParser::OBJParser() = default;

void OBJParser::setBuff(const std::vector<char>& buff) {
    buffer = buff;
}

std::vector<char> OBJParser::getBuff() const {
    return buffer;
}

std::vector<std::tuple<double, double, double> > OBJParser::parseV(const std::string& fileName) const{
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << fileName << std::endl;
        return {};
    }
    std::vector<std::tuple<double, double, double> > result;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        double x, y, z;
        std::string pref;
        lineStream >> pref >> x >> y >> z;
        if (pref == "v") {
           result.emplace_back(x, y, z);
        }
        if (pref == "vt") {
            return result;
        }
    }
}



