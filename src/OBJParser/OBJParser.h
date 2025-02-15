#pragma once
#include "string"
#include "vector"
#ifndef PARSER_H
#define PARSER_H

class OBJParser
{
public:
   // explicit OBJParser(const std::string& fileName);

    void setBuff(const std::vector<char>& buff);
    std::vector<char> getBuff() const;
    std::vector<std::tuple<double,double,double>> parseV(const std::string& fileName) const;
    OBJParser();
private:
    std::vector<char> buffer;
    std::vector<char> bufferV;
    std::vector<char> bufferVT;
    std::vector<char> bufferVN;
    std::vector<char> bufferF;
};

#endif //PARSER_H
