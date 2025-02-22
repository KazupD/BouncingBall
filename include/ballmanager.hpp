#ifndef BALLMANAGER_HPP
#define BALLMANAGER_HPP

#include <vector>
#include "ball.hpp"

class BallManager {
    int WIDTH;
    int HEIGHT;
    float relativeHalfWidth;
    float relativeHalfHeight;
    const float gravity = -10.0f;
    const float energyLoss = 0.95f;
    const float dampingBias = 0.9f;
    const float elasticityBias = 10.0f;
    std::vector<Ball> ballVector;

    bool isCollideWithBall(Ball& b1, Ball& b2);
    bool isCollideWithWall(Ball& b1);
    bool aboutToCollide(Ball& b1, Ball& b2);
    void hitWall(Ball& b1);

public:
    int timePeriod;
    
    BallManager() = delete;
    BallManager(std::vector<Ball>& balls, const int w, const int h, const int tp);

    bool placeBalls();
    void updateBalls();
    void drawBalls();

    ~BallManager();
};

#endif