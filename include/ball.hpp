#ifndef BALL_HPP
#define BALL_HPP

struct RGB{
    float red;
    float green;
    float blue;
};

class Ball{
public:
    RGB rgb = {0.0, 0.0, 0.0};
    float x = 0.0, y = 0.0;
    float vx = 0.0f, vy = 0.0f;
    float fx = 0.0f, fy = 0.0f;
    float radius = 0.0;
    float mass = 0.0;

    Ball() = delete;
    Ball(const float r, const RGB rgb);
    void drawBall();
    ~Ball();
};

#endif