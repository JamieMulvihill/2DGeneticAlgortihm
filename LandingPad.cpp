#include "LandingPad.h"

LandingPad::LandingPad(sf::Vector2f position, sf::Vector2f size)
{
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(sf::Vector2f(size.x / 2.f, 0.f)); // centre horizontally
}

void LandingPad::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

sf::FloatRect LandingPad::getBounds() const
{
    return shape.getGlobalBounds();
}