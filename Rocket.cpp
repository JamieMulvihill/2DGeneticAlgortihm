#include "Rocket.h"
#include <cmath>

static const float DEG_TO_RAD = 3.14159265f / 180.f;

Rocket::Rocket(sf::Vector2f startPos)
    : position(startPos), velocity(0.f, 0.f), rotation(-90.f), angularVel(0.f)
{
    buildShape();
}

void Rocket::buildShape()
{
    shape.setPointCount(3);
    shape.setPoint(0, sf::Vector2f(0.f, -15.f));  
    shape.setPoint(1, sf::Vector2f(-10.f, 15.f)); 
    shape.setPoint(2, sf::Vector2f(10.f, 15.f));  
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(sf::Vector2f(0.f, 0.f));
    shape.setPosition(position);
}

void Rocket::update(float dt)
{
    if (state != RocketStatus::Flying) return;

    // Apply gravity
    velocity.y += gravity * dt;

    // Update position
    position += velocity * dt;

    // Update rotation
    rotation += angularVel * dt;
    angularVel *= 0.95f; // dampen spin over time

    shape.setPosition(position);
    shape.setRotation(sf::degrees(rotation));
}

void Rocket::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

void Rocket::thrustUp()
{
    // Thrust in the direction the rocket is pointing
    float rad = (rotation - 90.f) * DEG_TO_RAD;
    velocity.x += std::cos(rad) * thrustPower;
    velocity.y += std::sin(rad) * thrustPower;
}

void Rocket::rotateLeft() { angularVel -= rotatePower; }
void Rocket::rotateRight() { angularVel += rotatePower; }

sf::FloatRect Rocket::getBounds() const
{
    return shape.getGlobalBounds();
}

void Rocket::checkLanding(sf::FloatRect padBounds)
{
    if (!getBounds().findIntersection(padBounds)) return;
    if (state != RocketStatus::Flying) return;

    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float tilt = std::fmod(std::abs(rotation), 360.f);

    if (speed < 70.f && tilt < 25.f)
    {
        state = RocketStatus::Landed;
        velocity = sf::Vector2f(0.f, 0.f);
        angularVel = 0.f;
        shape.setFillColor(sf::Color::Green);
    }
    else
    {
        state = RocketStatus::Crashed;
        velocity = sf::Vector2f(0.f, 0.f);
        shape.setFillColor(sf::Color::Red);
    }
}

void Rocket::reset()
{
    position = sf::Vector2f(400.f, 50.f);
    velocity = sf::Vector2f(0.f, 0.f);
    rotation = -90.f;
    angularVel = 0.f;
    state = RocketStatus::Flying;
    shape.setFillColor(sf::Color::White);
    shape.setPosition(position);
}


FlightState Rocket::getFlightState(sf::Vector2f padPosition) const
{
    FlightState fs;
    fs.relativeX = (position.x - padPosition.x) / 800.f; // normalise by window width
    fs.relativeY = (position.y - padPosition.y) / 600.f; // normalise by window height
    fs.velocityX = velocity.x / 500.f;                   // normalise by max expected speed
    fs.velocityY = velocity.y / 500.f;
    fs.rotation = rotation / 180.f;                      // normalise to -1..1 range
    fs.angularVel = angularVel / 200.f;
    return fs;
}