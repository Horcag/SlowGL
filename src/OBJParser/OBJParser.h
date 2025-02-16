#pragma once
#include "string"
#include "vector"
#ifndef PARSER_H
#define PARSER_H
#include "Buffer.h"
class OBJParser {
public:
    std::vector<sf::Vector3f> get_bufferV() const;

    void set_BufferV(const std::vector<sf::Vector3f> &buffer);

    std::vector<sf::Vector3f> parseV(const std::string &fileName);
    std::vector<sf::Vector3f> parseV(std::ifstream& file);

private:
    Buffer buffer;
};
#endif //PARSER_H
