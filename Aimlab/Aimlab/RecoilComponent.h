#pragma once

#include <SFML/System/Vector2.hpp>

class RecoilComponent
{
public:
    RecoilComponent(float kick = 4.0f, float shake = 2.0f, float recoverySeconds = 0.5f,
                    float maxY = 0.0f, float jitterY = 0.0f, float maxX = 0.0f);

    void applyRecoil();
    void update(float deltaTime, bool isFiring);
    sf::Vector2f getOffset() const;

private:
    float currentOffsetY = 0.0f;
    float currentOffsetX = 0.0f;
    float targetOffsetY = 0.0f;
    float targetOffsetX = 0.0f;
    float kickY = 4.0f;
    float shakeX = 2.0f;
    float recoveryTime = 0.5f;
    float maxOffsetY = 0.0f;
    float jitterRangeY = 0.0f;
    float maxOffsetX = 0.0f;
    float smoothingSpeed = 14.0f;
    float punchOffsetX = 0.0f;
    float punchOffsetY = 0.0f;
    float punchDecaySpeed = 18.0f;
    bool isRecovering = false;
    float recoveryElapsed = 0.0f;
    float recoveryStartX = 0.0f;
    float recoveryStartY = 0.0f;
};
