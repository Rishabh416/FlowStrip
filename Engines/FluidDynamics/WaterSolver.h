#pragma once

#include "PhysicsState.h"

class WaterSolver {
    public:
    double calculateVelocity(const WaterCell& cell) const;
};