#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <memory>
#include <pthread.h>
#include <cerrno>
#include "ball.hpp"
#include "ballmanager.hpp"

const int WIDTH = 800, HEIGHT = 800;
std::mutex mtx;
std::atomic<bool> running{true};

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void updateTask(std::shared_ptr<BallManager> ballManager)
{
    auto wakeupTime = std::chrono::steady_clock::now();

    while (running) 
    {
        std::unique_lock<std::mutex> lock(mtx);
        wakeupTime += std::chrono::microseconds(ballManager->timePeriod);
        ballManager->updateBalls();
        lock.unlock();
        std::this_thread::sleep_until(wakeupTime);
    }
}

void set_highest_priority(std::thread &t) {
    pthread_t native_handle = t.native_handle();

    sched_param sch_params;
    sch_params.sched_priority = sched_get_priority_max(SCHED_FIFO);

    if (pthread_setschedparam(native_handle, SCHED_FIFO, &sch_params) != 0) {
        std::cerr << "Failed to set thread priority" << std::endl;
        running = false;
    }
}

int main(int argc, char* argv[]) {
    if(argc != 2){
        std::cout << "No ball number was entered" << std::endl;
        return 0;
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bouncing Ball", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    srand(time(nullptr));

    const int period = 500; //micros
    const int ballNumber = atoi(argv[1]);
    std::vector<Ball> ballVector;
    for(int iter = 0; iter < ballNumber; iter++)
    {
        RGB colors = {((rand() % 90) + 10) / 100.0f, ((rand() % 90) + 10) / 100.0f, ((rand() % 90) + 10) / 100.0f};
        ballVector.push_back(Ball(0.1f, colors));
    }

    std::shared_ptr<BallManager> ballManager = std::make_shared<BallManager>(ballVector, WIDTH, HEIGHT, period);

    if(!ballManager->placeBalls()){
        std::cout << "Could not place balls" << std::endl;
        return 0;
    }

    std::thread updateThread(updateTask, ballManager);
    set_highest_priority(updateThread);
    
    while (!glfwWindowShouldClose(window) && running) {

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);
        
        ballManager->drawBalls();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    running = false;  
    updateThread.join(); 
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
