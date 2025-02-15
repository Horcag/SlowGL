#pragma once
#include "string"
#include "vector"
#ifndef PARSER_H
#define PARSER_H

class OBJParser
{
public:
   // explicit OBJParser(const std::string& fileName);
    std::vector<std::tuple<double, double, double>> get_bufferV();
    void set_BufferV(const std::vector<std::tuple<double, double, double>>& buffer);
    std::vector<std::tuple<double,double,double>> parseV(const std::string& fileName);
private:
    std::vector<std::tuple<double, double, double>> bufferV;
    std::vector<std::tuple<double, double, double>> bufferVT;
    std::vector<std::tuple<double, double, double>> bufferVN;
    std::vector<std::tuple<double, double, double>> bufferF;
};

#endif //PARSER_H
