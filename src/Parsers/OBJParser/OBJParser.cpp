#include "OBJParser.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "Model3D.h"

sf::Vector3f calculateNormal(const sf::Vector3f &v1, const sf::Vector3f &v2, const sf::Vector3f &v3) {
    sf::Vector3f edge1 = v2 - v1;
    sf::Vector3f edge2 = v3 - v1;

    sf::Vector3f normal(
        edge1.y * edge2.z - edge1.z * edge2.y,
        edge1.z * edge2.x - edge1.x * edge2.z,
        edge1.x * edge2.y - edge1.y * edge2.x
    );

    float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    normal.x /= length;
    normal.y /= length;
    normal.z /= length;
    return normal;
}

std::vector<sf::Vector3f> OBJParser::parse_vertex(std::ifstream &file) {
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
            file.seekg(0, std::ios::beg);
            return result;
        }
    }
    file.seekg(0, std::ios::beg);
    return result;
}

std::vector<sf::Vector2f> OBJParser::parse_vertex_texture(std::ifstream &file) {
    std::vector<sf::Vector2f> result;
    std::string line;
    double x, y;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref >> x >> y;
        if (pref == "vt") {
            result.emplace_back(x, y);
        } else if (pref == "vn") {
            file.seekg(0, std::ios::beg);
            return result;
        }
    }
    file.seekg(0, std::ios::beg);
    return result;
}

std::vector<sf::Vector3f> OBJParser::parse_vertex_normal(std::ifstream &file) {
    std::vector<sf::Vector3f> result;
    std::string line;
    double x, y, z;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref >> x >> y >> z;
        if (pref == "vn") {
            result.emplace_back(x, y, z);
        } else if (pref == "f") {
            file.seekg(0, std::ios::beg);
            return result;
        }
    }
    file.seekg(0, std::ios::beg);
    return result;
}

std::vector<Face> OBJParser::parse_faces(std::ifstream &file) {
    std::vector<Face> result;
    Face face{};
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string pref;
        lineStream >> pref;
        if (pref == "f") {
            std::array<int *, 3> vertexIndices = {&face.vertexIndices.x, &face.vertexIndices.y, &face.vertexIndices.z};
            std::array<int *, 3> textureIndices = {
                &face.textureIndices.x, &face.textureIndices.y, &face.textureIndices.z
            };
            std::array<int *, 3> normalIndices = {&face.normalIndices.x, &face.normalIndices.y, &face.normalIndices.z};

            for (int i = 0; i < 3; ++i) {
                std::string faceData;
                lineStream >> faceData;
                std::replace(faceData.begin(), faceData.end(), '/', ' ');
                std::istringstream faceDataStream(faceData);

                faceDataStream >> *vertexIndices[i] >> *textureIndices[i] >> *normalIndices[i];

                (*vertexIndices[i])--;
                (*textureIndices[i])--;
                (*normalIndices[i])--;
            }


            result.push_back(face);
        }
    }

    file
            .
            seekg(
                0
                ,
                std::ios::beg
            );
    return
            result;
}

Model3D OBJParser::parse(std::ifstream &file) const {
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
        } else if (pref == "vt") {
            lineStream >> x >> y;
            vertex_texture.emplace_back(x, y);
        } else if (pref == "vn") {
            lineStream >> x >> y >> z;
            vertex_normal.emplace_back(x, y, z);
        } else if (pref == "f" && !vertex_normal.empty()) {
            for (int i = 0; i < 3; ++i) {
                std::string faceData;
                lineStream >> faceData;
                std::replace(faceData.begin(), faceData.end(), '/', ' ');
                std::istringstream faceDataStream(faceData);
                faceDataStream >> (&face.vertexIndices.x)[i] >> (&face.textureIndices.x)[i] >> (&face.normalIndices.x)[
                    i]; //это безопасно честно)
                (&face.vertexIndices.x)[i]--;
                (&face.textureIndices.x)[i]--;
                (&face.normalIndices.x)[i]--;
            }
            faces.push_back(face);
        } else if (pref == "f") {
            for (int i = 0; i < 2; ++i) {
                std::string faceData;
                lineStream >> faceData;
                std::replace(faceData.begin(), faceData.end(), '/', ' ');
                std::istringstream faceDataStream(faceData);
                faceDataStream >> (&face.vertexIndices.x)[i] >> (&face.textureIndices.x)[i]; //это безопасно честно)
                (&face.vertexIndices.x)[i]--;
                (&face.textureIndices.x)[i]--;
            }
            faces.push_back(face);
            vertex_normal.resize(vertex.size(), sf::Vector3f(0, 0, 0));
            for (auto &face: faces) {
                sf::Vector3f v1 = vertex[face.vertexIndices.x];
                sf::Vector3f v2 = vertex[face.vertexIndices.y];
                sf::Vector3f v3 = vertex[face.vertexIndices.z];
                sf::Vector3f normal = calculateNormal(v1, v2, v3);
                vertex_normal[face.vertexIndices.x] += normal;
                vertex_normal[face.vertexIndices.y] += normal;
                vertex_normal[face.vertexIndices.z] += normal;
            }
            for (auto &normal: vertex_normal) {
                float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
                normal.x /= length;
                normal.y /= length;
                normal.z /= length;
            }
        }
        model.set_vertex(vertex);
        model.set_vertex_texture(vertex_texture);
        model.set_vertex_normal(vertex_normal);
        model.set_faces(faces);
        return model;
    }
}
