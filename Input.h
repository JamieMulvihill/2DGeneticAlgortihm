#pragma once
#include <SFML/Graphics.hpp>

class Input
{
public:
    bool thrustUp = false;
    bool rotateLeft = false;
    bool rotateRight = false;

    void update();
};