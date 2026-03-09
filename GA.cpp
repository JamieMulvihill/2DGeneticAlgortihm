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
    srand(static_cast<unsigned>(time(nullptr)));

    population.reserve(populationSize);
    for (int i = 0; i < populationSize; i++)
    {
        float randomX = 100.f + ((float)rand() / RAND_MAX) * 600.f;
        population.emplace_back(sf::Vector2f(randomX, 50.f));
    }

    fitnessScores.resize(populationSize, 0.f);
}

void GA::update(float dt)
{
    generationTimer += dt;

    for (auto& rocket : population)
    {
        if (rocket.getState() != RocketStatus::Flying) continue;

        rocket.updateBestDistance(padPosition);

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
    float score = 0.f;

    // 1. PROXIMITY - normalise to 0-3000 range (most important)
    float proximityScore = std::max(0.f, 1.f - (rocket.getBestDistance() / 800.f));
    score += proximityScore * 3000.f;

    // 2. UPRIGHT - 0-2000 range
    float tilt = std::fmod(std::abs(rocket.getRotation()), 360.f);
    if (tilt > 180.f) tilt = 360.f - tilt;
    float uprightScore = std::max(0.f, 1.f - (tilt / 180.f));
    score += uprightScore * 2000.f;

    // 3. TIME ALIVE - max ~150 over 15 seconds (minor)
    score += rocket.getTimeAlive() * 100.f;

    // 4. SLOW NEAR PAD
    sf::FloatRect bounds = rocket.getBounds();
    sf::Vector2f finalPos = sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    );
    float dx = finalPos.x - padPosition.x;
    float dy = finalPos.y - padPosition.y;
    float finalDist = std::sqrt(dx * dx + dy * dy);
    sf::Vector2f vel = rocket.getVelocity();
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (finalDist < 150.f)
    {
        float slowScore = std::max(0.f, 1.f - (speed / 200.f));
        score += slowScore * 1000.f;
    }

    // 5. OUTCOMES
    if (rocket.getState() == RocketStatus::Crashed) score -= 2500.f;
    if (rocket.getState() == RocketStatus::Landed)  score += 5000.f;

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

    population.back().GetBrain().randomise();
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


    int bestIdx = std::max_element(fitnessScores.begin(), fitnessScores.end())
        - fitnessScores.begin();

    FlightState testState;
    testState.relativeX = 0.f;   // directly above pad
    testState.relativeY = -0.5f;  // halfway up screen
    testState.velocityX = 0.f;
    testState.velocityY = 0.3f;  // falling
    testState.rotation = 0.f;   // upright
    testState.angularVel = 0.f;

    ControlOutput bestOutput = population[bestIdx].GetBrain().think(testState);
    std::cout << "Best brain decisions: "
        << "Thrust=" << bestOutput.thrustUp << " "
        << "Left=" << bestOutput.rotateLeft << " "
        << "Right=" << bestOutput.rotateRight << "\n";

    auto& w = population[bestIdx].GetBrain().weights;
    float wMin = *std::min_element(w.begin(), w.end());
    float wMax = *std::max_element(w.begin(), w.end());
    std::cout << "Weight range: " << wMin << " to " << wMax << "\n\n";

    select();
    crossover(selectedWeights);
    mutate();

    for (int i = 0; i < 2; i++)
        population[i].GetBrain().weights = selectedWeights[i];

    // reset all rockets
    for (auto& rocket : population)
    {
        float randomX = 100.f + ((float)rand() / RAND_MAX) * 600.f;
        rocket.setStartPos(sf::Vector2f(randomX, 50.f));
        rocket.reset();
    }

    generationTimer = 0.f;
    generation++;
}