#include "OBJParser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Buffer.h"
std::vector<sf::Vector3f> OBJParser::get_bufferV() const{
    return buffer.getBufferV();
}

void OBJParser::set_BufferV(const  std::vector<sf::Vector3f>& buffer) {
    this->buffer.setBufferV(buffer);
}

std::vector<sf::Vector3f> OBJParser::parseV(const std::string& fileName){
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << fileName << std::endl;
        return {};
    }
    std::vector<sf::Vector3f> result;
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
    set_BufferV(result);
    return result;
}

std::vector<sf::Vector3f> OBJParser::parseV(std::ifstream& file)
{
    std::vector<sf::Vector3f> result;
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
    set_BufferV(result);
    return result;
}
