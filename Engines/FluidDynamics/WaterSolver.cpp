#include "WaterSolver.h"
#include "NumericalFlux.h"
#include "BoundaryConditions.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

double WaterSolver::calculateVelocity(
    const WaterCell& cell
) const {
    if (cell.depth <= 0.0) {
        return 0.0;
    }

    return cell.discharge / cell.depth;
}

double WaterSolver::calculateTimeStep(
    const PhysicsState& state,
    double dx,
    const PhysicsConfig& config
) {
    double maxSpeed = 0.0;

    if (dx <= 0.0) {
        throw std::invalid_argument("Nonpositive cell width!");
    }

    for (const auto& cell : state.water) {
        if (cell.depth < 0.0) {
        throw std::invalid_argument("Negative cell depth!");
    }
        double velocity = calculateVelocity(cell);
        double waveSpeed = std::sqrt(config.gravity * cell.depth);
        double currentSpeed = std::abs(velocity) + waveSpeed;
        if (currentSpeed > maxSpeed) {
            maxSpeed = currentSpeed;
        }
    }
    if (maxSpeed <= 0.0) {
        return config.maxTimeStep;
    }
    return std::min(config.cfl * dx / maxSpeed, config.maxTimeStep);
}

std::vector<WaterFlux> WaterSolver::interiorFlux(
    const PhysicsState& state,
    const PhysicsConfig& config
) {
    std::vector<WaterFlux> fluxes(state.water.size() + 1);

    if (state.water.empty()) {
        throw std::invalid_argument("Water state empty!");
    }

    for (const auto& cell : state.water) {
        if (cell.depth < 0.0) {
            throw std::invalid_argument("Negative cell depth!");
        }
    }

    for (int i = 1; i < state.water.size(); ++i) {
        fluxes[i] = hllFlux(state.water[i - 1], state.water[i], config.gravity);
    }

    WaterCell leftGhostCell = reflectiveGhost(state.water[0]);
    WaterFlux leftBoundaryFlux = hllFlux(leftGhostCell, state.water[0], config.gravity);

    WaterCell rightGhostCell = reflectiveGhost(state.water.back());
    WaterFlux rightBoundaryFlux = hllFlux(state.water.back(), rightGhostCell, config.gravity);

    fluxes[0] = leftBoundaryFlux;
    fluxes.back() = rightBoundaryFlux;
    return fluxes;
}

PhysicsState WaterSolver::advance(
    const PhysicsState& state,
    double dx,
    const PhysicsConfig& config
) {
    std::vector<WaterFlux> fluxes = interiorFlux(state, config);
    std::vector<WaterCell> newWater(state.water.size());

    double timeStep = calculateTimeStep(state, dx, config);

    for (int i = 0; i < state.water.size(); ++i) {
        newWater[i].depth = state.water[i].depth 
        - (timeStep / dx) * (fluxes[i + 1].mass - fluxes[i].mass);
        newWater[i].discharge = state.water[i].discharge 
        - (timeStep / dx) * (fluxes[i + 1].momentum - fluxes[i].momentum);
    }

    return {newWater, state.time + timeStep};
}