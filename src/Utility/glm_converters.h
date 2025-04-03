#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int3.hpp>
#include <SFML/System/Vector3.hpp>

static glm::vec3 glm_vec(const sf::Vector3f& vec){
    return glm::vec3(vec.x, vec.y, vec.z);
}

static glm::vec2 glm_vec(const sf::Vector2f& vec){
    return glm::vec2(vec.x, vec.y);
}

static glm::ivec3 glm_ivec(const sf::Vector3i& vec){
    return glm::ivec3(vec.x, vec.y, vec.z);
}
