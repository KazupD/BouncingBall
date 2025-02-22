#include "ballmanager.hpp"
#include <cstdlib>
#include <cmath>
#include <iostream>

BallManager::BallManager(std::vector<Ball>& balls, const int w, const int h, const int tp)
    : ballVector(balls)
{
    this->WIDTH = w;
    this->HEIGHT = h;
    this->timePeriod = tp;
    this->relativeHalfWidth = 1.0f;
    this->relativeHalfHeight = (float)h/(float)w;
}

bool BallManager::isCollideWithBall(Ball& b1, Ball& b2)
{
    float eucDistance = sqrt((b1.x-b2.x)*(b1.x-b2.x) + (b1.y-b2.y)*(b1.y-b2.y));
    if(eucDistance <= b1.radius + b2.radius){
        return true;
    }
    return false;
}
bool BallManager::isCollideWithWall(Ball& b1)
{
    if((b1.x+b1.radius) >= relativeHalfWidth || (b1.x-b1.radius) <= -relativeHalfWidth ||
        (b1.y+b1.radius) >= relativeHalfHeight || (b1.y-b1.radius) <= -relativeHalfHeight){
        return true;
    }
    return false;
}

bool BallManager::aboutToCollide(Ball& b1, Ball& b2) {
    float n12x = b2.x - b1.x;
    float n12y = b2.y - b1.y;
    float relative_vx = b2.vx - b1.vx;
    float relative_vy = b2.vy - b1.vy;

    return (n12x * relative_vx + n12y * relative_vy) <= 0;
}

void BallManager::hitWall(Ball& b1)
{
    if (b1.x - b1.radius < -relativeHalfWidth || b1.x + b1.radius > relativeHalfWidth) {
        b1.vx *= -energyLoss;
        b1.x = b1.x - b1.radius < -relativeHalfWidth ? -relativeHalfWidth + b1.radius : relativeHalfWidth - b1.radius;
    }
    if (b1.y - b1.radius < -relativeHalfHeight || b1.y + b1.radius > relativeHalfHeight) {
        b1.vy *= -energyLoss;
        b1.y = b1.y - b1.radius < -relativeHalfHeight ? -relativeHalfHeight + b1.radius : relativeHalfHeight - b1.radius;
    }
}

bool BallManager::placeBalls()
{
    const int maxTryNumber = 15;
    for(int i = 0; i < this->ballVector.size(); i++)
    {
        bool placed = true;
        int counter = 0;
        do
        {
            ballVector[i].x = ((rand() % 2000) - 1000) / 1000.0f;
            ballVector[i].y = ((rand() % 2000) - 1000) / 1000.0f;

            for(int j = 0; j < i; j++)
            {
                if(isCollideWithBall(ballVector[i], ballVector[j])){
                    placed = false;
                    break;
                }
                if(isCollideWithWall(ballVector[i])){
                    placed = false;
                    break;
                }
                if(j == i-1){placed = true;}
            }

            counter++;
            if(counter > maxTryNumber){ return false;}

        } while(!placed);

        ballVector[i].vx = ((rand() % 2000) - 1000) / 100.0f;
        ballVector[i].vy = ((rand() % 2000) - 1000) / 100.0f;
    }
    return true;
}

void BallManager::updateBalls()
{
    std::vector<Ball> ballVectorSave = ballVector;

    for (int i = 0; i < ballVector.size(); i++)
    {   
        ballVector[i].fx = 0.0f;
        ballVector[i].fy = gravity;
        for (int j = i + 1; j < ballVector.size(); j++)
        {
            float cx = ballVectorSave[j].x - ballVectorSave[i].x;
            float cy = ballVectorSave[j].y - ballVectorSave[i].y;
            float realDistance = sqrt(cx*cx + cy*cy);
            float cAngle = atan2(cy, cx);
            if (isCollideWithBall(ballVectorSave[i], ballVectorSave[j]) &&
                aboutToCollide(ballVectorSave[i], ballVectorSave[j]))
            {
                float nx = cos(cAngle);
                float ny = sin(cAngle);

                float v1n = ballVectorSave[i].vx * nx + ballVectorSave[i].vy * ny;
                float v2n = ballVectorSave[j].vx * nx + ballVectorSave[j].vy * ny;

                float m1 = ballVector[i].mass;
                float m2 = ballVector[j].mass;
                
                float v1n_after = ((m1 - m2) * v1n + 2 * m2 * v2n) / (m1 + m2);
                float v2n_after = ((m2 - m1) * v2n + 2 * m1 * v1n) / (m1 + m2);
                
                float v1t_x = ballVectorSave[i].vx - v1n * nx;
                float v1t_y = ballVectorSave[i].vy - v1n * ny;
                float v2t_x = ballVectorSave[j].vx - v2n * nx;
                float v2t_y = ballVectorSave[j].vy - v2n * ny;

                float damping = std::max(0.0f, (realDistance/(ballVector[i].radius+ballVector[j].radius)))*dampingBias;

                ballVector[i].vx = (v1t_x + v1n_after * nx)*damping;
                ballVector[i].vy = (v1t_y + v1n_after * ny)*damping;

                ballVector[j].vx = (v2t_x + v2n_after * nx)*damping;
                ballVector[j].vy = (v2t_y + v2n_after * ny)*damping;

            }

            if(isCollideWithBall(ballVectorSave[i], ballVectorSave[j]))
            {
                float elasticity = std::max(0.0f, ((ballVector[i].radius+ballVector[j].radius)/realDistance))*elasticityBias;
                float force = fabs(sin(cAngle))*fabs(gravity)*elasticity;
                ballVector[i].fx += -cos(cAngle)*force;
                ballVector[i].fy += -sin(cAngle)*force;
            }

        }

        ballVector[i].vx += ballVector[i].fx*((float)(this->timePeriod)/1000000.0f);
        ballVector[i].vy += ballVector[i].fy*((float)(this->timePeriod)/1000000.0f);
        ballVector[i].x += ballVector[i].vx*((float)(this->timePeriod)/1000000.0f);
        ballVector[i].y += ballVector[i].vy*((float)(this->timePeriod)/1000000.0f);

        hitWall(ballVector[i]);
    }
}

void BallManager::drawBalls()
{
    for(Ball& b1 : ballVector)
    {
        b1.drawBall();
    }
}

BallManager::~BallManager(){}