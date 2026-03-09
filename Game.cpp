#include "Game.h"

Game::Game()
    : window(sf::VideoMode(sf::Vector2u(800, 600)), sf::String("SFML Window"))
    , landingPad(sf::Vector2f(400.f, 550.f), sf::Vector2f(120.f, 15.f))
    , ga(sf::Vector2f(400.f, 50.f), sf::Vector2f(400.f, 550.f), landingPad.getBounds())
{
    if (!font.openFromFile("arial/arial.ttf"))
    {
        // handle error - make sure arial.ttf is in your project directory
    }

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
    }
}

void Game::update(float dt)
{
    ga.update(dt);
}

void Game::render()
{
    window.clear();
    ga.render(window);
    landingPad.draw(window);
    renderHUD();
    window.display();
}

void Game::renderHUD()
{
    sf::Text genText(font);
    genText.setString("Generation: " + std::to_string(ga.getGeneration()));
    genText.setCharacterSize(20);
    genText.setFillColor(sf::Color::White);
    genText.setPosition(sf::Vector2f(10.f, 10.f));
    window.draw(genText);

    sf::Text modeText(font);
    modeText.setString("Rockets: 50 | Time: " + std::to_string((int)ga.getTimer()) + "s");
    modeText.setCharacterSize(20);
    modeText.setFillColor(sf::Color::Yellow);
    modeText.setPosition(sf::Vector2f(10.f, 35.f));
    window.draw(modeText);
}