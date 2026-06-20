#include "PauseAndResult.h"

#include "UITheme.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <utility>

void PauseOverlay::init()
{
    pauseTitle.emplace(*UITheme::DefaultFont);
    pauseTitle->setString("PAUSED");
    pauseTitle->setCharacterSize(42);
    pauseTitle->setFillColor(UITheme::TextWhite);
    pauseTitle->setStyle(sf::Text::Bold);

    hintText.emplace(*UITheme::DefaultFont);
    hintText->setString("Press ESC to resume");
    hintText->setCharacterSize(14);
    hintText->setFillColor(UITheme::TextDim);

    const sf::Vector2f btnSize{ UITheme::ButtonMinWidth, UITheme::ButtonHeight };

    buttons[0] = UIButton("Resume", 22, btnSize, { 0, 0 });
    buttons[0].setCallback([this]() { if (onStateChange) onStateChange(GameState::Playing); });

    buttons[1] = UIButton("Restart", 22, btnSize, { 0, 0 });
    buttons[1].setCallback([this]() {
        if (onRestartRequested) onRestartRequested();
        if (onStateChange) onStateChange(GameState::Playing);
    });

    buttons[2] = UIButton("Main Menu", 22, btnSize, { 0, 0 });
    buttons[2] = UIButton("Settings", 22, btnSize, { 0, 0 });
    buttons[2].setCallback([this]() { if (onStateChange) onStateChange(GameState::Settings); });

    buttons[3] = UIButton("Main Menu", 22, btnSize, { 0, 0 });
    buttons[3].setCallback([this]() { if (onStateChange) onStateChange(GameState::MainMenu); });

    const char* wnames[] = { "USP", "AK-47", "M4A1" };
    for (int i = 0; i < 3; ++i)
    {
        weaponBtns[i].setFillColor(UITheme::BtnNormal);
        weaponBtns[i].setOutlineColor(UITheme::BtnBorder);
        weaponBtns[i].setOutlineThickness(1.0f);
        weaponLabels[i].emplace(*UITheme::DefaultFont);
        weaponLabels[i]->setString(wnames[i]);
        weaponLabels[i]->setCharacterSize(18);
        weaponLabels[i]->setFillColor(UITheme::TextWhite);
    }
}

void PauseOverlay::setActiveWeapon(int idx)
{
    activeWeaponIndex = std::clamp(idx, 0, 2);
    for (int i = 0; i < 3; ++i)
    {
        weaponBtns[i].setFillColor(i == activeWeaponIndex ? UITheme::AccentBlue : UITheme::BtnNormal);
        weaponBtns[i].setOutlineColor(i == activeWeaponIndex ? sf::Color::White : UITheme::BtnBorder);
        weaponBtns[i].setOutlineThickness(i == activeWeaponIndex ? 2.0f : 1.0f);
    }
}

void PauseOverlay::setStateCallback(StateCallback cb)
{
    onStateChange = std::move(cb);
}

void PauseOverlay::setRestartCallback(std::function<void()> cb)
{
    onRestartRequested = std::move(cb);
}

void PauseOverlay::setWeaponCallback(std::function<void(int)> cb)
{
    onWeaponSelected = std::move(cb);
}

void PauseOverlay::onResize(const sf::Vector2u& windowSize)
{
    viewSize = windowSize;
    layout();
}

void PauseOverlay::handleEvent(const sf::Event& event)
{
    if (const auto* moved = event.getIf<sf::Event::MouseMoved>())
    {
        const sf::Vector2i mp = { moved->position.x, moved->position.y };
        for (auto& btn : buttons) btn.handleMouseMove(mp);
        for (int i = 0; i < 3; ++i)
        {
            if (weaponBtns[i].getGlobalBounds().contains({ static_cast<float>(mp.x), static_cast<float>(mp.y) }))
            {
                weaponBtns[i].setOutlineColor(UITheme::BtnBorderHover);
                weaponBtns[i].setOutlineThickness(2.0f);
            }
            else
            {
                weaponBtns[i].setOutlineColor(i == activeWeaponIndex ? sf::Color::White : UITheme::BtnBorder);
                weaponBtns[i].setOutlineThickness(i == activeWeaponIndex ? 2.0f : 1.0f);
            }
        }
    }
    else if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (pressed->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2i mp = { pressed->position.x, pressed->position.y };
            for (int i = 0; i < 3; ++i)
            {
                if (weaponBtns[i].getGlobalBounds().contains({ static_cast<float>(mp.x), static_cast<float>(mp.y) }))
                {
                    setActiveWeapon(i);
                    if (onWeaponSelected) onWeaponSelected(i);
                    return;
                }
            }
            for (auto& btn : buttons) btn.handleClick(mp);
        }
    }
}

void PauseOverlay::render(sf::RenderWindow& window)
{
    window.draw(overlayBg);
    if (pauseTitle) window.draw(*pauseTitle);

    for (int i = 0; i < 3; ++i)
    {
        window.draw(weaponBtns[i]);
        if (weaponLabels[i]) window.draw(*weaponLabels[i]);
    }

    for (auto& btn : buttons) btn.render(window);
    if (hintText) window.draw(*hintText);
}

void PauseOverlay::layout()
{
    const float w = static_cast<float>(viewSize.x);
    const float h = static_cast<float>(viewSize.y);
    const float cx = w * 0.5f;
    const float cy = h * 0.5f;

    overlayBg.setSize({ w, h });
    overlayBg.setPosition({ 0.0f, 0.0f });
    overlayBg.setFillColor(UITheme::OverlayBg);

    if (pauseTitle)
    {
        pauseTitle->setCharacterSize(static_cast<unsigned int>(h * 0.065f));
        const sf::FloatRect tb = pauseTitle->getLocalBounds();
        pauseTitle->setOrigin({ tb.position.x + tb.size.x * 0.5f, tb.position.y + tb.size.y * 0.5f });
        pauseTitle->setPosition({ cx, cy - h * 0.16f });
    }

    const float wbw = w * 0.09f;
    const float wbh = h * 0.06f;
    const float wby = cy - h * 0.09f;
    const float wbGap = w * 0.008f;
    for (int i = 0; i < 3; ++i)
    {
        const float wx = cx - (wbw * 3.0f + wbGap * 2.0f) * 0.5f + static_cast<float>(i) * (wbw + wbGap);
        weaponBtns[i].setSize({ wbw, wbh });
        weaponBtns[i].setPosition({ wx, wby });
        if (weaponLabels[i])
        {
            weaponLabels[i]->setCharacterSize(static_cast<unsigned int>(wbh * 0.4f));
            const auto& b = weaponLabels[i]->getLocalBounds();
            weaponLabels[i]->setOrigin({ b.position.x + b.size.x * 0.5f, b.position.y + b.size.y * 0.5f });
            weaponLabels[i]->setPosition({ wx + wbw * 0.5f, wby + wbh * 0.5f - 1.0f });
        }
    }

    const float btnW = w * 0.28f;
    const float btnH = h * 0.065f;
    const unsigned int btnFontSize = static_cast<unsigned int>(h * 0.032f);
    for (auto& btn : buttons)
    {
        btn.setSize({ btnW, btnH });
        btn.setFontSize(btnFontSize);
    }
    const float btnX = cx - btnW * 0.5f;
    buttons[0].setPosition({ btnX, cy - h * 0.015f });
    buttons[1].setPosition({ btnX, cy + h * 0.065f });
    buttons[2].setPosition({ btnX, cy + h * 0.145f });
    buttons[3].setPosition({ btnX, cy + h * 0.225f });

    if (hintText)
    {
        const sf::FloatRect hb = hintText->getLocalBounds();
        hintText->setOrigin({ hb.position.x + hb.size.x * 0.5f, hb.position.y + hb.size.y });
        hintText->setPosition({ cx, h - 20.0f });
    }
}

void ResultScreen::init()
{
    overlayBg.setFillColor(UITheme::OverlayBg);
    panelBg.setFillColor(UITheme::PanelBg);
    panelBg.setOutlineColor(UITheme::PanelBorder);
    panelBg.setOutlineThickness(UITheme::PanelBorderWidth);

    titleText.emplace(*UITheme::DefaultFont);
    titleText->setString("Training Complete");
    titleText->setCharacterSize(36);
    titleText->setFillColor(UITheme::TextWhite);
    titleText->setStyle(sf::Text::Bold);

    separator.setFillColor(UITheme::PanelBorder);

    ratingText.emplace(*UITheme::DefaultFont);
    ratingText->setCharacterSize(20);
    ratingText->setFillColor(UITheme::AccentBlue);

    const char* labels[] = { "Hits", "Misses", "Total Shots", "Accuracy", "Avg Time/Hit", "Total Time" };
    for (int i = 0; i < 6; ++i)
    {
        statRows[i].label.emplace(*UITheme::DefaultFont);
        statRows[i].label->setString(labels[i]);
        statRows[i].label->setCharacterSize(17);
        statRows[i].label->setFillColor(UITheme::TextGray);

        statRows[i].value.emplace(*UITheme::DefaultFont);
        statRows[i].value->setString("--");
        statRows[i].value->setCharacterSize(17);
        statRows[i].value->setFillColor(UITheme::TextWhite);
    }

    const sf::Vector2f btnSize{ UITheme::ButtonMinWidth, UITheme::ButtonHeight };

    buttons[0] = UIButton("Play Again", 22, btnSize, { 0, 0 });
    buttons[0].setCallback([this]() {
        if (onRestartRequested) onRestartRequested();
        if (onStateChange) onStateChange(GameState::Playing);
    });

    buttons[1] = UIButton("Main Menu", 22, btnSize, { 0, 0 });
    buttons[1].setCallback([this]() { if (onStateChange) onStateChange(GameState::MainMenu); });
}

void ResultScreen::setStateCallback(StateCallback cb)
{
    onStateChange = std::move(cb);
}

void ResultScreen::setRestartCallback(std::function<void()> cb)
{
    onRestartRequested = std::move(cb);
}

void ResultScreen::onResize(const sf::Vector2u& windowSize)
{
    viewSize = windowSize;
    layout();
}

void ResultScreen::setStats(int h, int m, int t, float a, float at, float tt)
{
    statHits = h;
    statMisses = m;
    statTotalShots = t;
    statAccuracy = a;
    statAvgTime = at;
    statTotalTime = tt;
    updateStatTexts();
}

void ResultScreen::handleEvent(const sf::Event& event)
{
    if (const auto* moved = event.getIf<sf::Event::MouseMoved>())
    {
        const sf::Vector2i mp = { moved->position.x, moved->position.y };
        for (auto& btn : buttons) btn.handleMouseMove(mp);
    }
    else if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (pressed->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2i mp = { pressed->position.x, pressed->position.y };
            for (auto& btn : buttons) btn.handleClick(mp);
        }
    }
}

void ResultScreen::render(sf::RenderWindow& window)
{
    window.draw(overlayBg);
    window.draw(panelBg);
    if (titleText) window.draw(*titleText);
    for (const auto& row : statRows)
    {
        if (row.label) window.draw(*row.label);
        if (row.value) window.draw(*row.value);
    }
    window.draw(separator);
    if (ratingText) window.draw(*ratingText);
    for (auto& btn : buttons) btn.render(window);
}

void ResultScreen::updateStatTexts()
{
    auto sv = [](std::optional<sf::Text>& t, const std::string& s, const sf::Color& c = UITheme::TextWhite)
    {
        if (t)
        {
            t->setString(s);
            t->setFillColor(c);
        }
    };

    sv(statRows[0].value, std::to_string(statHits));
    sv(statRows[1].value, std::to_string(statMisses));
    sv(statRows[2].value, std::to_string(statTotalShots));
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << statAccuracy << "%";
        sf::Color c = statAccuracy >= 70.0f ? UITheme::AccentGreen
                    : statAccuracy >= 40.0f ? UITheme::AccentOrange : UITheme::AccentRed;
        sv(statRows[3].value, oss.str(), c);
    }
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << statAvgTime << " s";
        sv(statRows[4].value, statHits > 0 ? oss.str() : "--",
           statHits > 0 ? UITheme::TextWhite : UITheme::TextDim);
    }
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << statTotalTime << " s";
        sv(statRows[5].value, oss.str());
    }

    std::string rating;
    sf::Color rc;
    if (statAccuracy >= 80.0f)      { rating = "S"; rc = sf::Color(255, 215, 0); }
    else if (statAccuracy >= 65.0f) { rating = "A"; rc = UITheme::AccentGreen; }
    else if (statAccuracy >= 50.0f) { rating = "B"; rc = UITheme::AccentBlue; }
    else if (statAccuracy >= 35.0f) { rating = "C"; rc = UITheme::AccentOrange; }
    else                            { rating = "D"; rc = UITheme::AccentRed; }

    if (ratingText)
    {
        ratingText->setString("Rating: " + rating);
        ratingText->setFillColor(rc);
    }
}

void ResultScreen::layout()
{
    const float w = static_cast<float>(viewSize.x);
    const float h = static_cast<float>(viewSize.y);
    const float cx = w * 0.5f;
    const float cy = h * 0.5f;
    overlayBg.setSize({ w, h });
    overlayBg.setPosition({ 0.0f, 0.0f });

    const float panelW = w * 0.42f;
    const float panelH = h * 0.58f;
    panelBg.setSize({ panelW, panelH });
    panelBg.setPosition({ cx - panelW * 0.5f, cy - panelH * 0.5f });

    if (titleText)
    {
        titleText->setCharacterSize(static_cast<unsigned int>(h * 0.05f));
        const sf::FloatRect tb = titleText->getLocalBounds();
        titleText->setOrigin({ tb.position.x + tb.size.x * 0.5f, tb.position.y });
        titleText->setPosition({ cx, cy - panelH * 0.5f + h * 0.03f });
    }

    const float rowStartY = cy - panelH * 0.5f + h * 0.11f;
    const float rowSpacing = h * 0.045f;
    const float labelX = cx - panelW * 0.33f;
    const float valueX = cx + panelW * 0.33f;
    const unsigned int rowFontSize = static_cast<unsigned int>(h * 0.024f);

    for (int i = 0; i < 6; ++i)
    {
        const float ry = rowStartY + static_cast<float>(i) * rowSpacing;
        if (statRows[i].label)
        {
            statRows[i].label->setCharacterSize(rowFontSize);
            const auto& b = statRows[i].label->getLocalBounds();
            statRows[i].label->setOrigin({ b.position.x, b.position.y + b.size.y * 0.5f });
            statRows[i].label->setPosition({ labelX, ry });
        }
        if (statRows[i].value)
        {
            statRows[i].value->setCharacterSize(rowFontSize);
            const auto& b = statRows[i].value->getLocalBounds();
            statRows[i].value->setOrigin({ b.position.x + b.size.x, b.position.y + b.size.y * 0.5f });
            statRows[i].value->setPosition({ valueX, ry });
        }
    }

    separator.setSize({ panelW - panelW * 0.15f, 1.5f });
    separator.setPosition({ cx - (panelW - panelW * 0.15f) * 0.5f, rowStartY + 6.0f * rowSpacing + 5.0f });

    if (ratingText)
    {
        ratingText->setCharacterSize(static_cast<unsigned int>(h * 0.03f));
        const auto& rb = ratingText->getLocalBounds();
        ratingText->setOrigin({ rb.position.x + rb.size.x * 0.5f, rb.position.y });
        ratingText->setPosition({ cx, rowStartY + 6.0f * rowSpacing + h * 0.03f });
    }

    const float btnW = w * 0.28f;
    const float btnH = h * 0.065f;
    const unsigned int btnFontSize = static_cast<unsigned int>(h * 0.032f);
    buttons[0].setSize({ btnW, btnH });
    buttons[0].setFontSize(btnFontSize);
    buttons[1].setSize({ btnW, btnH });
    buttons[1].setFontSize(btnFontSize);
    const float btnX = cx - btnW * 0.5f;
    buttons[0].setPosition({ btnX, cy + panelH * 0.5f - h * 0.14f });
    buttons[1].setPosition({ btnX, cy + panelH * 0.5f - h * 0.07f });
}
