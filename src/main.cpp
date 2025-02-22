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
#include <string.h>
#include "ball.hpp"
#include "ballmanager.hpp"

std::mutex mtx;
std::atomic<bool> running{true};

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void setProjection(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspectRatio = static_cast<float>(height) / width;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -aspectRatio, aspectRatio, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

bool parseArgs(int argc, char* argv[], std::string executableName, int& ballNumber, float& radius)
{
    auto printUsage = [executableName]()
    {
        std::cout << "Usage :" << std::endl;
        std::cout << "sudo ./" << executableName << " -b <ballnumber> -r <radius>" << std::endl;
        std::cout << "Where <ballnumber> is positive integer and <radius> is positive floating point number" << std::endl;
        std::cout << "Assigning 0 or 0.0 is interpreted as random" << std::endl;
    };
    switch(argc){
        case 1:
            printUsage();
            std::cout << "Defaulting to -b 5 -r 0.1" << std::endl;
            ballNumber = 5;
            radius = 0.1f;
            return true;
        case 5:
            if(!strcmp(argv[1], "-b") && !strcmp(argv[3], "-r"))
            {
                ballNumber = atoi(argv[2]);
                char *endptr;
                radius = strtof(argv[4], &endptr);
            }
            else{ printUsage(); return false; }
            break;
        default:
            printUsage(); return false;
    };
    if(ballNumber < 0 || radius < 0.0){ printUsage(); return false; }
    return true;
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
    
    int ballNumber = 0;
    float radius = 0.0;
    if(!parseArgs(argc, argv, "bouncing_ball", ballNumber, radius)){
        return -1;
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Bouncing Ball", primaryMonitor, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    glewInit();

    srand(time(nullptr));

    const int period = 500; //micros
    std::vector<Ball> ballVector;
    for(int iter = 0; iter < (ballNumber > 0 ? ballNumber : (rand() % 10) + 1); iter++)
    {
        RGB colors = {((rand() % 90) + 10) / 100.0f, ((rand() % 90) + 10) / 100.0f, ((rand() % 90) + 10) / 100.0f};
        ballVector.push_back(Ball((radius > 0.0 ? radius : ((rand() % 90) + 10) / 1000.0f), colors));
    }

    std::shared_ptr<BallManager> ballManager = std::make_shared<BallManager>(ballVector, mode->width, mode->height, period);

    if(!ballManager->placeBalls()){
        std::cout << "Could not place balls" << std::endl;
        return 0;
    }

    std::thread updateThread(updateTask, ballManager);
    set_highest_priority(updateThread);
    
    while (!glfwWindowShouldClose(window) && running) {

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);
        
        setProjection(window);
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
