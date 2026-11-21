#include <algorithm>
#include <cmath>
#include <cstdint>

#include "FluidRenderer.h"

FluidRenderer::FluidRenderer(double minDepth, double maxDepth)
    : minDepth(minDepth), maxDepth(maxDepth) {
}

double FluidRenderer::normalise(double depth) {
    if (minDepth == maxDepth) {
        return 0.0;
    }
    return std::clamp((depth - minDepth) / (maxDepth - minDepth), 0.0, 1.0);
}

RGB FluidRenderer::interpolate(RGB c0, RGB c1, double t) {
    double clampedT = std::clamp(t, 0.0, 1.0);
    return {
        static_cast<uint8_t>(std::round(c0.r + (c1.r - c0.r) * clampedT)),
        static_cast<uint8_t>(std::round(c0.g + (c1.g - c0.g) * clampedT)),
        static_cast<uint8_t>(std::round(c0.b + (c1.b - c0.b) * clampedT))
    };
}

RGB FluidRenderer::depthToColour(double t) {
    double clampedT = std::clamp(t, 0.0, 1.0);
    int index = static_cast<int>(clampedT * (depthColors.size() - 1));
    double localT = (clampedT * (depthColors.size() - 1)) - index;
    return interpolate(depthColors[index], depthColors[std::min(index + 1, static_cast<int>(depthColors.size() - 1))], localT);
}


double FluidRenderer::averageDepth(
    const std::vector<double>& depths,
    std::size_t ledIndex,
    std::size_t numLEDs
) {
    if (depths.empty() || numLEDs == 0) {
        return 0.0;
    }
    const double startIndex =
        static_cast<double>(ledIndex) * depths.size() / numLEDs;
    const double endIndex =
        static_cast<double>(ledIndex + 1) * depths.size() / numLEDs;

    double weightedSum = 0.0;

    for (std::size_t i = 0; i < depths.size(); ++i) {
        double overlap = std::max(
            0.0,
            std::min(static_cast<double>(i + 1), endIndex)
                - std::max(static_cast<double>(i), startIndex)
        );

        weightedSum += overlap * depths[i];
    }

    return weightedSum / (endIndex - startIndex);
}

LEDFrame FluidRenderer::render(
    const std::vector<double>& depths,
    std::size_t numLEDs
) {
    LEDFrame frame(numLEDs);

    for (std::size_t i = 0; i < numLEDs; i++) {
        double avgDepth = averageDepth(depths, i, numLEDs);
        double normalised = normalise(avgDepth);
        RGB colour = depthToColour(normalised);
        frame[i].on = avgDepth > 1e-6 ? 1 : 0;
        frame[i].r = colour.r;
        frame[i].g = colour.g;
        frame[i].b = colour.b;
    }
    
    return frame;
}
