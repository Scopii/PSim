#include "Application.h"
#include <ctime>
#include <GL/gl.h>

using namespace ParticleUtils;

Application* Application::currentInstance = nullptr;

Application::Application(unsigned short width, unsigned short height){
    screenWidth = width;
    screenHeight = height;
    Init();
}

void Application::InitOpenGLWindow()
{
    /* Initialize the library */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        throw std::runtime_error("Failed to initialize GLFW");
    }
    // Get primary monitor for fullscreen
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Now create the actual window
    window = glfwCreateWindow(screenWidth, screenHeight, "Particle Simulation", primaryMonitor, nullptr);
    if (!window) {
        const char* description;
        int errorCode = glfwGetError(&description);
        std::cerr << "GLFW Error Code: " << errorCode << " Description: " << description << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSwapInterval(0); //-> V-Sync
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_BLEND); // Enable Blending
    glDepthFunc(GL_LESS); // Depth test passes if incoming depth value is less than the stored depth value

    glEnable(GL_DEPTH_TEST); // Enable Depth Test
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set Blending Settings
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    glfwSetKeyCallback(window, key_callback); // Set the key callback
}

void Application::Init()
{
    currentInstance = this;
    InitOpenGLWindow(); //indow and OPENGL Settings

    timeData= {0.0, 0.0, 2.0 , 2.0};
    velData= {vec4(vec3(0.0, 1.0, 0.0),0),1.0,0.0001,360.0,1.0,1.0,1.0};
    posData= {0.0, 0.0, vec4(vec3(0.0, 0.0, 0.0),0)};
    gravData= {vec4(vec3(0.0, 1.0, 0.0),0),0.3,3.0,360.0,1.0,1.0,1.0};
    spinData= {0.0,TWO_PI,-TWO_PI,TWO_PI};

    sizeData = {
        0.4,0.4,0.65,0.65,
        0.0,0.1,0.9,1.0,
        0.0,0.1,0.0,0.1,
        1.0,1.0,1,1};

    accelData = {0.0,0.0,0.9,1.0,
        0.0,0.1,0.9,1.0,
        0.0,0.2,0.0,0.2,
        0.5,0.1,1,5};

    colorData = { // ARE NEGATIVE VALUES FINE?
        0.0,0.0,0.99,1.0,
        0.0,0.0,1.0,1.0,
        -0.0,0.0,-0.0,-0.0,
        1.0,1.0,1,1};

    alphaData = {
        0.0,0.2,0.8,1.0,
        0.0,0.2,0.8,1.0,
        0.0,0.1,0.0,0.0,
        0.8,1.0,1,1};

    timeUBO = new UniformBuffer(&timeData,sizeof(timeData),0 ,DYNAMIC);
    posUBO = new UniformBuffer(&posData, sizeof(posData), 1, DYNAMIC);
    spinUBO = new UniformBuffer(&spinData, sizeof(spinData), 2, DYNAMIC);
    sizeUBO = new UniformBuffer(&sizeData, sizeof(sizeData), 3, DYNAMIC);
    accelUBO = new UniformBuffer(&accelData, sizeof(accelData), 4, DYNAMIC);
    colorUBO = new UniformBuffer(&colorData, sizeof(colorData), 5, DYNAMIC);
    alphaUBO = new UniformBuffer(&alphaData, sizeof(alphaData), 6, DYNAMIC);
    velUBO = new UniformBuffer(&velData, sizeof(velData), 7, DYNAMIC);
    gravUBO = new UniformBuffer(&gravData, sizeof(gravData), 8, DYNAMIC);

    particleBuffer = new StorageBuffer(cluster.particles.data(), NUM_PARTICLES * sizeof(ParticleCluster::Particle), 0, DYNAMIC);

    vectorArray<v4> colorArray = {
        {0.977f, 0.4f, 0.0f, 0.0f},
        {0.954f, 0.186f, 0.227f, 0.0f},
        {0.904f, 0.261f, 0.326f, 0.0f},
        {0.86f, 0.308f, 0.403f, 0.0f},
        {0.809f, 0.338f, 0.473f, 0.0f},
        {0.758f, 0.364f, 0.530f, 0.0f},
        {0.709f, 0.38f, 0.585f, 0.0f},
        {0.657f, 0.392f, 0.64f, 0.0f},
        {0.604f, 0.399f, 0.686f, 0.0f},
        {0.547f, 0.402f, 0.736f, 0.0f},
        {0.486f, 0.40f, 0.781f, 0.0f},
        {0.425f, 0.389f, 0.826f, 0.0f},
        {0.357f, 0.377f, 0.874f, 0.0f},
        {0.269f, 0.365f, 0.914f, 0.0f},
        {0.163f, 0.338f, 0.961f, 0.0f},
        {0.16f, 0.304f, 1.0f, 0.0f}
    };
    colorBuffer = new StorageBuffer(colorArray.data(),colorArray.size() * sizeof(vec4), 1, DYNAMIC);

    vectorArray<float> sizeArray = {
        0.05,
        0.05,
        0.05,
    };
    sizeBuffer = new StorageBuffer(sizeArray.data(),sizeArray.size() * sizeof(float), 2, DYNAMIC);

    vectorArray<float> accelArray = {
        10.0,
        0.0,
        0.0,
    };
    accelBuffer = new StorageBuffer(accelArray.data(),accelArray.size() * sizeof(float), 3, DYNAMIC);

    vectorArray<float> alphaArray = {
        0.0,
        1.0,
        0.0,
    };
    alphaBuffer = new StorageBuffer(alphaArray.data(),alphaArray.size() * sizeof(float), 4, DYNAMIC);

    GUI = new GUIElement(window);
    GUI->setPointers(&timeData,&velData,&posData,&gravData,&spinData,&sizeData,&accelData,&colorData,&alphaData);
    shader = new BaseShader("res/shaders/VertShdr.glsl","res/shaders/GeoShdr.glsl", "res/shaders/FragShdr.glsl");
    //computeShader = new ComputeShader("res/shaders/CmpShdr.glsl");
    cam = new Camera(90.0f, (float)screenWidth / screenHeight, 0.01f, 10000.0f, 10.0f);
    cam->bind(*shader);
}

void Application::Run()
{
    while (!glfwWindowShouldClose(window)) {
        curTime = glfwGetTime();
        deltaTime = curTime - lastTime;
        lastTime = curTime;
        accumulator += deltaTime;

        while (accumulator >= 1.0 / updatesPerSecond) {
            accumulator -= 1.0 / updatesPerSecond;
            totalUpdates++;
        }



        ComputeUpdate(deltaTime);
        Update(deltaTime);

        GUI->GPUTrack();
        Render();
        GUI->GPUEndTrack();
        totalFrames++;
    }
    glfwTerminate();
}

void Application::ComputeUpdate(float deltaTime) {
    //GUI->ComputeTrack();
    //computeShader->Bind();
    shader->SetUniform("deltaTime", deltaTime);
    shader->SetUniform("totalTime", (float)curTime);
    //computeShader->Dispatch(NUM_PARTICLES / WORK_GROUP_SIZE + 1, 1, 1);
    //GUI->ComputeEndTrack();
    //shader->Bind(); // for safety
}

void Application::Update(float deltaTime){
    //GUI->CPUTrack();
    //timeData.deathMin = GUI->deathMin;
    //timeData.deathMax = GUI->deathMax;
    timeUBO->Update(&timeData,sizeof(timeData));
    posUBO->Update(&posData, sizeof(posData));
    spinUBO->Update(&spinData, sizeof(spinData));
    sizeUBO->Update(&sizeData, sizeof(sizeData));
    accelUBO->Update(&accelData, sizeof(accelData));
    colorUBO->Update(&colorData, sizeof(colorData));
    alphaUBO->Update(&alphaData, sizeof(alphaData));
    velUBO->Update(&velData, sizeof(velData));
    gravUBO->Update(&gravData, sizeof(gravData));
    //GUI->CPUEndTrack();
}

void Application::Render(){
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClear(GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);

    GUI->SetupNewFrame();
    GUI->ShowPerformance(deltaTime, curTime, totalUpdates, updatesPerSecond, totalFrames, NUM_PARTICLES);
    GUI->ShowParticleMenu();
    GUI->Render();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Application::handleKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_W:
            cam->zoom(-0.1f);
            break;
        case GLFW_KEY_S:
            cam->zoom(0.1f);
            break;
        case GLFW_KEY_LEFT:
            cam->rotateHorizontal(1.0f);
            break;
        case GLFW_KEY_RIGHT:
            cam->rotateHorizontal(-1.0f);
            break;
        case GLFW_KEY_UP:
            cam->rotateVertical(1.0f);
            break;
        case GLFW_KEY_DOWN:
            cam->rotateVertical(-1.0f);
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        }

        cam->bind(*shader);
        //cam->bind(*computeShader);
    }
}

Application::~Application(){

    delete particleBuffer;
    delete sizeBuffer;
    delete accelBuffer;
    delete colorBuffer;
    //ADD OTHER BUFFER DELETIONS

    delete shader;
    //delete computeShader;
    delete cam;
    delete GUI;
}
