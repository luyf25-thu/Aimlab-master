#pragma once

#include "UIComponents.h"

#include <SFML/Graphics.hpp>

#include <array>
#include <optional>
#include <string>

class GameHUD
{
public:
    struct WeaponInfo
    {
        std::string shortName;
        std::string fireType;
        int keyIndex = 0;
        bool isActive = false;
        int currentAmmo = 0;
        int ammoCapacity = 0;
        bool isReloading = false;
        bool isInfiniteAmmo = false;
    };

    struct StatsInfo
    {
        int hits = 0;
        int misses = 0;
        int totalShots = 0;
        float accuracy = 0.0f;
        float avgSecondsPerHit = 0.0f;
        float mouseSensitivity = 1.0f;
    };

    struct ModeInfo
    {
        std::string name;
        int index = 0;
        int totalModes = 3;
    };

    GameHUD();

    void initTexts();
    void update(const WeaponInfo& active, const std::array<WeaponInfo, 3>& allWeapons,
                const StatsInfo& stats, const ModeInfo& mode, float timeLeft,
                const sf::Vector2u& winSize);
    void render(sf::RenderWindow& window);

private:
    void initPanels();
    void initWeaponSlots();
    void updateTopBar(const ModeInfo& mode, const StatsInfo& stats, float timeLeft,
                      const sf::Vector2u& winSize);
    void updateBottomBar(const WeaponInfo& active, const std::array<WeaponInfo, 3>& allWeapons,
                         const sf::Vector2u& winSize);

    sf::RectangleShape topBarBg;
    sf::RectangleShape modeIndicator;
    sf::RectangleShape bottomBarBg;
    sf::RectangleShape bottomAccentLine;
    struct WeaponSlot { sf::RectangleShape bg; };
    std::array<WeaponSlot, 3> weaponSlots;
    sf::RectangleShape ammoBarBg;
    sf::RectangleShape ammoBarFill;

    std::optional<sf::Text> modeNameText;
    std::optional<sf::Text> timerText;
    std::array<std::optional<sf::Text>, 4> topRightTexts;
    std::array<std::optional<sf::Text>, 3> weaponSlotLabels;
    std::array<std::optional<sf::Text>, 3> weaponKeyHints;
    std::optional<sf::Text> ammoText;

    bool needsTextInit = false;
};
