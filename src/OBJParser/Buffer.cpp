#include "Buffer.h"

std::vector<sf::Vector3f> Buffer::getBufferV() const {
    return bufferV;
}

std::vector<sf::Vector3f> Buffer::getBufferVT() const {
    return bufferVT;
}

std::vector<sf::Vector3f> Buffer::getBufferVN() const {
    return bufferVN;
}

std::vector<sf::Vector3f> Buffer::getBufferF() const {
    return bufferF;
}

void Buffer::setBufferV(const std::vector<sf::Vector3f>& newBufferV) {
    bufferV = newBufferV;
}

void Buffer::setBufferVT(const std::vector<sf::Vector3f>& newBufferVT) {
    bufferVT = newBufferVT;
}

void Buffer::setBufferVN(const std::vector<sf::Vector3f>& newBufferVN) {
    bufferVN = newBufferVN;
}

void Buffer::setBufferF(const std::vector<sf::Vector3f>& newBufferF) {
    bufferF = newBufferF;
}