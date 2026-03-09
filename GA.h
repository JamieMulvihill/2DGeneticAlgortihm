#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Rocket.h"
#include "LandingPad.h"

class GA
{
public:
    GA(sf::Vector2f startPos, sf::Vector2f padPosition, sf::FloatRect padBounds);

    void update(float dt);
    void render(sf::RenderWindow& window);

    int getGeneration() const { return generation; }
    float getTimer() const { return generationTimer; }
private:
    void evaluate();
    void select();
    void crossover(const std::vector<std::vector<float>>& parents);
    void mutate();
    void nextGeneration();
    float fitnessScore(const Rocket& rocket) const;
    bool allDone() const;

    std::vector<Rocket> population;
    sf::Vector2f startPos;
    sf::Vector2f padPosition;
    sf::FloatRect padBounds;

    std::vector<float> fitnessScores;
    std::vector<std::vector<float>> selectedWeights;

    int generation = 1;
    float generationTimer = 0.f;

    static const int populationSize = 50;
    const float maxGenerationTime = 10.f; // seconds before force next gen
    const float mutationRate = 0.1f;
    const float mutationStrength = 0.5f;
};

