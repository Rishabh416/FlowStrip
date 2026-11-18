#pragma once

#include <vector>

struct WaterCell {
    double depth = 0.0; // depth of the water
    double discharge = 0.0; // depth of water * average horizontal velocity
};

struct PhysicsState {
    std::vector<WaterCell> water;
    double time = 0.0;
};