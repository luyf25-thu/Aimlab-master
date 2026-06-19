#pragma once

#include <algorithm>

class ScoreManager
{
public:

    void recordHit()
    {
        ++hits;
        ++totalShots;
    }


    void recordMiss()
    {
        ++misses;
        ++totalShots;
    }

    float getAccuracy() const
    {
        if (totalShots == 0)
        {
            return 0.0f;
        }
        return static_cast<float>(hits) / static_cast<float>(totalShots) * 100.0f;
    }


    void reset()
    {
        hits = 0;
        misses = 0;
        totalShots = 0;
    }


    int getHits() const
    {
        return hits;
    }


    int getMisses() const
    {
        return misses;
    }

private:
    int hits = 0;
    int misses = 0;
    int totalShots = 0;
};

