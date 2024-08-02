#include "GUIElement.h"

#include <string>

#include "imgui/imgui_internal.h"

GUIElement::GUIElement(GLFWwindow* window) {
    Init(window);
}

GUIElement::~GUIElement() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteQueries(1, &gpuQuery);
    glDeleteQueries(1, &computeQuery);

    //Settings

}

void GUIElement::setPointers(TimeData* time, DirecAngleVecData* vel, RadiusVecData* pos, DirecAngleVecData* grav, SpinData* spin,
                             RandAttributeGradient* size, RandAttributeGradient* accel, RandAttributeGradient* color, RandAttributeGradient* alpha) {
    timeData = time;
    velData = vel;
    posData = pos;
    gravData = grav;
    spinData = spin;
    sizeData = size;
    accelData = accel;
    colorData = color;
    alphaData = alpha;
}

void GUIElement::updateHistory(std::vector<float>& history, const float newValue, float& avg, float& percentile1, float& percentile01, const bool isFrameTime) {
    if (history.size() >= HISTORY_SIZE_EXTENDED) {
        history.erase(history.begin());
    }
    history.push_back(newValue);

    avg = std::accumulate(history.begin(), history.end(), 0.0f) / history.size();

    std::vector<float> sortedHistory = history;
    std::sort(sortedHistory.begin(), sortedHistory.end());

    size_t size = sortedHistory.size();
    if (isFrameTime) {
        // For frame times, we want the highest values (lowest FPS)
        percentile1 = sortedHistory[static_cast<size_t>(size * 0.99)];
        percentile01 = sortedHistory[static_cast<size_t>(size * 0.999)];
    }
    else {
        // For other metrics, we want the highest values
        percentile1 = sortedHistory[static_cast<size_t>(size * 0.99)];
        percentile01 = sortedHistory[static_cast<size_t>(size * 0.999)];
    }
}

void GUIElement::Init(GLFWwindow* window) {
    fpsHistory.reserve(HISTORY_SIZE);
    frameTimeHistory.reserve(HISTORY_SIZE);
    gpuTimeHistory.reserve(HISTORY_SIZE);
    cpuTimeHistory.reserve(HISTORY_SIZE);
    computeTimeHistory.reserve(HISTORY_SIZE);

    glGenQueries(1, &gpuQuery);
    glGenQueries(1, &computeQuery);

    GUIwindow = window;
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.Fonts->AddFontFromFileTTF("src/vendor/imgui/Roboto-Medium.ttf", 20.0f);
    //io.Fonts->Build();
    //io.FontGlobalScale = 0.5f;
    // 
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(GUIwindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;
    style.WindowPadding = ImVec2(0, 0);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;

}

void GUIElement::SetupNewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUIElement::ShowPerformance(double &deltaTime, double &currentTime, unsigned long long &totalUpdates, float &updatesPerSecond, unsigned long long& totalFrames, size_t particleCount) {
    // (Performance) Transparent overlay window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::Begin("Overlay", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::SetCursorPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 150)); // Position at bottom left

    // Application Info
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Runtime: %.2f | Updates: %llu | Update Rate: %.f", currentTime, totalUpdates, updatesPerSecond);

    // Frametime
    float fps = ImGui::GetIO().Framerate;
    float frameTime = deltaTime * 1000.0f;
    updateHistory(frameTimeHistory, frameTime, frameTimeAvg, frameTime1, frameTime01, true);
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Frame Time: %.2f ms | 1%%: %.2f | 0.1%%: %.2f", frameTimeAvg, frameTime1, frameTime01);

    // FPS
    fpsAvg = 1000.0f / frameTimeAvg;
    fps1 = 1000.0f / frameTime1;
    fps01 = 1000.0f / frameTime01;
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "FPS: %.2f | 1%%: %.2f | 0.1%%: %.2f | Total: %llu", fpsAvg, fps1, fps01, totalFrames);

    updateHistory(gpuTimeHistory, gpuTime, gpuTimeAvg, gpuTime1, gpuTime01, false);
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "GPU Time: %.5f ms | 1%%: %.5f | 0.1%%: %.5f", gpuTimeAvg, gpuTime1, gpuTime01);

    updateHistory(cpuTimeHistory, cpuTime, cpuTimeAvg, cpuTime1, cpuTime01, false);
    ImGui::TextColored(ImVec4(0.0f, 0.76f, 1.0f, 1.0f), "CPU Time: %.5f ms | 1%%: %.5f | 0.1%%: %.5f", cpuTimeAvg, cpuTime1, cpuTime01);

    // I think this is Compute Shader Time?
    // Dispatch compute shader
    updateHistory(computeTimeHistory, computeTime, computeTimeAvg, computeTime1, computeTime01, false);
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.5f, 1.0f), "Compute Time: %.5f ms | 1%%: %.5f | 0.1%%: %.5f", computeTimeAvg, computeTime1, computeTime01);

    ImGui::Text("Particle Count %llu", particleCount); //llu

    ImGui::End();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
}

void GUIElement::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIElement::CPUTrack() {
    cpuStart = std::chrono::high_resolution_clock::now();
}

void GUIElement::CPUEndTrack() {
    cpuEnd = std::chrono::high_resolution_clock::now();
    cpuTime = std::chrono::duration<float, std::milli>(cpuEnd - cpuStart).count();
}

void GUIElement::GPUTrack(){
    glBeginQuery(GL_TIME_ELAPSED, gpuQuery);
}

void GUIElement::GPUEndTrack(){
    glEndQuery(GL_TIME_ELAPSED);
    GLuint64 gpuTimeNs;
    glGetQueryObjectui64v(gpuQuery, GL_QUERY_RESULT, &gpuTimeNs);
    gpuTime = gpuTimeNs / 1000000.0f;
}

void GUIElement::ComputeTrack() {
    glBeginQuery(GL_TIME_ELAPSED, computeQuery);
}

void GUIElement::ComputeEndTrack() {
    glEndQuery(GL_TIME_ELAPSED);
    GLuint64 computeTimeNs;
    glGetQueryObjectui64v(computeQuery, GL_QUERY_RESULT, &computeTimeNs);
    computeTime = computeTimeNs / 1000000.0f;
}

// Size Gradient
void doubleSlider(const char* Name,float min = 0.0f, float max = 100.0f, float low = 25.0f, float up = 75.0f)
{
    static float minValue = min;
    static float maxValue = max;
    static float lowerValue = low;
    static float upperValue = up;

    ImGui::PushID(Name);
    ImGui::BeginGroup();
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 40);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    const ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    const float width = canvas_size.x - 20;

    // Draw the base line
    draw_list->AddLine(
        ImVec2(canvas_pos.x, canvas_pos.y + 10),
        ImVec2(canvas_pos.x + width, canvas_pos.y + 10),
        ImGui::GetColorU32(ImGuiCol_FrameBg), 4.0f);

    // Calculate positions
    float lowerPos = ((lowerValue - minValue) / (maxValue - minValue)) * width;
    float upperPos = ((upperValue - minValue) / (maxValue - minValue)) * width;

    // Draw and handle lower slider
    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + lowerPos - 20, canvas_pos.y));
    ImGui::InvisibleButton("lower", ImVec2(40, 20));
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
        lowerValue += ImGui::GetIO().MouseDelta.x * ((maxValue - minValue) / width);
        lowerValue = ImClamp(lowerValue, minValue, upperValue);
    }
    draw_list->AddCircleFilled(
        ImVec2(canvas_pos.x + lowerPos, canvas_pos.y + 10),
        10, ImGui::GetColorU32(ImGuiCol_SliderGrab));

    // Draw and handle upper slider
    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + upperPos - 20, canvas_pos.y));
    ImGui::InvisibleButton("upper", ImVec2(40, 20));
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
        upperValue += ImGui::GetIO().MouseDelta.x * ((maxValue - minValue) / width);
        upperValue = ImClamp(upperValue, lowerValue, maxValue);
    }
    draw_list->AddCircleFilled(
        ImVec2(canvas_pos.x + upperPos, canvas_pos.y + 10),
        10, ImGui::GetColorU32(ImGuiCol_SliderGrab));

    ImGui::PopStyleVar();
    ImGui::EndGroup();
    ImGui::PopID();

    ImGui::Text("Range: %.2f - %.2f", lowerValue, upperValue);
}

void GUIElement::ShowParticleMenu() {
    ImGui::Begin("Particle Settings");

    // Deathtime
    ImGui::Text("Time");
    ImGui::InputFloat("Death Time Min", &timeData->deathMin, 0.1f, 1.0f, "%.4f");
    ImGui::InputFloat("Death Time Max", &timeData->deathMax, 0.1f, 1.0f, "%.4f");

    // Velocity
    ImGui::Text("Velocity");

    float tempDirec[3] = {velData->direction.vec.x,velData->direction.vec.y,velData->direction.vec.z};
    ImGui::InputFloat3("Normalized Direction", tempDirec, "%.4f");
    velData->direction.vec = vec3(tempDirec[0],tempDirec[1],tempDirec[2]);

    ImGui::SliderFloat("Angle", &velData->angleDegrees, 0, 360);
    ImGui::InputFloat("Radius Min", &velData->minRadius, 0.1f, 1.0f, "%.4f");
    ImGui::InputFloat("Radius Max", &velData->maxRadius, 0.1f, 1.0f, "%.4f");

    float tempMult[3] = {velData->xMult,velData->yMult,velData->zMult};
    ImGui::InputFloat3("Axis Multiplier", tempMult, "%.4f");
    velData->xMult = tempMult[0];
    velData->yMult = tempMult[1];
    velData->zMult = tempMult[2];

    // Position (DOESNT WORK)
    ImGui::Text("Position");
    float tempOrigin[3] = {posData->posOrigin.vec.x,posData->posOrigin.vec.y,posData->posOrigin.vec.z};
    ImGui::InputFloat3("Origin", tempOrigin, "%.4f");
    posData->posOrigin.vec = vec3(tempOrigin[0],tempOrigin[1],tempOrigin[2]);
    ImGui::InputFloat("Pos Radius Min", &posData->posRadiusMin, 0.1f, 1.0f, "%.4f");
    ImGui::InputFloat("Pos Radius Max", &posData->posRadiusMax, 0.1f, 1.0f, "%.4f");

    // Gravity
    ImGui::Text("Gravity");

    float tempDirecGrav[3] = {gravData->direction.vec.x,gravData->direction.vec.y,gravData->direction.vec.z};
    ImGui::InputFloat3("Normalized Direction Gravity", tempDirec, "%.4f");
    gravData->direction.vec = vec3(tempDirec[0],tempDirec[1],tempDirec[2]);

    ImGui::SliderFloat("Gravity Angle", &gravData->angleDegrees, 0, 360);
    ImGui::InputFloat("Grav Radius Min", &gravData->minRadius, 0.1f, 1.0f, "%.4f");
    ImGui::InputFloat("Grav Radius Max", &gravData->maxRadius, 0.1f, 1.0f, "%.4f");

    float tempMultGrav[3] = {gravData->xMult,gravData->yMult,gravData->zMult};
    ImGui::InputFloat3("Normalized Direction Grav", tempMultGrav, "%.4f");
    gravData->xMult = tempMultGrav[0];
    gravData->yMult = tempMultGrav[1];
    gravData->zMult = tempMultGrav[2];

    // Spin
    ImGui::Text("Rotations");
    float rotMin = (spinData->rotMin/TWO_PI)*360;
    ImGui::SliderFloat("Rot Min", &rotMin, 0, 360);
    spinData->rotMin = (rotMin/360)*TWO_PI;
    float rotMax = (spinData->rotMax/TWO_PI)*360;;
    ImGui::SliderFloat("Rot Max", &rotMax, 0, 360);
    spinData->rotMax = (rotMax/360)*TWO_PI;

    // Rot
    ImGui::Text("Rotations");
    float spinMin = (spinData->spinMin/TWO_PI)*360;
    ImGui::SliderFloat("Rot Min", &spinMin, -360, 360);
    spinData->spinMin = (spinMin/360)*TWO_PI;
    float spinMax = (spinData->spinMax/TWO_PI)*360;;
    ImGui::SliderFloat("Rot Max", &spinMax, -360, 360);
    spinData->spinMax = (spinMax/360)*TWO_PI;

    doubleSlider("Size1", 0,100,2,3);
    doubleSlider("Size2", 0,1,0.2,0.4);

    ImGui::End();
}

