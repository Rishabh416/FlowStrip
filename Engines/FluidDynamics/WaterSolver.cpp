#include "WaterSolver.h"

double WaterSolver::calculateVelocity(
    const WaterCell& cell
) const {
    if (cell.depth <= 0.0) {
        return 0.0;
    }

    return cell.discharge / cell.depth;
}