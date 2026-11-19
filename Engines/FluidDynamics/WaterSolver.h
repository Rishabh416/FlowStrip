#pragma once

#include <vector>
#include "PhysicsState.h"
#include "PhysicsConfig.h"
#include "NumericalFlux.h"

class WaterSolver {
    public:
    double calculateVelocity(const WaterCell& cell) const;
    double calculateTimeStep(
        const PhysicsState& state,
        double dx,
        const PhysicsConfig& config
    );
    std::vector<WaterFlux> interiorFlux(
        const PhysicsState& state,
        const PhysicsConfig& config
    );
    PhysicsState advance(
        const PhysicsState& state,
        double dx,
        const PhysicsConfig& config
    );
};