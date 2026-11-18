#include "NumericalFlux.h"
#include <cmath>
#include <algorithm>

// Shallow water flux F(U) = [q, q^2/h + 0.5 * g * h^2], q = h*u
WaterFlux physicalFlux(
    const WaterCell& cell,
    double gravity
) {
    if (cell.depth <= 0.0) {
        return WaterFlux{0.0, 0.0};
    }

    double discharge = cell.discharge;
    double depth = cell.depth;

    double momentum = discharge * discharge / depth;
    double pressure = 0.5 * gravity * depth * depth;

    return WaterFlux{discharge, momentum + pressure};
}

// See: Harten-Lax-van Leer Riemann solver
// TODO: write tests lol I spent a lot of time understanding the physics of this
// and I am not sure if I implemented it correctly...
// if interested in understanding/verifying/testing, the readme contains the resources I used
// TODO: figure out what to do for dry cells...
WaterFlux hllFlux(
    const WaterCell& left,
    const WaterCell& right,
    double gravity
) {
    WaterFlux leftFlux = physicalFlux(left, gravity);
    WaterFlux rightFlux = physicalFlux(right, gravity);

    double leftVelocity = left.depth > 0.0 ? left.discharge / left.depth : 0.0;
    double leftWaveSpeed = std::sqrt(gravity * left.depth);
    
    double rightVelocity = right.depth > 0.0 ? right.discharge / right.depth : 0.0;
    double rightWaveSpeed = std::sqrt(gravity * right.depth);

    double leftCharacteristicSpeed = std::min(
        leftVelocity - leftWaveSpeed,
        rightVelocity - rightWaveSpeed
    );

    double rightCharacteristicSpeed = std::max(
        leftVelocity + leftWaveSpeed,
        rightVelocity + rightWaveSpeed
    );

    if (leftCharacteristicSpeed >= 0) {
        return leftFlux;
    }

    if (rightCharacteristicSpeed <= 0) {
        return rightFlux;
    }

    double mass = rightCharacteristicSpeed * leftFlux.mass - 
                  leftCharacteristicSpeed * rightFlux.mass + 
                  leftCharacteristicSpeed * rightCharacteristicSpeed * 
                  (right.depth - left.depth);

    double momentum = rightCharacteristicSpeed * leftFlux.momentum -
                      leftCharacteristicSpeed * rightFlux.momentum +
                      leftCharacteristicSpeed * rightCharacteristicSpeed * 
                      (right.discharge - left.discharge);

    double denominator = rightCharacteristicSpeed - leftCharacteristicSpeed;

    return WaterFlux{mass/denominator, momentum/denominator};
}