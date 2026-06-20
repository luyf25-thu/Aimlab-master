#pragma once

#include <SFML/Audio.hpp>

#include <cstddef>
#include <optional>
#include <vector>

#include "RecoilComponent.h"

struct WeaponConfig
{
    int ammoCapacity = 0;
    float fireCooldown = 0.0f;
    bool automatic = false;
    RecoilComponent recoil;
};

// Base class for all weapons. Shared shooting, reload, ammo, and recoil logic
// lives here; concrete weapons provide different configs.
class Weapon
{
public:
    virtual ~Weapon() = default;

    virtual bool fire()
    {
        if (!canFire())
        {
            return false;
        }
        commitFire();
        return true;
    }

    virtual void update(float deltaTime, bool isFiring)
    {
        timeSinceLastFire += deltaTime;
        recoil.update(deltaTime, isFiring);
        if (!infiniteAmmo && !isReloading && currentAmmo == 0)
        {
            reload();
        }
        if (isReloading)
        {
            reloadTimer -= deltaTime;
            if (reloadTimer <= 0.0f)
            {
                currentAmmo = ammoCapacity;
                isReloading = false;
            }
        }
    }

    void reload()
    {
        if (isReloading || currentAmmo == ammoCapacity)
        {
            return;
        }
        isReloading = true;
        reloadTimer = reloadDuration;
        playReloadSound();
    }

    int getCurrentAmmo() const
    {
        return currentAmmo;
    }

    int getAmmoCapacity() const
    {
        return ammoCapacity;
    }

    bool getIsReloading() const
    {
        return isReloading;
    }

    void setInfiniteAmmo(bool enabled)
    {
        infiniteAmmo = enabled;
    }

    bool getInfiniteAmmo() const
    {
        return infiniteAmmo;
    }

    void setFireSoundBuffer(const sf::SoundBuffer& buffer)
    {
        fireSounds.clear();
        fireSounds.reserve(soundPoolSize);
        for (std::size_t i = 0; i < soundPoolSize; ++i)
        {
            fireSounds.emplace_back(buffer);
            fireSounds.back().setVolume(fireVolume);
        }
        nextSoundIndex = 0;
    }

    void setFireVolume(float volume)
    {
        fireVolume = volume;
        for (auto& sound : fireSounds)
        {
            sound.setVolume(fireVolume);
        }
    }

    void setReloadSoundBuffer(const sf::SoundBuffer& buffer)
    {
        reloadSound.emplace(buffer);
        reloadSound->setVolume(reloadVolume);
    }

    void setReloadVolume(float volume)
    {
        reloadVolume = volume;
        if (reloadSound)
        {
            reloadSound->setVolume(reloadVolume);
        }
    }

    virtual bool isAutomatic() const
    {
        return automatic;
    }

    sf::Vector2f getRecoilOffset() const
    {
        return recoil.getOffset();
    }

protected:
    Weapon() = default;

    explicit Weapon(const WeaponConfig& config)
        : recoil(config.recoil),
          ammoCapacity(config.ammoCapacity),
          currentAmmo(config.ammoCapacity),
          fireCooldown(config.fireCooldown),
          automatic(config.automatic)
    {
    }

    bool canFire() const
    {
        const bool hasAmmo = infiniteAmmo || currentAmmo > 0;
        return !isReloading && hasAmmo && timeSinceLastFire >= fireCooldown;
    }

    void commitFire()
    {
        if (!infiniteAmmo)
        {
            --currentAmmo;
        }
        timeSinceLastFire = 0.0f;
        recoil.applyRecoil();
        playFireSound();
    }

    void playFireSound()
    {
        if (fireSounds.empty())
        {
            return;
        }

        sf::Sound& sound = fireSounds[nextSoundIndex];
        sound.stop();
        sound.play();
        nextSoundIndex = (nextSoundIndex + 1) % fireSounds.size();
    }

    void playReloadSound()
    {
        if (!reloadSound)
        {
            return;
        }

        reloadSound->stop();
        reloadSound->play();
    }

    RecoilComponent recoil;
    int ammoCapacity = 0;
    int currentAmmo = 0;
    float fireCooldown = 0.0f;
    float timeSinceLastFire = 0.0f;
    static constexpr std::size_t soundPoolSize = 8;
    std::vector<sf::Sound> fireSounds;
    std::size_t nextSoundIndex = 0;
    float fireVolume = 100.0f;
    std::optional<sf::Sound> reloadSound;
    float reloadVolume = 85.0f;
    bool isReloading = false;
    float reloadTimer = 0.0f;
    float reloadDuration = 2.0f;
    bool infiniteAmmo = false;
    bool automatic = false;
};
