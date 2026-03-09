#pragma once
#include <vector>
#include "Types.h"

class Brain
{
public:
    Brain();
    ControlOutput think(const FlightState& state) const;
    void randomise();

    std::vector<float> weights;

    static const int inputCount = 6;
    static const int hiddenCount = 8;
    static const int outputCount = 3;
    // 6*8 + 8*3 = 48 + 24 = 72 weights
    static const int weightCount = (inputCount * hiddenCount) + (hiddenCount * outputCount);

private:
    float activate(float value) const;
};