#pragma once

#include "config.h"
#include "engine.h"
#include "scene.h"

class App {
private:
	std::unique_ptr<Engine> graphicsEngine;
	std::shared_ptr<GLFWwindow> window;
	std::shared_ptr<Scene> scene;

	double lastTime, currentTime;
	int numFrames;
	float frameTime;

	void build_glfw_window(const int& width, const int& height, const bool& debugMode);

	void calculateFrameRate();

public:
	App(const int& width, const int& height, const bool& debug);
	~App();
	void run();
};

