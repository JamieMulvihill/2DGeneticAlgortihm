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
    {
        // random weight between -1 and 1
        w = ((float)rand() / RAND_MAX) * 2.f - 1.f;
    }
}

float Brain::activate(float value) const
{
    // sigmoid — squashes value to 0..1
    return 1.f / (1.f + std::exp(-value));
}

ControlOutput Brain::think(const FlightState& fs) const
{
    // pack state into array for easy iteration
    float inputs[inputCount] = {
        fs.relativeX,
        fs.relativeY,
        fs.velocityX,
        fs.velocityY,
        fs.rotation,
        fs.angularVel
    };

    ControlOutput output;
    float raw[outputCount] = { 0.f, 0.f, 0.f };

    // simple single layer — each output is a weighted sum of all inputs
    for (int o = 0; o < outputCount; o++)
    {
        for (int i = 0; i < inputCount; i++)
        {
            raw[o] += inputs[i] * weights[o * inputCount + i];
        }
        raw[o] = activate(raw[o]);
    }

    // threshold at 0.5 to get boolean controls
    output.thrustUp = raw[0] > 0.5f;
    output.rotateLeft = raw[1] > 0.5f;
    output.rotateRight = raw[2] > 0.5f;

    return output;
}