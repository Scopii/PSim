#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include "Types.h"
#include "BaseShader.h"
#include "ComputeShader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Camera.h"
#include "ParticleCluster.h"
#include "GUIElement.h"
#include "StorageBuffer.h"
#include "ParticleUtils.h"
#include "UniformBuffer.h"

class Application {
public:
    Application(unsigned short, unsigned short);
    ~Application();
    void Init();
    void InitOpenGLWindow();
    void Run();
    void Update(float deltaTime);
    void ComputeUpdate(float deltaTime);
    void Render();
    static void key_callback(GLFWwindow* window, const int key, const int scancode,const int action, const int mods) {
        if (currentInstance) {
            currentInstance->handleKeyCallback(window, key, scancode, action, mods);
        }
    }
private:
    void handleKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static Application* currentInstance;

    unsigned short screenWidth;
    unsigned short screenHeight;
    GLFWwindow* window;

    Camera* cam;
    GUIElement* GUI;
    ParticleCluster cluster = ParticleCluster(NUM_PARTICLES);

    BaseShader* shader;
    ComputeShader* comp;

    StorageBuffer* particleBuffer;
    StorageBuffer* sizeBuffer;
    StorageBuffer* accelBuffer;
    StorageBuffer* colorBuffer;
    StorageBuffer* alphaBuffer;

    UniformBuffer* timeUBO;
    UniformBuffer* velUBO;
    UniformBuffer* posUBO;
    UniformBuffer* gravUBO;
    UniformBuffer* spinUBO;
    UniformBuffer* sizeUBO;
    UniformBuffer* accelUBO;
    UniformBuffer* colorUBO;
    UniformBuffer* alphaUBO;

    TimeData timeData;
    DirecAngleVecData velData;
    RadiusVecData posData;
    DirecAngleVecData gravData;
    SpinData spinData;
    RandAttributeGradient sizeData;
    RandAttributeGradient accelData;
    RandAttributeGradient colorData;
    RandAttributeGradient alphaData;

    //Time related
    double lastTime;
    double accumulator;
    float updatesPerSecond = 240.0;
    unsigned long long totalUpdates = 0;
    unsigned long long totalFrames = 0;
    double curTime = 0;
    double deltaTime = 0;
};
