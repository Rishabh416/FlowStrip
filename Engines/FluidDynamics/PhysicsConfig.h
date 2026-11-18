#pragma once

struct PhysicsConfig {
    double gravity = 9.81; // gravitational acceleration (m/s^2)
    double cfl = 0.4; // safety factor for numerical stability
    double maxTimeStep = 0.01; // maximum allowed time-step (seconds)
    double dryTolerance = 1e-6; // minimum depth considered wet (meters)
};