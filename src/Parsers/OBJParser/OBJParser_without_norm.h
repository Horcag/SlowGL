#pragma once
#include "string"
#include "Parser.h"
#include "SFML/System/Vector2.hpp"
#include "Model3D.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>

class OBJParser_without_norm final : public Parser {
public:
    Model3D parse(std::ifstream&file) const override;
};
