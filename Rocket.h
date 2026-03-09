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

    Brain& GetBrain() { return brain; }
    float getBestDistance() const { return bestDistance; }

    void updateBestDistance(sf::Vector2f padPosition);

    sf::Vector2f getVelocity() const { return velocity; }
    float getRotation() const { return rotation; }
    float getTimeAlive() const { return timeAlive; }
    void setStartPos(sf::Vector2f pos) { startPos = pos; }

private:
    float timeAlive = 0.f;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float rotation;       // degrees
    float angularVel;

    const float gravity = 98.f;
    const float thrustPower = .075f;
    const float rotatePower = 10.0f;

    float bestDistance = 99999.f;

    Brain brain;

    RocketStatus state = RocketStatus::Flying;

    sf::ConvexShape shape;
    sf::Vector2f startPos;

};

