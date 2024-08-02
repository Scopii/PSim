#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Types.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <numeric>
#include <algorithm>

class GUIElement {
public:
	GUIElement(GLFWwindow* window);
	~GUIElement();
	void Init(GLFWwindow* window);
	void SetupNewFrame();
	void ShowPerformance(double& deltaTime, double& curentTime, unsigned long long& totalUpdates, float& updatesPerSecond, unsigned long long& totalFrames, size_t particleCount);
	void ShowParticleMenu();
	void Render();

	void GPUTrack();
	void GPUEndTrack();
	void CPUTrack();
	void CPUEndTrack();
	void ComputeTrack();
	void ComputeEndTrack();
	void setPointers(TimeData* time, DirecAngleVecData* vel, RadiusVecData* pos, DirecAngleVecData* grav, SpinData* spin,
								RandAttributeGradient* size, RandAttributeGradient* accel, RandAttributeGradient* color, RandAttributeGradient* alpha);

	float deathMin = 2.0f;
	float deathMax = 2.0f;

	TimeData* timeData;
	DirecAngleVecData* velData;
	RadiusVecData* posData;
	DirecAngleVecData* gravData;
	SpinData* spinData;
	RandAttributeGradient* sizeData;
	RandAttributeGradient* accelData;
	RandAttributeGradient* colorData;
	RandAttributeGradient* alphaData;

private:
	GLFWwindow* GUIwindow{};
	static constexpr int HISTORY_SIZE = 100;
	static constexpr int HISTORY_SIZE_EXTENDED = 1000;
	float fpsAvg{}, fps1{}, fps01{};
	float frameTimeAvg{}, frameTime1{}, frameTime01{};
	float gpuTimeAvg{}, gpuTime1{}, gpuTime01{};
	float cpuTimeAvg{}, cpuTime1{}, cpuTime01{};
	float computeTimeAvg{}, computeTime1{}, computeTime01{};
	vectorArray<float> fpsHistory;
	vectorArray<float> frameTimeHistory;
	vectorArray<float> gpuTimeHistory;
	vectorArray<float> cpuTimeHistory;
	vectorArray<float> computeTimeHistory;

	// GPU Time
	GLuint gpuQuery{};
	float gpuTime{};
	// Compute Time
	GLuint computeQuery{};
	float computeTime{};

	// CPU Time
	std::chrono::time_point<std::chrono::high_resolution_clock> cpuStart;
	std::chrono::time_point<std::chrono::high_resolution_clock> cpuEnd;
	float cpuTime{};

	static void updateHistory(std::vector<float>& history, float newValue, float& avg, float& percentile1, float& percentile01, bool isFrameTime);
};