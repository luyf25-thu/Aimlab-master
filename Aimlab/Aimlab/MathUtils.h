#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <random>


class MathUtils
{
public:
    static float distance(const sf::Vector2f& p1, const sf::Vector2f& p2)
    {
        const float dx = p1.x - p2.x;
        const float dy = p1.y - p2.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    static bool checkCirclePointCollision(const sf::Vector2f& center, float radius, const sf::Vector2f& point)
    {
        return distance(center, point) <= radius;
    }

    static float getRandomFloat(float minValue, float maxValue)
    {
        std::uniform_real_distribution<float> dist(minValue, maxValue);
        return dist(getRng());
    }

private:
    static std::mt19937& getRng()
    {
        static std::mt19937 rng(std::random_device{}());
        return rng;
    }
};

