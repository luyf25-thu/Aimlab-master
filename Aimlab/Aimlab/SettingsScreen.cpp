#include "SettingsScreen.h"

#include "UITheme.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <utility>

void SettingsScreen::init()
{
    overlayBg.setFillColor(UITheme::OverlayBg);
    panelBg.setFillColor(UITheme::PanelBg);
    panelBg.setOutlineColor(UITheme::PanelBorder);
    panelBg.setOutlineThickness(UITheme::PanelBorderWidth);

    titleText.emplace(*UITheme::DefaultFont);
    titleText->setString("Settings");
    titleText->setCharacterSize(34);
    titleText->setFillColor(UITheme::TextWhite);
    titleText->setStyle(sf::Text::Bold);

    const char* sections[] = { "Mode", "Weapon", "Ammo", "Sensitivity" };
    for (int i = 0; i < 4; ++i)
    {
        sectionLabels[i].emplace(*UITheme::DefaultFont);
        sectionLabels[i]->setString(sections[i]);
        sectionLabels[i]->setCharacterSize(18);
        sectionLabels[i]->setFillColor(UITheme::AccentBlue);
        sectionLabels[i]->setStyle(sf::Text::Bold);
    }

    const sf::Vector2f smallButtonSize{ 145.0f, 42.0f };
    const char* modeNames[] = { "Simple", "Hard", "Recoil" };
    for (int i = 0; i < 3; ++i)
    {
        modeButtons[i] = UIButton(modeNames[i], 18, smallButtonSize, { 0.0f, 0.0f });
        modeButtons[i].setCallback([this, i]() {
            setSelectedMode(i);
            if (onModeSelected) onModeSelected(i);
        });
    }

    const char* weaponNames[] = { "USP", "AK-47", "M4A1" };
    for (int i = 0; i < 3; ++i)
    {
        weaponButtons[i] = UIButton(weaponNames[i], 18, smallButtonSize, { 0.0f, 0.0f });
        weaponButtons[i].setCallback([this, i]() {
            setSelectedWeapon(i);
            if (onWeaponSelected) onWeaponSelected(i);
        });
    }

    ammoButton = UIButton("Infinite Ammo: ON", 18, { 240.0f, 42.0f }, { 0.0f, 0.0f });
    ammoButton.setCallback([this]() {
        setInfiniteAmmo(!infiniteAmmo);
        if (onInfiniteAmmoChanged) onInfiniteAmmoChanged(infiniteAmmo);
    });

    sensitivityText.emplace(*UITheme::DefaultFont);
    sensitivityText->setCharacterSize(18);
    sensitivityText->setFillColor(UITheme::TextWhite);

    sensitivityButtons[0] = UIButton("-", 22, { 48.0f, 38.0f }, { 0.0f, 0.0f });
    sensitivityButtons[0].setCallback([this]() {
        if (onSensitivityChanged) onSensitivityChanged(-0.1f);
    });
    sensitivityButtons[1] = UIButton("+", 22, { 48.0f, 38.0f }, { 0.0f, 0.0f });
    sensitivityButtons[1].setCallback([this]() {
        if (onSensitivityChanged) onSensitivityChanged(0.1f);
    });

    backButton = UIButton("Back", 20, { 170.0f, 42.0f }, { 0.0f, 0.0f });
    backButton.setCallback([this]() { if (onStateChange) onStateChange(backState); });

    updateLabels();
}

void SettingsScreen::setStateCallback(StateCallback cb)
{
    onStateChange = std::move(cb);
}

void SettingsScreen::setModeCallback(std::function<void(int)> cb)
{
    onModeSelected = std::move(cb);
}

void SettingsScreen::setWeaponCallback(std::function<void(int)> cb)
{
    onWeaponSelected = std::move(cb);
}

void SettingsScreen::setInfiniteAmmoCallback(std::function<void(bool)> cb)
{
    onInfiniteAmmoChanged = std::move(cb);
}

void SettingsScreen::setSensitivityCallback(std::function<void(float)> cb)
{
    onSensitivityChanged = std::move(cb);
}

void SettingsScreen::setSelectedMode(int index)
{
    selectedMode = std::clamp(index, 0, 2);
    updateLabels();
}

void SettingsScreen::setSelectedWeapon(int index)
{
    selectedWeapon = std::clamp(index, 0, 2);
    updateLabels();
}

void SettingsScreen::setInfiniteAmmo(bool enabled)
{
    infiniteAmmo = enabled;
    updateLabels();
}

void SettingsScreen::setSensitivity(float value)
{
    sensitivity = std::clamp(value, 0.2f, 3.0f);
    updateLabels();
}

void SettingsScreen::setBackState(GameState state)
{
    backState = state;
}

void SettingsScreen::onResize(const sf::Vector2u& windowSize)
{
    viewSize = windowSize;
    layout();
}

void SettingsScreen::handleEvent(const sf::Event& event)
{
    if (const auto* moved = event.getIf<sf::Event::MouseMoved>())
    {
        const sf::Vector2i mp = { moved->position.x, moved->position.y };
        for (auto& btn : modeButtons) btn.handleMouseMove(mp);
        for (auto& btn : weaponButtons) btn.handleMouseMove(mp);
        ammoButton.handleMouseMove(mp);
        for (auto& btn : sensitivityButtons) btn.handleMouseMove(mp);
        backButton.handleMouseMove(mp);
    }
    else if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (pressed->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2i mp = { pressed->position.x, pressed->position.y };
            for (auto& btn : modeButtons) btn.handleClick(mp);
            for (auto& btn : weaponButtons) btn.handleClick(mp);
            ammoButton.handleClick(mp);
            for (auto& btn : sensitivityButtons) btn.handleClick(mp);
            backButton.handleClick(mp);
        }
    }
    else if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Escape || key->code == sf::Keyboard::Key::Backspace)
        {
            if (onStateChange) onStateChange(backState);
        }
    }
}

void SettingsScreen::render(sf::RenderWindow& window)
{
    window.draw(overlayBg);
    window.draw(panelBg);
    if (titleText) window.draw(*titleText);
    for (const auto& label : sectionLabels) if (label) window.draw(*label);
    for (auto& btn : modeButtons) btn.render(window);
    for (auto& btn : weaponButtons) btn.render(window);
    ammoButton.render(window);
    if (sensitivityText) window.draw(*sensitivityText);
    for (auto& btn : sensitivityButtons) btn.render(window);
    backButton.render(window);
}

void SettingsScreen::updateLabels()
{
    static constexpr std::array<const char*, 3> modeNames = { "Simple", "Hard", "Recoil" };
    static constexpr std::array<const char*, 3> weaponNames = { "USP", "AK-47", "M4A1" };

    for (int i = 0; i < 3; ++i)
    {
        modeButtons[i].setLabel(std::string(i == selectedMode ? "> " : "") + modeNames[i], 18);
        weaponButtons[i].setLabel(std::string(i == selectedWeapon ? "> " : "") + weaponNames[i], 18);
    }

    ammoButton.setLabel(std::string("Infinite Ammo: ") + (infiniteAmmo ? "ON" : "OFF"), 18);

    if (sensitivityText)
    {
        std::ostringstream oss;
        oss << "Sensitivity " << std::fixed << std::setprecision(1) << sensitivity;
        sensitivityText->setString(oss.str());
    }
}

void SettingsScreen::layout()
{
    const float w = static_cast<float>(viewSize.x);
    const float h = static_cast<float>(viewSize.y);
    const float cx = w * 0.5f;

    overlayBg.setSize({ w, h });
    overlayBg.setPosition({ 0.0f, 0.0f });

    const float panelW = std::min(640.0f, w * 0.78f);
    const float panelH = std::min(560.0f, h * 0.78f);
    const float panelX = cx - panelW * 0.5f;
    const float panelY = (h - panelH) * 0.5f;
    panelBg.setSize({ panelW, panelH });
    panelBg.setPosition({ panelX, panelY });

    if (titleText)
    {
        titleText->setCharacterSize(static_cast<unsigned int>(std::max(28.0f, h * 0.046f)));
        const sf::FloatRect tb = titleText->getLocalBounds();
        titleText->setOrigin({ tb.position.x + tb.size.x * 0.5f, tb.position.y });
        titleText->setPosition({ cx, panelY + 22.0f });
    }

    const float leftX = panelX + 48.0f;
    const float buttonGap = 16.0f;
    const float smallW = (panelW - 96.0f - buttonGap * 2.0f) / 3.0f;
    const float smallH = 42.0f;
    float y = panelY + 88.0f;

    for (int section = 0; section < 4; ++section)
    {
        if (sectionLabels[section])
        {
            const auto& b = sectionLabels[section]->getLocalBounds();
            sectionLabels[section]->setOrigin({ b.position.x, b.position.y });
            sectionLabels[section]->setPosition({ leftX, y });
        }
        y += 30.0f;

        if (section == 0)
        {
            for (int i = 0; i < 3; ++i)
            {
                modeButtons[i].setSize({ smallW, smallH });
                modeButtons[i].setFontSize(18);
                modeButtons[i].setPosition({ leftX + static_cast<float>(i) * (smallW + buttonGap), y });
            }
        }
        else if (section == 1)
        {
            for (int i = 0; i < 3; ++i)
            {
                weaponButtons[i].setSize({ smallW, smallH });
                weaponButtons[i].setFontSize(18);
                weaponButtons[i].setPosition({ leftX + static_cast<float>(i) * (smallW + buttonGap), y });
            }
        }
        else if (section == 2)
        {
            ammoButton.setSize({ panelW - 96.0f, smallH });
            ammoButton.setFontSize(18);
            ammoButton.setPosition({ leftX, y });
        }
        else
        {
            sensitivityButtons[0].setSize({ 54.0f, smallH });
            sensitivityButtons[1].setSize({ 54.0f, smallH });
            sensitivityButtons[0].setPosition({ leftX, y });
            sensitivityButtons[1].setPosition({ panelX + panelW - 48.0f - 54.0f, y });

            if (sensitivityText)
            {
                sensitivityText->setCharacterSize(19);
                const auto& sb = sensitivityText->getLocalBounds();
                sensitivityText->setOrigin({ sb.position.x + sb.size.x * 0.5f, sb.position.y + sb.size.y * 0.5f });
                sensitivityText->setPosition({ cx, y + smallH * 0.5f });
            }
        }
        y += 74.0f;
    }

    backButton.setSize({ 170.0f, 42.0f });
    backButton.setFontSize(20);
    backButton.setPosition({ cx - 85.0f, panelY + panelH - 60.0f });
}
