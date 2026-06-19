#pragma once

#include "GameState.h"
#include "UIComponents.h"

#include <SFML/Graphics.hpp>

#include <functional>
#include <optional>

class MainMenu
{
public:
    using StateCallback = std::function<void(GameState)>;

    MainMenu() = default;

    bool init(const sf::Texture& backgroundTexture);
    void setStateCallback(StateCallback cb);
    void onResize(const sf::Vector2u& windowSize);
    void handleEvent(const sf::Event& event);
    void render(sf::RenderWindow& window);

private:
    void createElements();
    void updateBackgroundScale();
    void layoutElements();

    std::optional<sf::Sprite> bgSprite;
    sf::RectangleShape overlayBg;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> subtitleText;
    sf::RectangleShape titleLine;

    UIButton btnStart;
    UIButton btnSettings;
    UIButton btnHelp;
    UIButton btnExit;

    std::optional<sf::Text> versionText;

    sf::Vector2u viewSize{ 800, 600 };
    StateCallback onStateChange;
};
