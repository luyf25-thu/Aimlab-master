#include "TargetSpawner.h"

#include "MathUtils.h"

#include <SFML/Graphics/Color.hpp>

#include <algorithm>

TargetSpawner::TargetSpawner(TargetPool* pool, GameMode* mode, float interval)
    : poolRef(pool), modeRef(mode), spawnInterval(interval)
{
}

void TargetSpawner::setMode(GameMode* mode)
{
    modeRef = mode;
    if (modeRef)
    {
        modeRef->setSpacingScale(spacingScale);
        modeRef->setGridScale(spacingScale);
    }
}

void TargetSpawner::setRadiusScale(float scale)
{
    radiusScale = std::max(1.0f, scale);
}

void TargetSpawner::setSpacingScale(float scale)
{
    spacingScale = std::max(1.0f, scale);
    if (modeRef)
    {
        modeRef->setSpacingScale(spacingScale);
    }
}

void TargetSpawner::update(float deltaTime, const sf::Vector2u& areaSize)
{
    timeSinceLastSpawn += deltaTime;
    if (!poolRef)
    {
        return;
    }

    const int desiredCount = modeRef ? modeRef->getDesiredActiveCount() : 1;
    int activeCount = countActiveTargets();
    std::vector<sf::Vector2f> occupiedPositions = getActivePositions();

    if (spawnInterval > 0.0f && timeSinceLastSpawn < spawnInterval)
    {
        return;
    }

    timeSinceLastSpawn = 0.0f;
    while (activeCount < desiredCount)
    {
        Target* target = poolRef->acquireTarget();
        if (!target)
        {
            return;
        }

        if (modeRef)
        {
            const SpawnInfo info = modeRef->createSpawn(areaSize, occupiedPositions);
            target->init(info.position, info.radius * radiusScale, info.lifeTime,
                         info.requiredHits, info.baseColor);
            occupiedPositions.push_back(info.position);
        }
        else
        {
            const float radius = MathUtils::getRandomFloat(15.0f, 35.0f) * radiusScale;
            const float x = MathUtils::getRandomFloat(radius, static_cast<float>(areaSize.x) - radius);
            const float y = MathUtils::getRandomFloat(radius, static_cast<float>(areaSize.y) - radius);
            const float lifeTime = MathUtils::getRandomFloat(1.5f, 3.5f);
            target->init({ x, y }, radius, lifeTime, 1, sf::Color::Red);
        }

        ++activeCount;
        if (spawnInterval > 0.0f)
        {
            break;
        }
    }
}

int TargetSpawner::countActiveTargets() const
{
    int count = 0;
    for (const auto& target : poolRef->getTargets())
    {
        if (target->getIsActive())
        {
            ++count;
        }
    }
    return count;
}

std::vector<sf::Vector2f> TargetSpawner::getActivePositions() const
{
    std::vector<sf::Vector2f> positions;
    for (const auto& target : poolRef->getTargets())
    {
        if (target->getIsActive())
        {
            positions.push_back(target->getPosition());
        }
    }
    return positions;
}
