#include "Game.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <windows.h>

#include "ResourceManager.h"
#include "UITheme.h"

namespace
{
constexpr float BackgroundWorldScale = 1.45f;
constexpr float MinimumWorldScale = 1.45f;
constexpr unsigned int InitialWindowWidth = 1024;
constexpr unsigned int InitialWindowHeight = 768;
constexpr float BaseWindowHeight = 768.0f;
}

Game::Game()
    : window(sf::VideoMode({ InitialWindowWidth, InitialWindowHeight }), "Aimlab OOP Demo", sf::Style::Titlebar | sf::Style::Close),
      targetPool(60),
      spawner(&targetPool, &simpleMode, 0.0f)
{
    window.setFramerateLimit(144);
    uiView = sf::View(sf::FloatRect(
        sf::Vector2f{ 0.f, 0.f },
        sf::Vector2f{ static_cast<float>(InitialWindowWidth), static_cast<float>(InitialWindowHeight) }));

    loadResources();
    initializeUi();

    infiniteAmmoEnabled = true;
    selectWeapon(0);
    selectMode(0);
    updateWorldScale();

    gameState = GameState::MainMenu;
    window.setMouseCursorVisible(true);
}

void Game::run()
{
    sf::Clock clock;
    while (window.isOpen())
    {
        if (uiManager.isPendingStateChange())
        {
            handleStateTransition(uiManager.consumePendingState());
        }

        processEvents();
        const float deltaTime = clock.restart().asSeconds();

        if (gameState == GameState::Playing)
        {
            update(deltaTime);
        }

        render();

        if (gameState == GameState::Exit)
        {
            window.close();
        }
    }
}

void Game::loadResources()
{
    auto& resources = ResourceManager::getInstance();

    const std::filesystem::path backgroundPath = getExecutableDir() / "background.jpg";
    try
    {
        resources.loadTexture("background", backgroundPath);
        backgroundTexture = &resources.getTexture("background");
        backgroundSprite.emplace(*backgroundTexture);
        bgSize = backgroundTexture->getSize();
        view = sf::View(sf::FloatRect(
            sf::Vector2f{ (static_cast<float>(bgSize.x) - static_cast<float>(InitialWindowWidth)) * 0.5f,
                          (static_cast<float>(bgSize.y) - static_cast<float>(InitialWindowHeight)) * 0.5f },
            sf::Vector2f{ static_cast<float>(InitialWindowWidth), static_cast<float>(InitialWindowHeight) }));
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to load background: " << e.what() << '\n';
        view = sf::View(sf::FloatRect(
            sf::Vector2f{ 0.f, 0.f },
            sf::Vector2f{ static_cast<float>(InitialWindowWidth), static_cast<float>(InitialWindowHeight) }));
    }
    window.setView(view);

    const std::filesystem::path fontPath = getExecutableDir() / "font.ttf";
    try
    {
        resources.loadFont("ui", fontPath);
        uiFont = &resources.getFont("ui");
        UITheme::DefaultFont = uiFont;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to load UI font: " << e.what() << '\n';
        UITheme::DefaultFont = nullptr;
    }

    try
    {
        const std::filesystem::path exeDir = getExecutableDir();
        resources.loadSoundBuffer("usp_fire", exeDir / "usp_fire.wav");
        resources.loadSoundBuffer("ak47_fire", exeDir / "ak47_fire.wav");
        resources.loadSoundBuffer("m4a1_fire", exeDir / "m4a1_fire.wav");
        resources.loadSoundBuffer("reload", exeDir / "reload.wav");
        resources.loadSoundBuffer("ui_click", exeDir / "ui_click.wav");

        usp.setFireSoundBuffer(resources.getSoundBuffer("usp_fire"));
        ak47.setFireSoundBuffer(resources.getSoundBuffer("ak47_fire"));
        m4a1.setFireSoundBuffer(resources.getSoundBuffer("m4a1_fire"));
        usp.setReloadSoundBuffer(resources.getSoundBuffer("reload"));
        ak47.setReloadSoundBuffer(resources.getSoundBuffer("reload"));
        m4a1.setReloadSoundBuffer(resources.getSoundBuffer("reload"));

        usp.setFireVolume(65.0f);
        ak47.setFireVolume(72.0f);
        m4a1.setFireVolume(68.0f);
        usp.setReloadVolume(80.0f);
        ak47.setReloadVolume(80.0f);
        m4a1.setReloadVolume(80.0f);
        initUiClickSound(resources.getSoundBuffer("ui_click"));
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to load one or more sound assets: " << e.what() << '\n';
    }
}

void Game::initializeUi()
{
    UIButton::setClickSoundCallback([this]() {
        playUiClickSound();
    });

    if (!backgroundTexture || !uiManager.init(*backgroundTexture, window.getSize()))
    {
        std::cerr << "Failed to initialize UI. Check background and font assets." << '\n';
        return;
    }

    uiManager.setPauseRestartCallback([this]() {
        restartGame();
    });
    uiManager.setResultRestartCallback([this]() {
        restartGame();
    });
    uiManager.setPauseWeaponCallback([this](int index) {
        selectWeapon(index);
    });
    uiManager.setSettingsModeCallback([this](int index) {
        setPreferredMode(index);
    });
    uiManager.setSettingsWeaponCallback([this](int index) {
        setPreferredWeapon(index);
    });
    uiManager.setSettingsInfiniteAmmoCallback([this](bool enabled) {
        setPreferredInfiniteAmmo(enabled);
    });
    uiManager.setSettingsSensitivityCallback([this](float delta) {
        adjustMouseSensitivity(delta);
    });
    syncSettingsUi();
}

void Game::initUiClickSound(const sf::SoundBuffer& buffer)
{
    uiClickSounds.clear();
    uiClickSounds.reserve(8);
    for (std::size_t i = 0; i < 8; ++i)
    {
        uiClickSounds.emplace_back(buffer);
        uiClickSounds.back().setVolume(55.0f);
    }
    nextUiClickSound = 0;
}

void Game::playUiClickSound()
{
    if (uiClickSounds.empty())
    {
        return;
    }

    sf::Sound& sound = uiClickSounds[nextUiClickSound];
    sound.stop();
    sound.play();
    nextUiClickSound = (nextUiClickSound + 1) % uiClickSounds.size();
}

void Game::syncSettingsUi()
{
    uiManager.setSettingsMode(preferredModeIndex);
    uiManager.setSettingsWeapon(preferredWeaponIndex);
    uiManager.setSettingsInfiniteAmmo(preferredInfiniteAmmo);
    uiManager.setSettingsSensitivity(mouseSensitivity);
}

void Game::handleStateTransition(GameState newState)
{
    if (newState == GameState::Exit)
    {
        gameState = GameState::Exit;
        return;
    }

    if (newState == GameState::Playing && gameState != GameState::Playing)
    {
        if (gameState == GameState::MainMenu || gameState == GameState::Result)
        {
            restartGame();
        }
        window.setMouseCursorVisible(false);
        sf::Mouse::setPosition(getWindowCenter(), window);
    }
    else if (newState == GameState::MainMenu ||
             newState == GameState::Paused ||
             newState == GameState::Help ||
             newState == GameState::Settings)
    {
        window.setMouseCursorVisible(true);
        if (newState == GameState::Paused)
        {
            uiManager.setPauseWeapon(activeWeaponIndex);
        }
        if (newState == GameState::Settings)
        {
            uiManager.setSettingsBackState(gameState == GameState::Paused ? GameState::Paused : GameState::MainMenu);
            syncSettingsUi();
        }
    }
    else if (newState == GameState::Result)
    {
        window.setMouseCursorVisible(true);
        const float accuracy = scoreManager.getAccuracy();
        const float avgTime = scoreManager.getHits() > 0
            ? elapsedTime / static_cast<float>(scoreManager.getHits())
            : 0.0f;
        uiManager.updateResultStats(
            scoreManager.getHits(),
            scoreManager.getMisses(),
            scoreManager.getHits() + scoreManager.getMisses(),
            accuracy,
            avgTime,
            elapsedTime);
    }

    gameState = newState;
    uiManager.setState(newState);
}

void Game::restartGame()
{
    targetPool.deactivateAll();
    scoreManager.reset();
    elapsedTime = 0.0f;
    infiniteAmmoEnabled = preferredInfiniteAmmo;
    selectWeapon(preferredWeaponIndex);
    selectMode(preferredModeIndex);
    remainingTime = roundTimeLimit;

    const sf::Vector2u ws = window.getSize();
    view.setSize(sf::Vector2f{ static_cast<float>(ws.x), static_cast<float>(ws.y) });
    const sf::Vector2u area = getSpawnAreaSize();
    view.setCenter(clampViewCenter({
        static_cast<float>(area.x) * 0.5f,
        static_cast<float>(area.y) * 0.5f
    }));
    window.setView(view);
}

void Game::processEvents()
{
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            return;
        }

        if (gameState != GameState::Playing)
        {
            uiManager.handleEvent(*event);
            handleNonPlayingHotkeys(*event);
            continue;
        }

        handlePlayingEvent(*event);
    }
}

void Game::handleNonPlayingHotkeys(const sf::Event& event)
{
    const auto* key = event.getIf<sf::Event::KeyPressed>();
    if (!key || key->code != sf::Keyboard::Key::Escape)
    {
        return;
    }

    if (gameState == GameState::MainMenu)
    {
        playUiClickSound();
        gameState = GameState::Exit;
    }
    else if (gameState == GameState::Paused)
    {
        playUiClickSound();
        handleStateTransition(GameState::Playing);
    }
    else if (gameState == GameState::Help)
    {
        playUiClickSound();
        handleStateTransition(GameState::MainMenu);
    }
}

void Game::handlePlayingEvent(const sf::Event& event)
{
    if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape ||
            key->code == sf::Keyboard::Key::P ||
            key->code == sf::Keyboard::Key::Space)
        {
            playUiClickSound();
            handleStateTransition(GameState::Paused);
        }
        else if (key->code == sf::Keyboard::Key::M)
        {
            playUiClickSound();
            toggleMode();
        }
        else if (key->code == sf::Keyboard::Key::X)
        {
            playUiClickSound();
            infiniteAmmoEnabled = !infiniteAmmoEnabled;
            activeWeapon->setInfiniteAmmo(infiniteAmmoEnabled);
        }
        else if (key->code == sf::Keyboard::Key::R)
        {
            activeWeapon->reload();
        }
        else if (key->code == sf::Keyboard::Key::LBracket)
        {
            playUiClickSound();
            adjustMouseSensitivity(-0.1f);
        }
        else if (key->code == sf::Keyboard::Key::RBracket)
        {
            playUiClickSound();
            adjustMouseSensitivity(0.1f);
        }
        else
        {
            handleWeaponSwitch(key->code);
        }
        return;
    }

    if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (pressed->button == sf::Mouse::Button::Left)
        {
            if (!trySelectWeaponSlot(pressed->position))
            {
                isFiring = true;
                tryFireOnce();
            }
        }
        return;
    }

    if (const auto* released = event.getIf<sf::Event::MouseButtonReleased>())
    {
        if (released->button == sf::Mouse::Button::Left)
        {
            isFiring = false;
        }
        return;
    }

    if (event.is<sf::Event::MouseMoved>())
    {
        handleMouseLook();
    }
}

bool Game::trySelectWeaponSlot(const sf::Vector2i& mousePos)
{
    const int winW = static_cast<int>(window.getSize().x);
    const int winH = static_cast<int>(window.getSize().y);
    const int slotH = static_cast<int>(winH * 0.09f);
    const int slotW = static_cast<int>(winW * 0.08f);
    const int gap = static_cast<int>(winW * 0.008f);
    const int slotY = winH - static_cast<int>(winH * 0.11f);

    for (int i = 0; i < weaponCount; ++i)
    {
        const int slotX = static_cast<int>(winW * 0.02f) + i * (slotW + gap);
        if (mousePos.x >= slotX && mousePos.x <= slotX + slotW &&
            mousePos.y >= slotY && mousePos.y <= slotY + slotH)
        {
            playUiClickSound();
            selectWeapon(i);
            return true;
        }
    }
    return false;
}

void Game::selectWeapon(int index)
{
    const auto weapons = getWeapons();
    activeWeaponIndex = std::clamp(index, 0, weaponCount - 1);
    activeWeapon = weapons[activeWeaponIndex];
    activeWeapon->setInfiniteAmmo(infiniteAmmoEnabled);
    uiManager.setPauseWeapon(activeWeaponIndex);
}

void Game::setPreferredWeapon(int index)
{
    preferredWeaponIndex = std::clamp(index, 0, weaponCount - 1);
    uiManager.setSettingsWeapon(preferredWeaponIndex);
    if (gameState != GameState::Playing)
    {
        selectWeapon(preferredWeaponIndex);
    }
}

std::array<Weapon*, 3> Game::getWeapons()
{
    return { &usp, &ak47, &m4a1 };
}

void Game::handleWeaponSwitch(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Key::Num1)
    {
        playUiClickSound();
        selectWeapon(0);
    }
    else if (key == sf::Keyboard::Key::Num2)
    {
        playUiClickSound();
        selectWeapon(1);
    }
    else if (key == sf::Keyboard::Key::Num3)
    {
        playUiClickSound();
        selectWeapon(2);
    }
}

void Game::selectMode(int index)
{
    modeIndex = std::clamp(index, 0, modeCount - 1);
    const auto modes = getModes();
    currentMode = modes[modeIndex];
    spawner.setMode(currentMode);
    if (currentMode)
    {
        currentMode->reset();
    }
}

void Game::setPreferredMode(int index)
{
    preferredModeIndex = std::clamp(index, 0, modeCount - 1);
    uiManager.setSettingsMode(preferredModeIndex);
    if (gameState != GameState::Playing)
    {
        selectMode(preferredModeIndex);
    }
}

std::array<GameMode*, 3> Game::getModes()
{
    return { &simpleMode, &hardMode, &recoilMode };
}

void Game::toggleMode()
{
    selectMode((modeIndex + 1) % modeCount);
    targetPool.deactivateAll();
    scoreManager.reset();
    elapsedTime = 0.0f;
}

void Game::update(float deltaTime)
{
    elapsedTime += deltaTime;
    remainingTime -= deltaTime;

    if (remainingTime <= 0.0f)
    {
        remainingTime = 0.0f;
        handleStateTransition(GameState::Result);
        return;
    }

    activeWeapon->update(deltaTime, isFiring);
    spawner.update(deltaTime, getSpawnAreaSize());

    for (const auto& target : targetPool.getTargets())
    {
        target->update(deltaTime);
    }

    if (isFiring && activeWeapon->isAutomatic())
    {
        tryFireOnce();
    }

    crosshair.updatePosition(getWindowCenterFloat());
    updateHUD();
}

void Game::updateHUD()
{
    const auto weapons = getWeapons();
    std::array<GameHUD::WeaponInfo, 3> allWeapons;
    for (int i = 0; i < weaponCount; ++i)
    {
        allWeapons[i] = getWeaponInfo(weapons[i], i, activeWeaponIndex == i);
    }

    GameHUD::StatsInfo stats;
    stats.hits = scoreManager.getHits();
    stats.misses = scoreManager.getMisses();
    stats.totalShots = stats.hits + stats.misses;
    stats.accuracy = scoreManager.getAccuracy();
    stats.avgSecondsPerHit = stats.hits > 0
        ? elapsedTime / static_cast<float>(stats.hits)
        : 0.0f;
    stats.mouseSensitivity = mouseSensitivity;

    GameHUD::ModeInfo modeInfo;
    static constexpr std::array<const char*, 3> modeNames = { "Simple", "Hard", "Recoil" };
    modeInfo.name = modeNames[modeIndex];
    modeInfo.index = modeIndex;
    modeInfo.totalModes = modeCount;

    uiManager.hud.update(
        allWeapons[activeWeaponIndex],
        allWeapons,
        stats,
        modeInfo,
        remainingTime,
        window.getSize());
}

GameHUD::WeaponInfo Game::getWeaponInfo(Weapon* weapon, int index, bool active) const
{
    static constexpr std::array<const char*, 3> shortNames = { "USP", "AK-47", "M4A1" };

    const int safeIndex = std::clamp(index, 0, weaponCount - 1);
    GameHUD::WeaponInfo info;
    info.shortName = shortNames[safeIndex];
    info.keyIndex = safeIndex + 1;
    info.isActive = active;
    info.currentAmmo = weapon->getCurrentAmmo();
    info.ammoCapacity = weapon->getAmmoCapacity();
    info.isReloading = weapon->getIsReloading();
    info.isInfiniteAmmo = weapon->getInfiniteAmmo();
    info.fireType = weapon->isAutomatic() ? "Auto" : "Semi";
    return info;
}

sf::Vector2u Game::getSpawnAreaSize() const
{
    const unsigned int viewW = window.getSize().x;
    const unsigned int viewH = window.getSize().y;
    const unsigned int scaledBgW = static_cast<unsigned int>(static_cast<float>(bgSize.x) * BackgroundWorldScale);
    const unsigned int scaledBgH = static_cast<unsigned int>(static_cast<float>(bgSize.y) * BackgroundWorldScale);
    const unsigned int minWorldW = static_cast<unsigned int>(static_cast<float>(viewW) * MinimumWorldScale);
    const unsigned int minWorldH = static_cast<unsigned int>(static_cast<float>(viewH) * MinimumWorldScale);
    return {
        std::max(scaledBgW, minWorldW),
        std::max(scaledBgH, minWorldH)
    };
}

float Game::getResolutionScale() const
{
    return static_cast<float>(window.getSize().y) / BaseWindowHeight;
}

void Game::updateWorldScale()
{
    const float resScale = getResolutionScale();
    spawner.setSpacingScale(resScale);

    if (!backgroundSprite || !backgroundTexture || bgSize.x == 0 || bgSize.y == 0)
    {
        return;
    }

    const sf::Vector2u area = getSpawnAreaSize();
    const sf::Vector2u texSize = backgroundTexture->getSize();
    const float scaleX = static_cast<float>(area.x) / static_cast<float>(texSize.x);
    const float scaleY = static_cast<float>(area.y) / static_cast<float>(texSize.y);
    const float scale = std::max(scaleX, scaleY);
    backgroundSprite->setScale(sf::Vector2f{ scale, scale });

    const float scaledW = static_cast<float>(texSize.x) * scale;
    const float scaledH = static_cast<float>(texSize.y) * scale;
    backgroundSprite->setPosition(sf::Vector2f{
        (static_cast<float>(area.x) - scaledW) * 0.5f,
        (static_cast<float>(area.y) - scaledH) * 0.5f
    });
}

void Game::setPreferredInfiniteAmmo(bool enabled)
{
    preferredInfiniteAmmo = enabled;
    infiniteAmmoEnabled = enabled;
    if (activeWeapon)
    {
        activeWeapon->setInfiniteAmmo(infiniteAmmoEnabled);
    }
    uiManager.setSettingsInfiniteAmmo(preferredInfiniteAmmo);
}

void Game::adjustMouseSensitivity(float delta)
{
    mouseSensitivity = std::clamp(mouseSensitivity + delta, 0.2f, 3.0f);
    uiManager.setSettingsSensitivity(mouseSensitivity);
}

void Game::tryFireOnce()
{
    if (!activeWeapon->fire())
    {
        return;
    }

    const sf::View recoilView = getRecoilView();
    const sf::Vector2f shotPosition = window.mapPixelToCoords(getWindowCenter(), recoilView);
    bool hit = false;

    for (const auto& target : targetPool.getTargets())
    {
        if (target->getIsActive() && target->isHit(shotPosition))
        {
            const bool destroyed = target->onHit();
            if (destroyed && currentMode)
            {
                currentMode->onTargetHit(target->getPosition());
            }
            hit = true;
            break;
        }
    }

    if (hit)
    {
        scoreManager.recordHit();
    }
    else
    {
        scoreManager.recordMiss();
    }
}

void Game::render()
{
    if (gameState == GameState::MainMenu ||
        gameState == GameState::Help ||
        gameState == GameState::Settings)
    {
        window.clear(sf::Color(20, 20, 20));
        window.setView(uiView);
        uiManager.render(window);
        window.display();
        return;
    }

    window.clear(sf::Color(20, 20, 20));
    window.setView(getRecoilView());

    if (backgroundSprite)
    {
        window.draw(*backgroundSprite);
    }

    for (const auto& target : targetPool.getTargets())
    {
        target->render(window);
    }

    window.setView(uiView);
    if (gameState == GameState::Playing || gameState == GameState::Paused)
    {
        uiManager.hud.render(window);
        crosshair.render(window);
    }

    if (gameState == GameState::Paused || gameState == GameState::Result)
    {
        uiManager.render(window);
    }

    window.display();
}

sf::Vector2i Game::getWindowCenter() const
{
    const sf::Vector2u size = window.getSize();
    return { static_cast<int>(size.x / 2), static_cast<int>(size.y / 2) };
}

sf::Vector2f Game::getWindowCenterFloat() const
{
    const sf::Vector2u size = window.getSize();
    return { static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y) * 0.5f };
}

void Game::handleMouseLook()
{
    const sf::Vector2i center = getWindowCenter();
    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    const sf::Vector2i delta = mousePos - center;
    if (delta.x != 0 || delta.y != 0)
    {
        const float speed = viewMoveSpeed * getResolutionScale() * mouseSensitivity;
        const sf::Vector2f offset(
            static_cast<float>(delta.x) * speed,
            static_cast<float>(delta.y) * speed);
        view.setCenter(clampViewCenter(view.getCenter() + offset));
        window.setView(view);
    }
    sf::Mouse::setPosition(center, window);
}

sf::View Game::getRecoilView() const
{
    sf::View recoilView = view;
    if (activeWeapon)
    {
        recoilView.move(activeWeapon->getRecoilOffset());
    }
    return recoilView;
}

std::filesystem::path Game::getExecutableDir()
{
    wchar_t buffer[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (length == 0)
    {
        return std::filesystem::current_path();
    }
    return std::filesystem::path(buffer).parent_path();
}

sf::Vector2f Game::clampViewCenter(const sf::Vector2f& center) const
{
    const sf::Vector2u area = getSpawnAreaSize();
    if (area.x == 0 || area.y == 0)
    {
        return center;
    }

    const float halfW = view.getSize().x * 0.5f;
    const float halfH = view.getSize().y * 0.5f;
    const float minX = halfW;
    const float maxX = std::max(halfW, static_cast<float>(area.x) - halfW);
    const float minY = halfH;
    const float maxY = std::max(halfH, static_cast<float>(area.y) - halfH);
    return { std::clamp(center.x, minX, maxX), std::clamp(center.y, minY, maxY) };
}
