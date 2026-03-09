#pragma once
#include <SFML/Graphics.hpp>
#include "Rocket.h"
#include "Input.h"
#include "LandingPad.h"


class Game
{
public:
    Game();
    void run();

private:
    void handleEvents();
    void update(float dt);
    void render();

    sf::RenderWindow window;
    sf::Clock clock;

    Input input;

    Rocket rocket;
    LandingPad landingPad;

    bool aiControlled = false;
};