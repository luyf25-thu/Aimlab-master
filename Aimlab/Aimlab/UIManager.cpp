#include "UIManager.h"

#include "UITheme.h"

#include <utility>

bool UIManager::init(const sf::Texture& backgroundTexture, const sf::Vector2u& windowSize)
{
    if (!UITheme::DefaultFont)
    {
        return false;
    }

    mainMenu.setStateCallback([this](GameState s) { requestStateChange(s); });
    pauseOverlay.setStateCallback([this](GameState s) { requestStateChange(s); });
    resultScreen.setStateCallback([this](GameState s) { requestStateChange(s); });
    helpScreen.setStateCallback([this](GameState s) { requestStateChange(s); });
    settingsScreen.setStateCallback([this](GameState s) { requestStateChange(s); });

    hud.initTexts();
    pauseOverlay.init();
    resultScreen.init();
    helpScreen.init();
    settingsScreen.init();

    if (!mainMenu.init(backgroundTexture))
    {
        return false;
    }
    mainMenu.onResize(windowSize);

    pauseOverlay.onResize(windowSize);
    resultScreen.onResize(windowSize);
    helpScreen.onResize(windowSize);
    settingsScreen.onResize(windowSize);

    currentState = GameState::MainMenu;
    return true;
}

GameState UIManager::getState() const
{
    return currentState;
}

bool UIManager::isPendingStateChange() const
{
    return pendingState.has_value();
}

GameState UIManager::consumePendingState()
{
    const GameState s = pendingState.value_or(currentState);
    applyState(s);
    pendingState.reset();
    return s;
}

void UIManager::setState(GameState state)
{
    applyState(state);
}

void UIManager::onResize(const sf::Vector2u& windowSize)
{
    mainMenu.onResize(windowSize);
    pauseOverlay.onResize(windowSize);
    resultScreen.onResize(windowSize);
    helpScreen.onResize(windowSize);
    settingsScreen.onResize(windowSize);
}

void UIManager::handleEvent(const sf::Event& event)
{
    switch (currentState)
    {
    case GameState::MainMenu:
        mainMenu.handleEvent(event);
        break;
    case GameState::Playing:
        break;
    case GameState::Paused:
        pauseOverlay.handleEvent(event);
        break;
    case GameState::Result:
        resultScreen.handleEvent(event);
        break;
    case GameState::Help:
        helpScreen.handleEvent(event);
        break;
    case GameState::Settings:
        settingsScreen.handleEvent(event);
        break;
    case GameState::Exit:
        break;
    }
}

void UIManager::render(sf::RenderWindow& window)
{
    switch (currentState)
    {
    case GameState::MainMenu:
        mainMenu.render(window);
        break;
    case GameState::Playing:
        break;
    case GameState::Paused:
        pauseOverlay.render(window);
        break;
    case GameState::Result:
        resultScreen.render(window);
        break;
    case GameState::Help:
        helpScreen.render(window);
        break;
    case GameState::Settings:
        settingsScreen.render(window);
        break;
    case GameState::Exit:
        break;
    }
}

void UIManager::updateResultStats(int hits, int misses, int totalShots,
                                  float accuracy, float avgTime, float totalTime)
{
    resultScreen.setStats(hits, misses, totalShots, accuracy, avgTime, totalTime);
}

void UIManager::setPauseWeapon(int idx)
{
    pauseOverlay.setActiveWeapon(idx);
}

void UIManager::setPauseRestartCallback(std::function<void()> cb)
{
    pauseOverlay.setRestartCallback(std::move(cb));
}

void UIManager::setPauseWeaponCallback(std::function<void(int)> cb)
{
    pauseOverlay.setWeaponCallback(std::move(cb));
}

void UIManager::setResultRestartCallback(std::function<void()> cb)
{
    resultScreen.setRestartCallback(std::move(cb));
}

void UIManager::setSettingsMode(int index)
{
    settingsScreen.setSelectedMode(index);
}

void UIManager::setSettingsWeapon(int index)
{
    settingsScreen.setSelectedWeapon(index);
}

void UIManager::setSettingsInfiniteAmmo(bool enabled)
{
    settingsScreen.setInfiniteAmmo(enabled);
}

void UIManager::setSettingsSensitivity(float value)
{
    settingsScreen.setSensitivity(value);
}

void UIManager::setSettingsBackState(GameState state)
{
    settingsScreen.setBackState(state);
}

void UIManager::setSettingsModeCallback(std::function<void(int)> cb)
{
    settingsScreen.setModeCallback(std::move(cb));
}

void UIManager::setSettingsWeaponCallback(std::function<void(int)> cb)
{
    settingsScreen.setWeaponCallback(std::move(cb));
}

void UIManager::setSettingsInfiniteAmmoCallback(std::function<void(bool)> cb)
{
    settingsScreen.setInfiniteAmmoCallback(std::move(cb));
}

void UIManager::setSettingsSensitivityCallback(std::function<void(float)> cb)
{
    settingsScreen.setSensitivityCallback(std::move(cb));
}

void UIManager::requestStateChange(GameState state)
{
    pendingState = state;
}

void UIManager::applyState(GameState state)
{
    currentState = state;
    pendingState.reset();
}
