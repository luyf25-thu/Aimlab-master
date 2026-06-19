#pragma once

#include <SFML/Graphics.hpp>
#include "MathUtils.h"

class Target
{
public:
    virtual ~Target() = default;

    virtual void init(const sf::Vector2f& startPos, float r, float lifeTime,
                      int requiredHits, const sf::Color& baseColor) = 0;


    virtual void update(float deltaTime) = 0;

    virtual bool onHit() = 0;


    void render(sf::RenderWindow& window)
    {
        if (!isActive)
        {
            return;
        }
        window.draw(shape);
    }

    bool isHit(const sf::Vector2f& mousePos) const
    {
        if (!isActive || isHitState)
        {
            return false;
        }
        return MathUtils::checkCirclePointCollision(position, radius, mousePos);
    }

    bool getIsActive() const
    {
        return isActive;
    }

    void deactivate()
    {
        isActive = false;
    }


    sf::Vector2f getPosition() const
    {
        return position;
    }

protected:
    sf::CircleShape shape;
    sf::Vector2f position;
    float radius = 0.0f;
    float lifeTimer = 0.0f;
    bool isActive = false;
    bool isHitState = false;
    float hitTimer = 0.0f;
    float hitDisplayDuration = 0.12f;
    int requiredHits = 1;
    int currentHits = 0;
    sf::Color baseColor = sf::Color::Red;
};

