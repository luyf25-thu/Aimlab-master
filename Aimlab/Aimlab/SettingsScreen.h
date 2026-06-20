#pragma once

#include "GameState.h"
#include "UIComponents.h"

#include <SFML/Graphics.hpp>

#include <array>
#include <functional>
#include <optional>

class SettingsScreen
{
public:
    using StateCallback = std::function<void(GameState)>;

    SettingsScreen() = default;

    void init();
    void setStateCallback(StateCallback cb);
    void setModeCallback(std::function<void(int)> cb);
    void setWeaponCallback(std::function<void(int)> cb);
    void setInfiniteAmmoCallback(std::function<void(bool)> cb);
    void setSensitivityCallback(std::function<void(float)> cb);
    void setSelectedMode(int index);
    void setSelectedWeapon(int index);
    void setInfiniteAmmo(bool enabled);
    void setSensitivity(float value);
    void setBackState(GameState state);
    void onResize(const sf::Vector2u& windowSize);
    void handleEvent(const sf::Event& event);
    void render(sf::RenderWindow& window);

private:
    void updateLabels();
    void layout();

    sf::RectangleShape overlayBg;
    sf::RectangleShape panelBg;
    std::optional<sf::Text> titleText;
    std::array<std::optional<sf::Text>, 4> sectionLabels;
    std::array<UIButton, 3> modeButtons;
    std::array<UIButton, 3> weaponButtons;
    UIButton ammoButton;
    std::optional<sf::Text> sensitivityText;
    std::array<UIButton, 2> sensitivityButtons;
    UIButton backButton;

    sf::Vector2u viewSize{ 800, 600 };
    int selectedMode = 0;
    int selectedWeapon = 0;
    bool infiniteAmmo = true;
    float sensitivity = 1.0f;
    GameState backState = GameState::MainMenu;

    StateCallback onStateChange;
    std::function<void(int)> onModeSelected;
    std::function<void(int)> onWeaponSelected;
    std::function<void(bool)> onInfiniteAmmoChanged;
    std::function<void(float)> onSensitivityChanged;
};
