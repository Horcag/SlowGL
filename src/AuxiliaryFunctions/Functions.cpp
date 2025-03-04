#include "Functions.h"


sf::Vector3f barycentric_coord(const std::array<sf::Vector2f, 3> &triangle_coord, const sf::Vector2u &point_coord) {
    float x0 = triangle_coord[0].x;
    float y0 = triangle_coord[0].y;
    float x1 = triangle_coord[1].x;
    float y1 = triangle_coord[1].y;
    float x2 = triangle_coord[2].x;
    float y2 = triangle_coord[2].y;
    float x = static_cast<float>(point_coord.x);
    float y = static_cast<float>(point_coord.y);
    float denominator = (x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2);
    float lambda0 = ((x - x2) * (y1 - y2) - (x1 - x2) * (y - y2)) / denominator;
    float lambda1 = ((x0 - x2) * (y - y2) - (x - x2) * (y0 - y2)) / denominator;
    float lambda2 = 1.0f - lambda0 - lambda1;
    return sf::Vector3f(lambda0, lambda1, lambda2);
}

bool is_inside(const sf::Vector3f& baryc_coord) {
    return baryc_coord.x < 0 && baryc_coord.y < 0 && baryc_coord.z < 0;
}

sf::Vector3f cross_product(const sf::Vector3f& v1, const sf::Vector3f& v2) {
    return {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
        };
}

float clip_edge(const sf::Vector3f& triangle_normal) {
    return (triangle_normal.x * light_direction.x + triangle_normal.y * light_direction.y + triangle_normal.z * light_direction.z) / (triangle_normal.x * triangle_normal.x + triangle_normal.y * triangle_normal.y + triangle_normal.z * triangle_normal.z);
}

