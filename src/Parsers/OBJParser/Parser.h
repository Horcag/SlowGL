#pragma once
#include "Model3D.h"
#ifndef PARSER_H
#define PARSER_H

class Parser {
public:
    virtual ~Parser() = default;

    virtual Model3D parse(std::ifstream&file) const = 0;
};

#endif //PARSER_H
