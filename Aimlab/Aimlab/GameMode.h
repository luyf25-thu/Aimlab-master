#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

struct SpawnInfo
{
    sf::Vector2f position;
    float radius = 0.0f;
    float lifeTime = 0.0f;
    int requiredHits = 1;
    sf::Color baseColor = sf::Color::Red;
};

class GameMode
{
public:
    virtual ~GameMode() = default;

    virtual SpawnInfo createSpawn(const sf::Vector2u& areaSize,
                                  const std::vector<sf::Vector2f>& occupiedPositions) = 0;

    virtual void onTargetHit(const sf::Vector2f& position)
    {
        (void)position;
    }

    virtual void reset()
    {
    }

    virtual int getDesiredActiveCount() const
    {
        return 1;
    }

    virtual void setSpacingScale(float scale)
    {
        (void)scale;
    }

    virtual void setGridScale(float scale)
    {
        (void)scale;
    }
};
