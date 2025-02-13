#include "gtest/gtest.h"
#include "../src/OBJParser/OBJParser.h"

TEST(OBJParser_test,Constructor_test) {
    auto parser = new OBJParser("model_1.bin");
}