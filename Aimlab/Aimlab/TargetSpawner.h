#pragma once

#include "GameMode.h"
#include "TargetPool.h"

#include <SFML/System/Vector2.hpp>

#include <vector>

class TargetSpawner
{
public:
    explicit TargetSpawner(TargetPool* pool, GameMode* mode, float interval = 1.0f);

    void setMode(GameMode* mode);
    void setSpacingScale(float scale);
    void update(float deltaTime, const sf::Vector2u& areaSize);

private:
    int countActiveTargets() const;
    std::vector<sf::Vector2f> getActivePositions() const;

    TargetPool* poolRef = nullptr;
    GameMode* modeRef = nullptr;
    float spawnInterval = 1.0f;
    float timeSinceLastSpawn = 0.0f;
    float spacingScale = 1.0f;
};
