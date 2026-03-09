#pragma once
struct FlightState
{
    float relativeX;      // normalised distance to pad on x
    float relativeY;      // normalised distance to pad on y
    float velocityX;      // normalised
    float velocityY;      // normalised
    float rotation;       // normalised
    float angularVel;     // normalised
};


struct ControlOutput
{
    bool thrustUp = false;
    bool rotateLeft = false;
    bool rotateRight = false;
};

enum class RocketStatus { Flying, Landed, Crashed };