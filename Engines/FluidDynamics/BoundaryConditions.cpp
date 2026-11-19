#include "BoundaryConditions.h"

WaterCell reflectiveGhost(const WaterCell& interiorCell) {
    WaterCell ghostCell;
    ghostCell.depth = interiorCell.depth;
    ghostCell.discharge = -interiorCell.discharge;
    return ghostCell;
}