#pragma once
#include <SFML/Graphics.hpp>
#include "Brain.h"
#include "Types.h"

class Brain;

class Rocket
{
public:
    Rocket(sf::Vector2f startPos);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    void thrustUp();
    void rotateLeft();
    void rotateRight();

    sf::FloatRect getBounds() const;
    RocketStatus getState() const { return state; }
    void checkLanding(sf::FloatRect padBounds);
    void reset();
    void buildShape();

    FlightState getFlightState(sf::Vector2f padPosition) const;

    Brain GetBrain() { return brain; }

private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float rotation;       // degrees
    float angularVel;

    const float gravity = 98.f;
    const float thrustPower = .1f;
    const float rotatePower = 8.0f;

    Brain brain;

    RocketStatus state = RocketStatus::Flying;

    sf::ConvexShape shape;
    sf::Vector2f startPos;

};

