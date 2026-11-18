#pragma once

#include "PhysicsState.h"

struct WaterFlux {
    double mass;
    double momentum;
};

WaterFlux physicalFlux(
    const WaterCell& cell,
    double gravity
);

WaterFlux hllFlux(
    const WaterCell& left,
    const WaterCell& right,
    double gravity
);