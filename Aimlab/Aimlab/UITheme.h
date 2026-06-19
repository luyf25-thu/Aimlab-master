#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>

namespace UITheme
{
    // Main colors.
    inline const sf::Color BgDark{ 18, 18, 30, 255 };
    inline const sf::Color PanelBg{ 22, 22, 42, 220 };
    inline const sf::Color PanelBorder{ 60, 70, 100, 180 };
    inline const sf::Color OverlayBg{ 0, 0, 0, 175 };

    // Accent colors.
    inline const sf::Color AccentBlue{ 79, 195, 247, 255 };
    inline const sf::Color AccentOrange{ 255, 152, 0, 255 };
    inline const sf::Color AccentGreen{ 76, 175, 80, 255 };
    inline const sf::Color AccentRed{ 244, 67, 54, 255 };

    // Text colors.
    inline const sf::Color TextWhite{ 245, 245, 250, 255 };
    inline const sf::Color TextGray{ 160, 170, 190, 255 };
    inline const sf::Color TextDim{ 100, 108, 130, 255 };

    // Weapon bar colors.
    inline const sf::Color WeaponBg{ 28, 28, 50, 200 };
    inline const sf::Color WeaponActive{ 79, 195, 247, 60 };
    inline const sf::Color WeaponInactive{ 35, 35, 55, 180 };

    // Ammo bar colors.
    inline const sf::Color AmmoBg{ 35, 35, 55, 255 };
    inline const sf::Color AmmoFill{ 79, 195, 247, 255 };
    inline const sf::Color AmmoLow{ 244, 67, 54, 255 };

    // Button colors.
    inline const sf::Color BtnNormal{ 45, 50, 75, 230 };
    inline const sf::Color BtnHover{ 60, 68, 100, 240 };
    inline const sf::Color BtnPressed{ 35, 40, 60, 250 };
    inline const sf::Color BtnBorder{ 79, 195, 247, 100 };
    inline const sf::Color BtnBorderHover{ 79, 195, 247, 220 };

    // Size constants.
    inline constexpr float PanelPadding = 14.0f;
    inline constexpr float PanelBorderWidth = 1.5f;
    inline constexpr float ButtonHeight = 46.0f;
    inline constexpr float ButtonMinWidth = 220.0f;
    inline constexpr float TopBarHeight = 44.0f;
    inline constexpr float BottomBarHeight = 80.0f;
    inline constexpr float WeaponSlotWidth = 80.0f;
    inline constexpr float WeaponSlotHeight = 64.0f;
    inline constexpr float AmmoBarWidth = 160.0f;
    inline constexpr float AmmoBarHeight = 12.0f;

    // Global font pointer set during Game initialization.
    inline sf::Font* DefaultFont = nullptr;
}
