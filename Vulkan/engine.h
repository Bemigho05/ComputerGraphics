#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "config.h"
#include "frame.h"
#include "scene.h"
#include "vertex_managerie.h"
#include "image.h"




class Engine
{
public:
	Engine(const int& width, const int& height, std::shared_ptr<GLFWwindow> window, const bool& debugMode);
	~Engine();

	void render(std::shared_ptr<Scene> scene);
	void present();
private:

	bool debugMode;

	int width{ 1280};
	int height{ 760 };
	 // std::unique_ptr<GLFWwindow, decltype(destroyGLFWwidow)> window{ nullptr, destroyGLFWwidow };

	std::shared_ptr<GLFWwindow> window{ nullptr };
	vk::Instance instance{ nullptr };
	vk::DebugUtilsMessengerEXT debugMessager{ nullptr };
	vk::DispatchLoaderDynamic dldi;
	vk::SurfaceKHR surface{};


	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue;

	vk::SwapchainKHR swapchain;
	std::vector<vkUtil::SwapChainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;

	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderPass;
	vk::Pipeline pipeline;

	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandBuffer;

	vk::Fence inFlightFence;
	vk::Semaphore imageAvailable, renderFinished;

	uint32_t maxFramesInFlight, frameNumber;
	uint32_t imageIndex;

	vk::DescriptorSetLayout frameSetLayout;
	vk::DescriptorPool frameDescriptorPool;

	vk::DescriptorSetLayout meshSetLayout;
	vk::DescriptorPool meshDescriptorPool;


	std::unique_ptr<VertexManagerie> meshes;
	std::unordered_map<meshTypes, std::unique_ptr<vkImage::Texture>> materials;

	
	
	void create_instance();
	void create_device();
	void create_swapchain();
	void recreate_swapchain();
	void create_descriptor_set_layouts();
	void create_pipeline();
	void finalize_setup();
	void create_framebuffers();
	void create_frame_resources();
	void create_assets();
	void prepare_scene(vk::CommandBuffer commandBuffer);
	void prepare_frame(uint32_t imageIndex, std::shared_ptr<Scene> scene);


	void render_objects(vk::CommandBuffer commandBuffer, meshTypes objectType, uint32_t& startInstance, uint32_t instanceCount);
	void record_draw_commands(vk::CommandBuffer commandBUffer, uint32_t imageIndex, std::shared_ptr<Scene> scene);
	void cleanup_swapchain();
};

