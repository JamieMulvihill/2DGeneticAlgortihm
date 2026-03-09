#include "Input.h"

void Input::update()
{
    thrustUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
    rotateLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    rotateRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
}