#pragma once

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

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
        if (!isHitState)
        {
            window.draw(shadowShape);
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
    void applyVisualColor(const sf::Color& color)
    {
        buildSphereTexture(color);
        buildShadowTexture();
        shape.setTexture(&sphereTexture, true);
        shape.setFillColor(sf::Color::White);

        const float shadowRadius = radius * 1.24f;
        shadowShape.setRadius(shadowRadius);
        shadowShape.setOrigin({ shadowRadius, shadowRadius });
        shadowShape.setPosition({ position.x + radius * 0.22f, position.y + radius * 0.24f });
        shadowShape.setTexture(&shadowTexture, true);
        shadowShape.setFillColor(sf::Color::White);

    }

    void buildSphereTexture(const sf::Color& color)
    {
        static constexpr unsigned int textureSize = 128;
        sf::Image image({ textureSize, textureSize }, sf::Color::Transparent);
        const float center = (static_cast<float>(textureSize) - 1.0f) * 0.5f;
        const float maxRadius = center - 1.0f;
        const sf::Vector2f lightDir{ -0.45f, -0.62f };
        const sf::Vector2f highlightCenter{
            center + lightDir.x * maxRadius * 0.34f,
            center + lightDir.y * maxRadius * 0.34f
        };

        const auto toByte = [](float value)
        {
            return static_cast<std::uint8_t>(std::clamp(value, 0.0f, 255.0f));
        };

        for (unsigned int y = 0; y < textureSize; ++y)
        {
            for (unsigned int x = 0; x < textureSize; ++x)
            {
                const float dx = (static_cast<float>(x) - center) / maxRadius;
                const float dy = (static_cast<float>(y) - center) / maxRadius;
                const float distSq = dx * dx + dy * dy;
                if (distSq > 1.0f)
                {
                    image.setPixel({ x, y }, sf::Color::Transparent);
                    continue;
                }

                const float z = std::sqrt(std::max(0.0f, 1.0f - distSq));
                const float light = std::clamp(-dx * lightDir.x - dy * lightDir.y + z * 0.82f, 0.0f, 1.0f);
                const float rim = std::sqrt(distSq);
                const float edgeShade = 1.0f - rim * 0.38f;

                const float hdx = (static_cast<float>(x) - highlightCenter.x) / maxRadius;
                const float hdy = (static_cast<float>(y) - highlightCenter.y) / maxRadius;
                const float highlight = std::max(0.0f, 1.0f - std::sqrt(hdx * hdx + hdy * hdy) * 4.6f);

                const float shadow = 0.72f + light * 0.42f;
                const float brightness = std::clamp(edgeShade * shadow + highlight * 0.45f, 0.32f, 1.45f);
                const float alpha = 255.0f * std::clamp((1.0f - rim) * 32.0f, 0.0f, 1.0f);

                const float r = color.r * brightness + 255.0f * highlight * 0.38f;
                const float g = color.g * brightness + 255.0f * highlight * 0.38f;
                const float b = color.b * brightness + 255.0f * highlight * 0.38f;
                image.setPixel({ x, y }, sf::Color(toByte(r), toByte(g), toByte(b), toByte(alpha)));
            }
        }

        if (sphereTexture.loadFromImage(image))
        {
            sphereTexture.setSmooth(false);
        }
    }

    void buildShadowTexture()
    {
        if (shadowReady)
        {
            return;
        }

        static constexpr unsigned int textureSize = 96;
        sf::Image image({ textureSize, textureSize }, sf::Color::Transparent);
        const float center = (static_cast<float>(textureSize) - 1.0f) * 0.5f;
        const float maxRadius = center - 1.0f;
        const auto toByte = [](float value)
        {
            return static_cast<std::uint8_t>(std::clamp(value, 0.0f, 255.0f));
        };

        for (unsigned int y = 0; y < textureSize; ++y)
        {
            for (unsigned int x = 0; x < textureSize; ++x)
            {
                const float dx = (static_cast<float>(x) - center) / maxRadius;
                const float dy = (static_cast<float>(y) - center) / maxRadius;
                const float dist = std::sqrt(dx * dx + dy * dy);
                if (dist > 1.0f)
                {
                    continue;
                }

                const float alpha = 96.0f * std::pow(std::max(0.0f, 1.0f - dist), 1.65f);
                image.setPixel({ x, y }, sf::Color(0, 0, 0, toByte(alpha)));
            }
        }

        if (shadowTexture.loadFromImage(image))
        {
            shadowTexture.setSmooth(true);
            shadowReady = true;
        }
    }

    sf::CircleShape shape;
    sf::CircleShape shadowShape;
    sf::Texture sphereTexture;
    sf::Texture shadowTexture;
    bool shadowReady = false;
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
