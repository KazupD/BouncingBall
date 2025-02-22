#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "ball.hpp"

Ball::Ball(const float r, const RGB rgb)
{
    this->vx = 0.0f;
    this->vy = 0.0f;
    this->radius = r;
    this->mass = r*r;
    this->rgb = rgb;
}

void Ball::drawBall()
{
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(this->rgb.red, this->rgb.green, this->rgb.blue);
    glVertex2f(this->x, this->y);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(this->x + cos(angle) * this->radius, this->y + sin(angle) * this->radius);
    }
    glEnd();
}

Ball::~Ball(){}