#pragma once

#include "GameState.h"
#include "UIComponents.h"

#include <SFML/Graphics.hpp>

#include <array>
#include <functional>
#include <optional>

class PauseOverlay
{
public:
    using StateCallback = std::function<void(GameState)>;

    PauseOverlay() = default;

    void init();
    void setActiveWeapon(int idx);
    void setSensitivity(float value);
    void setStateCallback(StateCallback cb);
    void setRestartCallback(std::function<void()> cb);
    void setWeaponCallback(std::function<void(int)> cb);
    void setSensitivityCallback(std::function<void(float)> cb);
    void onResize(const sf::Vector2u& windowSize);
    void handleEvent(const sf::Event& event);
    void render(sf::RenderWindow& window);

private:
    void layout();

    sf::RectangleShape overlayBg;
    std::optional<sf::Text> pauseTitle;
    std::array<sf::RectangleShape, 3> weaponBtns;
    std::array<std::optional<sf::Text>, 3> weaponLabels;
    std::optional<sf::Text> sensitivityText;
    std::array<UIButton, 2> sensitivityButtons;
    std::array<UIButton, 3> buttons;
    std::optional<sf::Text> hintText;
    sf::Vector2u viewSize{ 800, 600 };
    int activeWeaponIndex = 0;
    float sensitivity = 1.0f;
    StateCallback onStateChange;
    std::function<void()> onRestartRequested;
    std::function<void(int)> onWeaponSelected;
    std::function<void(float)> onSensitivityChanged;
};

class ResultScreen
{
public:
    using StateCallback = std::function<void(GameState)>;

    ResultScreen() = default;

    void init();
    void setStateCallback(StateCallback cb);
    void setRestartCallback(std::function<void()> cb);
    void onResize(const sf::Vector2u& windowSize);
    void setStats(int h, int m, int t, float a, float at, float tt);
    void handleEvent(const sf::Event& event);
    void render(sf::RenderWindow& window);

private:
    void updateStatTexts();
    void layout();

    sf::RectangleShape overlayBg, panelBg;
    std::optional<sf::Text> titleText;
    sf::RectangleShape separator;
    std::optional<sf::Text> ratingText;
    struct StatRow { std::optional<sf::Text> label; std::optional<sf::Text> value; };
    std::array<StatRow, 6> statRows;
    std::array<UIButton, 2> buttons;
    int statHits = 0, statMisses = 0, statTotalShots = 0;
    float statAccuracy = 0.0f, statAvgTime = 0.0f, statTotalTime = 0.0f;
    sf::Vector2u viewSize{ 800, 600 };
    StateCallback onStateChange;
    std::function<void()> onRestartRequested;
};
