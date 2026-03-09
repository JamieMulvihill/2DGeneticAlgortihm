#include "Game.h"

Game::Game()
    : window(sf::VideoMode(sf::Vector2u(800, 600)), sf::String("SFML Window"))
    , rocket(sf::Vector2f(400.f, 50.f))
    , landingPad(sf::Vector2f(400.f, 550.f), sf::Vector2f(120.f, 15.f))
{
}

void Game::run()
{
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents()
{
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();

        if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPress->code == sf::Keyboard::Key::Tab)
            {
                aiControlled = !aiControlled;
                //rocket.reset();
            }
            if (keyPress->code == sf::Keyboard::Key::R)
                rocket.reset();
        }
    }
}

void Game::update(float dt)
{
    if (rocket.getState() != RocketStatus::Flying) return;

    if (aiControlled)
    {
        
        sf::Vector2f padPos = sf::Vector2f(400.f, 550.f);
        FlightState fs = rocket.getFlightState(padPos);
        ControlOutput output = rocket.GetBrain().think(fs);

        if (output.thrustUp)    rocket.thrustUp();
        if (output.rotateLeft)  rocket.rotateLeft();
        if (output.rotateRight) rocket.rotateRight();
    }
    else
    {
        
        input.update();
        if (input.thrustUp)    rocket.thrustUp();
        if (input.rotateLeft)  rocket.rotateLeft();
        if (input.rotateRight) rocket.rotateRight();
    }

    rocket.update(dt);
    rocket.checkLanding(landingPad.getBounds());
}

void Game::render()
{
    window.clear();
    rocket.draw(window);
    landingPad.draw(window);
    window.display();
}