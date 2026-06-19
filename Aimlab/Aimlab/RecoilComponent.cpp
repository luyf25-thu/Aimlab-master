#include "RecoilComponent.h"

#include "MathUtils.h"

#include <algorithm>

RecoilComponent::RecoilComponent(float kick, float shake, float recoverySeconds,
                                 float maxY, float jitterY, float maxX)
    : kickY(kick), shakeX(shake), recoveryTime(recoverySeconds),
      maxOffsetY(maxY), jitterRangeY(jitterY), maxOffsetX(maxX)
{
}

void RecoilComponent::applyRecoil()
{
    const float nextY = targetOffsetY + kickY;
    if (maxOffsetY > 0.0f && nextY > maxOffsetY)
    {
        const float jitter = MathUtils::getRandomFloat(-jitterRangeY, jitterRangeY);
        targetOffsetY = std::max(0.0f, maxOffsetY + jitter);
    }
    else
    {
        targetOffsetY = nextY;
    }

    targetOffsetX += MathUtils::getRandomFloat(-shakeX, shakeX);
    if (maxOffsetX > 0.0f)
    {
        targetOffsetX = std::clamp(targetOffsetX, -maxOffsetX, maxOffsetX);
    }

    const float instantShakeX = MathUtils::getRandomFloat(-shakeX * 0.7f, shakeX * 0.7f);
    const float instantShakeY = MathUtils::getRandomFloat(-kickY * 0.35f, kickY * 0.35f);
    punchOffsetX += instantShakeX;
    punchOffsetY += instantShakeY;
    isRecovering = false;
}

void RecoilComponent::update(float deltaTime, bool isFiring)
{
    const float punchDecay = std::min(1.0f, punchDecaySpeed * deltaTime);
    punchOffsetX += (0.0f - punchOffsetX) * punchDecay;
    punchOffsetY += (0.0f - punchOffsetY) * punchDecay;

    if (isFiring)
    {
        isRecovering = false;
        const float t = std::min(1.0f, smoothingSpeed * deltaTime);
        currentOffsetX += (targetOffsetX - currentOffsetX) * t;
        currentOffsetY += (targetOffsetY - currentOffsetY) * t;
        return;
    }

    if (!isRecovering)
    {
        recoveryElapsed = 0.0f;
        recoveryStartX = currentOffsetX;
        recoveryStartY = currentOffsetY;
        targetOffsetX = currentOffsetX;
        targetOffsetY = currentOffsetY;
        isRecovering = true;
    }

    if (recoveryTime <= 0.0f)
    {
        currentOffsetX = 0.0f;
        currentOffsetY = 0.0f;
        return;
    }

    recoveryElapsed += deltaTime;
    const float t = std::min(1.0f, recoveryElapsed / recoveryTime);
    currentOffsetX = recoveryStartX * (1.0f - t);
    currentOffsetY = recoveryStartY * (1.0f - t);
    targetOffsetX = currentOffsetX;
    targetOffsetY = currentOffsetY;
}

sf::Vector2f RecoilComponent::getOffset() const
{
    return { currentOffsetX + punchOffsetX, -(currentOffsetY + punchOffsetY) };
}
