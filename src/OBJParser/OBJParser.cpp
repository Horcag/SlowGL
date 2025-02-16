#include "OBJParser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Buffer.h"

std::vector<sf::Vector3f> OBJParser::parseV(const std::string &fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << fileName << std::endl;
        return {};
    }
    std::vector<sf::Vector3f> result;
    std::string line;
    double x, y, z;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref >> x >> y >> z;
        if (pref == "v") {
            result.emplace_back(x, y, z);
        }
        if (pref == "vt") {
            return result;
        }
    }
    return result;
}

std::vector<sf::Vector3f> OBJParser::parseV(std::ifstream &file) {
    std::vector<sf::Vector3f> result;
    std::string line;
    double x, y, z;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref >> x >> y >> z;
        if (pref == "v") {
            result.emplace_back(x, y, z);
        } else if (pref == "vt") {
            return result;
        }
    }
    return result;
}

std::vector<sf::Vector2f> OBJParser::parseVt(std::ifstream &file) {
    std::vector<sf::Vector2f> result;
    std::string line;
    double x, y;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref >> x >> y;
        if (pref == "vt") {
            result.emplace_back(x, y);
        } else if (pref != "vt") {
            continue;
        } else if (pref == "f") {
            return result;
        }
    }
    return result;
}

std::vector<Face> OBJParser::parseF(std::ifstream &file) {
    std::vector<Face> result;
    Face face{};
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref;
        if (pref == "f") {
            for (int i = 0; i < 3; ++i) {
                std::string faceData;
                lineStream >> faceData;
                std::replace(faceData.begin(), faceData.end(), '/', ' ');
                std::istringstream faceDataStream(faceData);
                faceDataStream >> face.vertexIndices[i] >> face.textureIndices[i] >> face.normalIndices[i];
            }
            result.push_back(face);
        }
    }
    return result;
}
