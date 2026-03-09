#pragma once
#include <SFML/Graphics.hpp>

class LandingPad
{
public:
    LandingPad(sf::Vector2f position, sf::Vector2f size);

    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape shape;
};
