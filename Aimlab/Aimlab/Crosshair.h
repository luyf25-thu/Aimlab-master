#pragma once

#include <SFML/Graphics.hpp>
#include <array>

class Crosshair
{
public:
    Crosshair()
    {
        const sf::Color mainColor(0, 255, 80, 235);
        const sf::Color outlineColor(0, 0, 0, 190);

        for (auto& segment : outlineSegments)
        {
            segment.setFillColor(outlineColor);
        }

        for (auto& segment : segments)
        {
            segment.setFillColor(mainColor);
        }

        outlineSegments[0].setSize({ lineThickness + outlinePadding * 2.0f, lineLength + outlinePadding * 2.0f });
        outlineSegments[1].setSize({ lineThickness + outlinePadding * 2.0f, lineLength + outlinePadding * 2.0f });
        outlineSegments[2].setSize({ lineLength + outlinePadding * 2.0f, lineThickness + outlinePadding * 2.0f });
        outlineSegments[3].setSize({ lineLength + outlinePadding * 2.0f, lineThickness + outlinePadding * 2.0f });

        segments[0].setSize({ lineThickness, lineLength });
        segments[1].setSize({ lineThickness, lineLength });
        segments[2].setSize({ lineLength, lineThickness });
        segments[3].setSize({ lineLength, lineThickness });
    }

    void updatePosition(const sf::Vector2f& basePos)
    {
        const float cx = basePos.x;
        const float cy = basePos.y;
        const float halfThickness = lineThickness * 0.5f;

        segments[0].setPosition({ cx - halfThickness, cy - centerGap - lineLength });
        segments[1].setPosition({ cx - halfThickness, cy + centerGap });
        segments[2].setPosition({ cx - centerGap - lineLength, cy - halfThickness });
        segments[3].setPosition({ cx + centerGap, cy - halfThickness });

        for (std::size_t i = 0; i < segments.size(); ++i)
        {
            const sf::Vector2f pos = segments[i].getPosition();
            outlineSegments[i].setPosition({ pos.x - outlinePadding, pos.y - outlinePadding });
        }
    }

    void render(sf::RenderWindow& window)
    {
        for (const auto& segment : outlineSegments)
        {
            window.draw(segment);
        }
        for (const auto& segment : segments)
        {
            window.draw(segment);
        }
    }

private:
    static constexpr float lineLength = 16.0f;
    static constexpr float lineThickness = 3.0f;
    static constexpr float centerGap = 7.0f;
    static constexpr float outlinePadding = 1.0f;

    std::array<sf::RectangleShape, 4> outlineSegments;
    std::array<sf::RectangleShape, 4> segments;
};
