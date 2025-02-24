#include "OBJParser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Model3D.h"

std::vector<sf::Vector3f> OBJParser::parse_vertex(std::ifstream&file) {
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
        else if (pref == "vt") {
            file.seekg(0, std::ios::beg);
            return result;
        }
    }
    file.seekg(0, std::ios::beg);
    return result;
}

std::vector<sf::Vector2f> OBJParser::parse_vertex_texture(std::ifstream&file) {
    std::vector<sf::Vector2f> result;
    std::string line;
    double x, y;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref >> x >> y;
        if (pref == "vt") {
            result.emplace_back(x, y);
        }
        else if (pref == "vn") {
            file.seekg(0, std::ios::beg);
            return result;
        }
    }
    file.seekg(0, std::ios::beg);
    return result;
}

std::vector<sf::Vector3f> OBJParser::parse_vertex_normal(std::ifstream&file) {
    std::vector<sf::Vector3f> result;
    std::string line;
    double x, y, z;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref >> x >> y >> z;
        if (pref == "vn") {
            result.emplace_back(x, y, z);
        }
        else if (pref == "f") {
            file.seekg(0, std::ios::beg);
            return result;
        }
    }
    file.seekg(0, std::ios::beg);
    return result;
}

std::vector<Face> OBJParser::parse_faces(std::ifstream&file) {
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
    file.seekg(0, std::ios::beg);
    return result;
}

Model3D OBJParser::parse(std::ifstream&file) const {
    Model3D model;
    std::vector<sf::Vector3f> vertex;
    std::vector<sf::Vector2f> vertex_texture;
    std::vector<sf::Vector3f> vertex_normal;
    std::vector<Face> faces;
    Face face{};
    std::string line;
    double x, y, z;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref;
        if (pref == "v") {
            lineStream >> x >> y >> z;
            vertex.emplace_back(x, y, z);
        }
        else if (pref == "vt") {
            lineStream >> x >> y;
            vertex_texture.emplace_back(x, y);
        }
        else if (pref == "vn") {
            lineStream >> x >> y >> z;
            vertex_normal.emplace_back(x, y, z);
        }
        else if (pref == "f") {
            for (int i = 0; i < 3; ++i) {
                std::string faceData;
                lineStream >> faceData;
                std::replace(faceData.begin(), faceData.end(), '/', ' ');
                std::istringstream faceDataStream(faceData);
                faceDataStream >> face.vertexIndices[i] >> face.textureIndices[i] >> face.normalIndices[i];
            }
            faces.push_back(face);
        }
    }
    model.set_vertex(vertex);
    model.set_vertex_texture(vertex_texture);
    model.set_vertex_normal(vertex_normal);
    model.set_faces(faces);
    return model;
}
