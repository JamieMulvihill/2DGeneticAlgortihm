#include "GA.h"
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <iostream>

GA::GA(sf::Vector2f startPos, sf::Vector2f padPosition, sf::FloatRect padBounds)
    : startPos(startPos)
    , padPosition(padPosition)
    , padBounds(padBounds)
{
    population.reserve(populationSize);
    for (int i = 0; i < populationSize; i++)
        population.emplace_back(startPos);

    fitnessScores.resize(populationSize, 0.f);
}

void GA::update(float dt)
{
    generationTimer += dt;

    for (auto& rocket : population)
    {
        if (rocket.getState() != RocketStatus::Flying) continue;

        FlightState fs = rocket.getFlightState(padPosition);
        ControlOutput output = rocket.GetBrain().think(fs);

        if (output.thrustUp)    rocket.thrustUp();
        if (output.rotateLeft)  rocket.rotateLeft();
        if (output.rotateRight) rocket.rotateRight();

        rocket.update(dt);
        rocket.checkLanding(padBounds);
    }

    if (allDone() || generationTimer >= maxGenerationTime)
        nextGeneration();
}

void GA::render(sf::RenderWindow& window)
{
    for (auto& rocket : population)
        rocket.draw(window);
}

bool GA::allDone() const
{
    for (const auto& rocket : population)
        if (rocket.getState() == RocketStatus::Flying)
            return false;
    return true;
}

float GA::fitnessScore(const Rocket& rocket) const
{
    sf::FloatRect bounds = rocket.getBounds();
    sf::Vector2f rocketPos = sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    );

    // distance to pad
    float dx = rocketPos.x - padPosition.x;
    float dy = rocketPos.y - padPosition.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    float score = 1000.f - distance; // closer = better

    if (rocket.getState() == RocketStatus::Landed)
        score += 5000.f; // big bonus for landing

    if (rocket.getState() == RocketStatus::Crashed)
        score -= 500.f; // penalty for crashing hard

    return std::max(0.f, score);
}

void GA::evaluate()
{
    for (int i = 0; i < populationSize; i++)
        fitnessScores[i] = fitnessScore(population[i]);
}

void GA::select()
{
    // sort indices by fitness
    std::vector<int> indices(populationSize);
    for (int i = 0; i < populationSize; i++) indices[i] = i;

    std::sort(indices.begin(), indices.end(), [&](int a, int b)
        {
            return fitnessScores[a] > fitnessScores[b];
        });

    // keep top 20%
    int topCount = populationSize / 5;
    selectedWeights.clear();
    for (int i = 0; i < topCount; i++)
        selectedWeights.push_back(population[indices[i]].GetBrain().weights);
}

void GA::crossover(const std::vector<std::vector<float>>& parents)
{
    for (int i = 0; i < populationSize; i++)
    {
        // pick two random parents
        int a = rand() % parents.size();
        int b = rand() % parents.size();

        // crossover point
        int crossPoint = rand() % Brain::weightCount;

        std::vector<float> childWeights(Brain::weightCount);
        for (int w = 0; w < Brain::weightCount; w++)
            childWeights[w] = (w < crossPoint) ? parents[a][w] : parents[b][w];

        population[i].GetBrain().weights = childWeights;
    }
}

void GA::mutate()
{
    for (auto& rocket : population)
    {
        for (float& w : rocket.GetBrain().weights)
        {
            if ((float)rand() / RAND_MAX < mutationRate)
                w += (((float)rand() / RAND_MAX) * 2.f - 1.f) * mutationStrength;

            // clamp weights
            w = std::max(-2.f, std::min(2.f, w));
        }
    }
}

void GA::nextGeneration()
{
    evaluate();

    float best = *std::max_element(fitnessScores.begin(), fitnessScores.end());
    float worst = *std::min_element(fitnessScores.begin(), fitnessScores.end());
    float avg = 0.f;
    for (float s : fitnessScores) avg += s;
    avg /= fitnessScores.size();

    std::cout << "Gen " << generation
        << " | Best: " << best
        << " | Avg: " << avg
        << " | Worst: " << worst << "\n";

    int landed = 0;
    int crashed = 0;
    for (const auto& rocket : population)
    {
        if (rocket.getState() == RocketStatus::Landed) landed++;
        if (rocket.getState() == RocketStatus::Crashed) crashed++;
    }
    std::cout << "Landed: " << landed << " | Crashed: " << crashed << "\n\n";


    select();
    crossover(selectedWeights);
    mutate();

    // reset all rockets
    for (auto& rocket : population)
        rocket.reset();

    generationTimer = 0.f;
    generation++;
}