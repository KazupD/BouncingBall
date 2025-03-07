#ifndef BALL_HPP
#define BALL_HPP

struct RGB{
    float red;
    float green;
    float blue;
};

class Ball{
public:
    RGB rgb;
    float x, y;
    float vx = 0.0f, vy = 0.0f;
    float fx = 0.0f, fy = 0.0f;
    float radius;
    float mass;

    Ball() = delete;
    Ball(const float r, const RGB rgb);
    void drawBall();
    ~Ball();
};

#endif