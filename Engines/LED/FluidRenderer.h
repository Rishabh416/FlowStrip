#pragma once

#include <array>
#include <cstdint>

#include "LEDFrame.h"

struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class FluidRenderer {
    private:
        static constexpr std::array<RGB, 5> depthColors = {{
            {8, 43, 86},
            {8, 127, 163},
            {34, 230, 210},
            {66, 142, 255},
            {183, 122, 255}
        }};

        double minDepth;
        double maxDepth;

        double normalise(double depth);
        static RGB interpolate (RGB c0, RGB c1, double t);
        static RGB depthToColour(double t);
        double averageDepth(
            const std::vector<double>& depths,
            std::size_t ledIndex,
            std::size_t numLEDs
        );

    public:
        FluidRenderer(double minDepth, double maxDepth);
        LEDFrame render(
            const std::vector<double>& depths,
            std::size_t numLEDs
        );
};
