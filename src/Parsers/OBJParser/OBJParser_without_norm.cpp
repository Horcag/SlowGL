#include "OBJParser_without_norm.h"

sf::Vector3f calculateNormal(const sf::Vector3f& v1, const sf::Vector3f& v2, const sf::Vector3f& v3) {
    sf::Vector3f edge1 = v2 - v1;
    sf::Vector3f edge2 = v3 - v1;

    sf::Vector3f normal(
        edge1.y * edge2.z - edge1.z * edge2.y,
        edge1.z * edge2.x - edge1.x * edge2.z,
        edge1.x * edge2.y - edge1.y * edge2.x
    );

    float length = std::sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
     normal.x /= length;
     normal.y /= length;
     normal.z /= length;
    return normal;
    }
Model3D OBJParser_without_norm::parse(std::ifstream& file) const {
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
        else if (pref == "f") {
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
        }
        vertex_normal.resize(vertex.size(), sf::Vector3f(0, 0, 0));
        for (auto& face : faces) {
            sf::Vector3f v1 = vertex[face.vertexIndices.x];
            sf::Vector3f v2 = vertex[face.vertexIndices.y];
            sf::Vector3f v3 = vertex[face.vertexIndices.z];
            sf::Vector3f normal = calculateNormal(v1, v2, v3);
            vertex_normal[face.vertexIndices.x] += normal;
            vertex_normal[face.vertexIndices.y] += normal;
            vertex_normal[face.vertexIndices.z] += normal;
    }
   for (auto& normal : vertex_normal) {
         float length = std::sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
           if (length > 0) {
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