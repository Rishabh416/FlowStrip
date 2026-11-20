#pragma once

#include <cstdint>
#include <vector>

struct LEDPixel {
    uint8_t on = 0;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

using LEDFrame = std::vector<LEDPixel>;