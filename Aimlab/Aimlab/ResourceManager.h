#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

class ResourceManager
{
public:
    static ResourceManager& getInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    void loadTexture(const std::string& name, const std::filesystem::path& filename)
    {
        sf::Texture texture;
        if (!texture.loadFromFile(filename))
        {
            throw std::runtime_error("texture load failed: " + filename.string());
        }
        textures[name] = std::move(texture);
    }

    sf::Texture& getTexture(const std::string& name)
    {
        return textures.at(name);
    }

    void loadSoundBuffer(const std::string& name, const std::filesystem::path& filename)
    {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filename))
        {
            throw std::runtime_error("sound load failed: " + filename.string());
        }
        soundBuffers[name] = std::move(buffer);
    }

    sf::SoundBuffer& getSoundBuffer(const std::string& name)
    {
        return soundBuffers.at(name);
    }

    void loadFont(const std::string& name, const std::filesystem::path& filename)
    {
        sf::Font font;
        if (!font.openFromFile(filename))
        {
            throw std::runtime_error("font load failed: " + filename.string());
        }
        fonts[name] = std::move(font);
    }

    sf::Font& getFont(const std::string& name)
    {
        return fonts.at(name);
    }

private:
    ResourceManager() = default;

    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
    std::unordered_map<std::string, sf::Font> fonts;
};

