#pragma once

#include "GameHUD.h"
#include "GameState.h"
#include "HelpScreen.h"
#include "MainMenu.h"
#include "PauseAndResult.h"
#include "SettingsScreen.h"

#include <SFML/Graphics.hpp>

#include <functional>
#include <optional>

class UIManager
{
public:
    UIManager() = default;

    bool init(const sf::Texture& backgroundTexture, const sf::Vector2u& windowSize);
    GameState getState() const;
    bool isPendingStateChange() const;
    GameState consumePendingState();
    void setState(GameState state);
    void onResize(const sf::Vector2u& windowSize);
    void handleEvent(const sf::Event& event);
    void render(sf::RenderWindow& window);
    void updateResultStats(int hits, int misses, int totalShots,
                           float accuracy, float avgTime, float totalTime);
    void setPauseWeapon(int idx);
    void setPauseRestartCallback(std::function<void()> cb);
    void setPauseWeaponCallback(std::function<void(int)> cb);
    void setResultRestartCallback(std::function<void()> cb);
    void setSettingsMode(int index);
    void setSettingsWeapon(int index);
    void setSettingsInfiniteAmmo(bool enabled);
    void setSettingsSensitivity(float value);
    void setSettingsBackState(GameState state);
    void setSettingsModeCallback(std::function<void(int)> cb);
    void setSettingsWeaponCallback(std::function<void(int)> cb);
    void setSettingsInfiniteAmmoCallback(std::function<void(bool)> cb);
    void setSettingsSensitivityCallback(std::function<void(float)> cb);

    GameHUD hud;

private:
    void requestStateChange(GameState state);
    void applyState(GameState state);

    GameState currentState = GameState::MainMenu;
    std::optional<GameState> pendingState;

    MainMenu mainMenu;
    PauseOverlay pauseOverlay;
    ResultScreen resultScreen;
    HelpScreen helpScreen;
    SettingsScreen settingsScreen;
};
