#pragma once

#include <memory>
#include <vector>

#include "StaticTarget.h"

class TargetPool
{
public:
    explicit TargetPool(std::size_t size = 50)
    {
        pool.reserve(size);
        for (std::size_t i = 0; i < size; ++i)
        {
            pool.emplace_back(std::make_unique<StaticTarget>());
        }
    }

    Target* acquireTarget()
    {
        for (const auto& target : pool)
        {
            if (!target->getIsActive())
            {
                return target.get();
            }
        }
        return nullptr;
    }

    void releaseTarget(Target* target)
    {
        if (target)
        {
            target->deactivate();
        }
    }

    void deactivateAll()
    {
        for (const auto& target : pool)
        {
            target->deactivate();
        }
    }

    const std::vector<std::unique_ptr<Target>>& getTargets() const
    {
        return pool;
    }

private:
    std::vector<std::unique_ptr<Target>> pool;
};
