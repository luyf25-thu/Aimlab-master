#pragma once

#include "GameState.h"
#include "UIComponents.h"

#include <SFML/Graphics.hpp>

#include <array>
#include <functional>
#include <optional>

class HelpScreen
{
public:
    using StateCallback = std::function<void(GameState)>;

    HelpScreen() = default;

    void init();
    void setStateCallback(StateCallback cb);
    void onResize(const sf::Vector2u& windowSize);
    void handleEvent(const sf::Event& event);
    void render(sf::RenderWindow& window);

private:
    void layout();

    sf::RectangleShape overlayBg, panelBg;
    std::optional<sf::Text> titleText;
    std::array<std::optional<sf::Text>, 3> sectionHeadings;
    struct KeyRow { std::optional<sf::Text> key, dash, desc; };
    std::array<KeyRow, 8> keyRows;
    sf::RectangleShape separator1, separator2;
    struct InfoRow { std::optional<sf::Text> desc; };
    std::array<InfoRow, 3> modeRows, weaponRows;
    std::optional<sf::Text> hintText;
    UIButton btnBack;
    sf::Vector2u viewSize{ 800, 600 };
    StateCallback onStateChange;
};
