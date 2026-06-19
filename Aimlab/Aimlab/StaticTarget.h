#pragma once

#include "Target.h"

class StaticTarget : public Target
{
public:
    void init(const sf::Vector2f& startPos, float r, float lifeTime,
              int requiredHitsParam, const sf::Color& baseColorParam) override;
    void update(float deltaTime) override;
    bool onHit() override;

private:
    bool persistent = false;
};
