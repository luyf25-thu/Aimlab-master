#include "StaticTarget.h"

#include <algorithm>
#include <cstdint>

void StaticTarget::init(const sf::Vector2f& startPos, float r, float lifeTime,
                        int requiredHitsParam, const sf::Color& baseColorParam)
{
    position = startPos;
    radius = r;
    lifeTimer = lifeTime;
    persistent = lifeTime < 0.0f;
    isActive = true;
    isHitState = false;
    hitTimer = 0.0f;
    requiredHits = std::max(1, requiredHitsParam);
    currentHits = 0;
    baseColor = baseColorParam;
    shape.setRadius(radius);
    shape.setOrigin({ radius, radius });
    shape.setPosition(position);
    shape.setFillColor(baseColor);
}

void StaticTarget::update(float deltaTime)
{
    if (!isActive)
    {
        return;
    }

    if (isHitState)
    {
        hitTimer -= deltaTime;
        if (hitTimer <= 0.0f)
        {
            isActive = false;
        }
        return;
    }

    if (persistent)
    {
        return;
    }

    lifeTimer -= deltaTime;
    if (lifeTimer <= 0.0f)
    {
        isActive = false;
    }
}

bool StaticTarget::onHit()
{
    if (!isActive || isHitState)
    {
        return false;
    }

    ++currentHits;
    if (currentHits < requiredHits)
    {
        const float ratio = static_cast<float>(currentHits) / static_cast<float>(requiredHits);
        const float lighten = 0.6f * ratio;
        const auto toByte = [](float value)
        {
            return static_cast<std::uint8_t>(std::clamp(value, 0.0f, 255.0f));
        };
        const float r = baseColor.r + (255.0f - baseColor.r) * lighten;
        const float g = baseColor.g + (255.0f - baseColor.g) * lighten;
        const float b = baseColor.b + (255.0f - baseColor.b) * lighten;
        shape.setFillColor(sf::Color{ toByte(r), toByte(g), toByte(b) });
        return false;
    }

    isHitState = true;
    hitTimer = hitDisplayDuration;
    shape.setFillColor(sf::Color::Green);
    return true;
}
