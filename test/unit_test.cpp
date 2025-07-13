#include <gtest/gtest.h>
#include "../include/ball.hpp"

TEST(ballTest, bouncing_unit_test) {
    Ball b1 = Ball(0.1f, {100.0f, 100.0f, 100.0f});

    EXPECT_NEAR(b1.radius, 0.1f, 1e-6);
    EXPECT_NEAR(b1.mass, 0.01f, 1e-6);
    EXPECT_NEAR(b1.rgb.red, 100.0f, 1e-6);
    EXPECT_NEAR(b1.rgb.green, 100.0f, 1e-6);
    EXPECT_NEAR(b1.rgb.blue, 100.0f, 1e-6);
    EXPECT_NEAR(b1.x, 0.0f, 1e-6);
    EXPECT_NEAR(b1.y, 0.0f, 1e-6);
    EXPECT_NEAR(b1.vx, 0.0f, 1e-6);
    EXPECT_NEAR(b1.vy, 0.0f, 1e-6);
    EXPECT_NEAR(b1.fx, 0.0f, 1e-6);
    EXPECT_NEAR(b1.fy, 0.0f, 1e-6);
}
