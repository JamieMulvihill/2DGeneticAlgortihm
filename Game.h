#pragma once
#include <SFML/Graphics.hpp>
#include "GA.h"
#include "LandingPad.h"
#include "Input.h"

class Game
{
public:
    Game();
    void run();

    void renderHUD();

private:
    void handleEvents();
    void update(float dt);
    void render();

    sf::RenderWindow window;
    sf::Clock clock;
    LandingPad landingPad;
    GA ga;
    sf::Font font;

    bool manualMode = false;
    Rocket testRocket;
    Input input;
};