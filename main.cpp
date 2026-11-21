// Currently only includes fluid dynamics simulation, will add particle gravity when
// rendering scheme done...
#include <vector>
#include <cstdint>
#include <cstddef>
#include <chrono>
#include <thread>

#include "Engines/FluidDynamics/WaterSolver.h"
#include "Engines/FluidDynamics/PhysicsState.h"
#include "Engines/FluidDynamics/PhysicsConfig.h"
#include "Engines/LED/FluidRenderer.h"
#include "Engines/LED/LEDFrame.h"
#include "Engines/LED/LEDSerialiser.h"
#include "Communication/SerialPort.h"

int main() {
    constexpr std::size_t NUM_CELLS = 100;
    constexpr std::size_t NUM_LEDS = 30;
    constexpr double LENGTH = 1.0;
    constexpr double DISPLAY_DT = 1.0 / 30.0;
    constexpr double SIMULATION_DURATION = 10.0;
    constexpr double SIMULATION_SPEED = 0.1;

    const double dx = LENGTH / NUM_CELLS;

    PhysicsConfig config;
    PhysicsState state;
    state.water.resize(NUM_CELLS);

    for (std::size_t i = 0; i < NUM_CELLS; ++i) {
        state.water[i].depth = i < NUM_CELLS / 2 ? 0.50 : 0.001;
        state.water[i].discharge = 0.0;
    }

    WaterSolver solver;
    FluidRenderer renderer(0.0, 0.50);

    SerialPort serial("\\\\.\\COM8");
    std::this_thread::sleep_for(std::chrono::seconds(2));

    using Clock = std::chrono::steady_clock;
    const auto startTime = Clock::now();

    double nextDisplayTime = DISPLAY_DT;
    double nextSimulationTime = DISPLAY_DT * SIMULATION_SPEED;

    while (nextSimulationTime <= SIMULATION_DURATION) {
        while (state.time < nextSimulationTime) {
            state = solver.advance(state, dx, config);
        }

        std::vector<double> depths;
        depths.reserve(state.water.size());

        for (const WaterCell& cell : state.water) {
            depths.push_back(cell.depth);
        }

        LEDFrame frame = renderer.render(depths, NUM_LEDS);
        std::vector<uint8_t> serialisedFrame = serialiseLEDFrame(frame);

        auto targetTime = startTime +
            std::chrono::duration_cast<Clock::duration>(
                std::chrono::duration<double>(nextDisplayTime)
            );

        std::this_thread::sleep_until(targetTime);

        serial.send(serialisedFrame);

        nextDisplayTime += DISPLAY_DT;
        nextSimulationTime += DISPLAY_DT * SIMULATION_SPEED;
    }

    LEDFrame offFrame(NUM_LEDS);
    serial.send(serialiseLEDFrame(offFrame));

    return 0;
}
