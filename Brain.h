#pragma once
#include <vector>
#include "Types.h"

class Brain
{
public:
    Brain();
    ControlOutput think(const FlightState& state) const;
    void randomise();

    std::vector<float> weights; // public so GA can access and evolve them

    static const int inputCount = 6;
    static const int outputCount = 3;
    static const int weightCount = inputCount * outputCount; // 18 weights


private:
    float activate(float value) const; // sigmoid
};