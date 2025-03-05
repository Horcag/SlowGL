#pragma once
#include <glm/ext/vector_float3.hpp>
#include <SFML/System/Vector3.hpp>

glm::vec3 glm_vec(const sf::Vector3f& vec){
    return glm::vec3(vec.x, vec.y, vec.z);
}