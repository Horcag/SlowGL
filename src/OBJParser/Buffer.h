#pragma once
#include <vector>
#include <SFML/System/Vector3.hpp>
#ifndef BUFFER_H
#define BUFFER_H



class Buffer {
private:
    std::vector<sf::Vector3f> bufferV;
    std::vector<sf::Vector3f> bufferVT;
    std::vector<sf::Vector3f> bufferVN;
    std::vector<sf::Vector3f> bufferF;

public:
    // Геттеры
    std::vector<sf::Vector3f> getBufferV() const;

    std::vector<sf::Vector3f> getBufferVT() const;

    std::vector<sf::Vector3f> getBufferVN() const;

    std::vector<sf::Vector3f> getBufferF() const;

    // Сеттеры
    void setBufferV(const std::vector<sf::Vector3f>& newBufferV);
    void setBufferVT(const std::vector<sf::Vector3f>& newBufferVT);
    void setBufferVN(const std::vector<sf::Vector3f>& newBufferVN);
    void setBufferF(const std::vector<sf::Vector3f>& newBufferF);
};




#endif //BUFFER_H
