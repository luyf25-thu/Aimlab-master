#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <array>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <vector>

#include "Ak47.h"
#include "Crosshair.h"
#include "GameHUD.h"
#include "GameMode.h"
#include "GameState.h"
#include "HardMode.h"
#include "M4A1.h"
#include "RecoilMode.h"
#include "ScoreManager.h"
#include "SimpleMode.h"
#include "TargetSpawner.h"
#include "UIManager.h"
#include "USP.h"
#include "Weapon.h"

class Game
{
public:
    Game();

    void run();

private:
    void loadResources();
    void initializeUi();
    void initUiClickSound(const sf::SoundBuffer& buffer);
    void playUiClickSound();
    void syncSettingsUi();
    void handleStateTransition(GameState newState);
    void restartGame();
    void processEvents();
    void handleNonPlayingHotkeys(const sf::Event& event);
    void handlePlayingEvent(const sf::Event& event);
    bool trySelectWeaponSlot(const sf::Vector2i& mousePos);
    void selectWeapon(int index);
    void setPreferredWeapon(int index);
    std::array<Weapon*, 3> getWeapons();
    void handleWeaponSwitch(sf::Keyboard::Key key);
    void selectMode(int index);
    void setPreferredMode(int index);
    std::array<GameMode*, 3> getModes();
    void toggleMode();
    void update(float deltaTime);
    void updateHUD();
    GameHUD::WeaponInfo getWeaponInfo(Weapon* weapon, int index, bool active) const;
    sf::Vector2u getSpawnAreaSize() const;
    float getResolutionScale() const;
    void updateWorldScale();
    void setPreferredInfiniteAmmo(bool enabled);
    void adjustMouseSensitivity(float delta);
    void tryFireOnce();
    void render();
    sf::Vector2i getWindowCenter() const;
    sf::Vector2f getWindowCenterFloat() const;
    void handleMouseLook();
    sf::View getRecoilView() const;
    static std::filesystem::path getExecutableDir();
    sf::Vector2f clampViewCenter(const sf::Vector2f& center) const;

    static constexpr int weaponCount = 3;
    static constexpr int modeCount = 3;

    sf::RenderWindow window;

    Usp usp;
    Ak47 ak47;
    M4A1 m4a1;
    Weapon* activeWeapon = nullptr;
    int activeWeaponIndex = 0;

    TargetPool targetPool;
    SimpleMode simpleMode;
    HardMode hardMode;
    RecoilMode recoilMode;
    TargetSpawner spawner;
    GameMode* currentMode = nullptr;
    int modeIndex = 0;

    Crosshair crosshair;
    ScoreManager scoreManager;
    UIManager uiManager;
    GameState gameState = GameState::MainMenu;

    bool isFiring = false;
    bool infiniteAmmoEnabled = true;
    bool preferredInfiniteAmmo = true;
    int preferredWeaponIndex = 0;
    int preferredModeIndex = 0;
    float elapsedTime = 0.0f;
    float avgSecondsPerHit = 0.0f;
    float remainingTime = 60.0f;
    float roundTimeLimit = 60.0f;

    sf::View view;
    sf::View uiView;
    float viewMoveSpeed = 0.35f;
    float mouseSensitivity = 1.0f;

    sf::Texture* backgroundTexture = nullptr;
    std::optional<sf::Sprite> backgroundSprite;
    sf::Vector2u bgSize{ 0, 0 };
    sf::Font* uiFont = nullptr;
    std::vector<sf::Sound> uiClickSounds;
    std::size_t nextUiClickSound = 0;
};
