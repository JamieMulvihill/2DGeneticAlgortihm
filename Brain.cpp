#include "Brain.h"
#include <cmath>
#include <cstdlib>

Brain::Brain()
{
    weights.resize(weightCount, 0.f);
    randomise();
}

void Brain::randomise()
{
    for (float& w : weights)
        w = ((float)rand() / RAND_MAX) * 2.f - 1.f;
}

float Brain::activate(float value) const
{
    return 1.f / (1.f + std::exp(-value));
}

ControlOutput Brain::think(const FlightState& fs) const
{
    float inputs[inputCount] = {
        fs.relativeX,
        fs.relativeY,
        fs.velocityX,
        fs.velocityY,
        fs.rotation,
        fs.angularVel
    };

    float hidden[hiddenCount] = {};
    for (int h = 0; h < hiddenCount; h++)
    {
        float sum = 0.f;
        for (int i = 0; i < inputCount; i++)
            sum += inputs[i] * weights[h * inputCount + i];
        hidden[h] = activate(sum);
    }

    int outputOffset = inputCount * hiddenCount;
    float raw[outputCount] = {};
    for (int o = 0; o < outputCount; o++)
    {
        float sum = 0.f;
        for (int h = 0; h < hiddenCount; h++)
            sum += hidden[h] * weights[outputOffset + o * hiddenCount + h];
        raw[o] = activate(sum);
    }

    ControlOutput output;
    output.thrustUp = raw[0] > 0.5f;
    output.rotateLeft = raw[1] > 0.5f;
    output.rotateRight = raw[2] > 0.5f;
    return output;
}