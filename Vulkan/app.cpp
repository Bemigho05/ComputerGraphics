#include "app.h"

static auto destroyGLFWwidow = [](GLFWwindow* window) {
	if (window) {
		glfwDestroyWindow(window);
#ifndef NDEBUG
		std::cout << "Window Destroyed" << std::endl;
#endif
	}
};

void App::build_glfw_window(const int& width, const int& height, const bool& debugMode)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window.reset(glfwCreateWindow(width, height, "VULKAN ENGINE", nullptr, nullptr), destroyGLFWwidow); 
	// window.reset(glfwCreateWindow(width, height, "VULKAN ENGINE", nullptr, nullptr));
	if (window) { if (debugMode) std::cout << "GLFWwindow created, width: " << width << ", height: " << height << "\n"; }
	else { if (debugMode) { std::cerr << "Failed creating GLFWwindow\n"; } }
}

void App::calculateFrameRate()
{
	currentTime = glfwGetTime();
	auto delta = currentTime - lastTime;

	if (delta >= 1) {
		auto framerate = std::max(1, int(numFrames / delta));
		std::stringstream title{}; title << "Running at " << framerate << " fps.";
		glfwSetWindowTitle(window.get(), title.str().c_str());
		lastTime = currentTime;
		numFrames = -1;
		frameTime = float(1000.0 / framerate);
	}

	++numFrames;
}

App::App(const int& width, const int& height, const bool& debug)
{
	build_glfw_window(width, height, debug);
	graphicsEngine = std::make_unique<Engine>(width, height, window, debug);
	scene = std::make_shared<Scene>();
}

App::~App()
{
}

void App::run()
{
	while (!glfwWindowShouldClose(window.get())) {
		glfwPollEvents();
		graphicsEngine->render(scene);
		graphicsEngine->present();
		calculateFrameRate();
	}
}
