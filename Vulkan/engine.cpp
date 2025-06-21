#include "engine.h"
#include "instance.h"
#include "logging.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "framebuffer.h"
#include "commands.h"
#include "sync.h"
#include "render_structs.h"


Engine::Engine(const int& width, const int& height, std::shared_ptr<GLFWwindow> window, const bool& debugMode)
	: width(width), height(height), window(window), debugMode(debugMode)
{
	if (debugMode) { std::cout << "Making a graphic engine\n"; }
	create_instance();
	create_device();
	create_pipeline();
	finalize_setup();
	create_assets();
}

void Engine::create_instance()
{
	instance = vkInit::make_instance(debugMode, "VULKAN ENGINE");
	dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
	if (debugMode) debugMessager = vkInit::make_debug_messenger(instance, dldi);

	VkSurfaceKHR c_style_surface;
	if (glfwCreateWindowSurface(instance, window.get(), nullptr, &c_style_surface) != VK_SUCCESS) {
		if (debugMode) { std::cerr << "Failed to abstract the glfw surfad for Vulkan." << std::endl; }
	}
	else if (debugMode) { std::cout << "Successfully abstracted the glfw surface for Vulkan\n"; }
	surface = c_style_surface;
}

enum {
	eGRAPHICS,
	ePRESENTING
};

void Engine::create_device()
{
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);
	device = vkInit::create_logical_device(physicalDevice, surface, debugMode);
	auto queues = vkInit::get_queues(physicalDevice, device, surface, debugMode);
	graphicsQueue = queues[eGRAPHICS];
	presentQueue = queues[ePRESENTING];

	create_swapchain();

	frameNumber = 0;
	// vkInit::query_swapchain_support(physicalDevice, surface, debugMode);
}

void Engine::create_swapchain()
{
	auto bundle = vkInit::create_swapchain(device, physicalDevice, surface, width, height, debugMode);
	swapchain = bundle.swapchain;
	swapchainFrames = bundle.frames;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;

	maxFramesInFlight = swapchainFrames.size();
}

void Engine::recreate_swapchain()
{

	width = 0;
	height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window.get(), &width, &height);
		glfwWaitEvents();
	}

	device.waitIdle();
	cleanup_swapchain();
	create_swapchain();
	create_framebuffers();
	create_frame_sync_objects();
	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	vkInit::make_frame_command_buffers(commandBufferInput, debugMode);

}

void Engine::create_pipeline()
{
	vkInit::GraphicsPipelineInBundle specification = {
		device, "Shaders/vertex.spv", "Shaders/fragment.spv", swapchainExtent, swapchainFormat
	};
	/*specification.device = device;
	specification.vertexFilePath = "Shaders/vertex.spv";
	specification.fragmentFilePath = "Shaders/fragment.spv";
	specification.swapchainExtent = swapchainExtent;*/
	auto output = vkInit::create_graphics_pipeline(specification, debugMode);
	layout = output.layout;
	renderPass = output.renderpass;
	pipeline = output.pipeline;
}

void Engine::finalize_setup()
{
	create_framebuffers();
	
	commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);

	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	mainCommandBuffer = vkInit::make_command_buffer(commandBufferInput, debugMode);
	vkInit::make_frame_command_buffers(commandBufferInput, debugMode);

	create_frame_sync_objects();
	
}

void Engine::create_framebuffers()
{
	vkInit::frameBufferInput frameBufferInput;
	frameBufferInput.device = device;
	frameBufferInput.renderpass = renderPass;
	frameBufferInput.swapchainExtent = swapchainExtent;

	vkInit::make_framebuffers(frameBufferInput, swapchainFrames, debugMode);
}

void Engine::create_frame_sync_objects()
{
	for (auto& frame : swapchainFrames) {
		frame.inFlight = vkInit::make_fence(device, debugMode);
		frame.imageAvailable = vkInit::make_semaphore(device, debugMode);
		frame.renderFinished = vkInit::make_semaphore(device, debugMode);
	}
}

void Engine::create_assets()
{
	meshes = std::make_unique<VertexManagerie>();
	std::vector<float> vertices = { {
		 0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
		 0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
		-0.05f, 0.05f, 0.0f, 1.0f, 0.0f
	} };
	meshTypes type = meshTypes::TRIANGLE;
	meshes->consume(type, vertices);

	vertices = { {
		-0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
		-0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
		 0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
		-0.05f,  0.05f, 1.0f, 0.0f, 0.0f
	} };
	type = meshTypes::SQUARE;
	meshes->consume(type, vertices);

	vertices = { {
		-0.05f, -0.025f, 0.0f, 0.0f, 1.0f,
		-0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		-0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		  0.0f,  -0.05f, 0.0f, 0.0f, 1.0f,
		 0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		-0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		 0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		 0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		 0.05f, -0.025f, 0.0f, 0.0f, 1.0f,
		 0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		 0.02f, -0.025f, 0.0f, 0.0f, 1.0f,
		 0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		 0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		 0.04f,   0.05f, 0.0f, 0.0f, 1.0f,
		  0.0f,   0.01f, 0.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		 0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		  0.0f,   0.01f, 0.0f, 0.0f, 1.0f,
		-0.03f,    0.0f, 0.0f, 0.0f, 1.0f,
		  0.0f,   0.01f, 0.0f, 0.0f, 1.0f,
		-0.04f,   0.05f, 0.0f, 0.0f, 1.0f
	} };
	type = meshTypes::STAR;
	meshes->consume(type, vertices);

	FinalizationChunk finalizeInput;
	finalizeInput.device = device;
	finalizeInput.commandBuffer = mainCommandBuffer;
	finalizeInput.physicalDevice = physicalDevice;
	finalizeInput.queue = graphicsQueue;
	

	meshes->finalize(finalizeInput);
}

void Engine::prepare_scene(vk::CommandBuffer commandBuffer)
{
	auto vertexBuffers = { meshes->vertexBuffer.buffer };
	vk::DeviceSize offsets[] = {0};
	commandBuffer.bindVertexBuffers(0, 1, vertexBuffers.begin(), offsets);
}

void Engine::record_draw_commands(vk::CommandBuffer commandBuffer, uint32_t imageIndex, std::shared_ptr<Scene> scene)
{
	vk::CommandBufferBeginInfo beginInfo = {};

	try {
		commandBuffer.begin(beginInfo);
	}
	catch (vk::SystemError err) { if (debugMode) std::cout << "Failed to begin recording command buffer" << std::endl; }

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapchainFrames[imageIndex].framebuffer;
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent = swapchainExtent;
	vk::ClearValue clearColor = { std::array<float, 4> {.0f, .0f, .0f, 1.0f} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

	prepare_scene(commandBuffer);

	int vertexCount = meshes->sizes.find(meshTypes::TRIANGLE)->second;
	int firstVerex = meshes->offsets.find(meshTypes::TRIANGLE)->second;


	for (const auto& position : scene->trianglePositions) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		vkUtil::ObjectData objectData;
		objectData.model = model;
		commandBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(objectData), & objectData);
		commandBuffer.draw(vertexCount, 1, firstVerex, 0);
	}

	vertexCount = meshes->sizes.find(meshTypes::SQUARE)->second;
	firstVerex = meshes->offsets.find(meshTypes::SQUARE)->second;

	for (const auto& position : scene->squarePositions) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		vkUtil::ObjectData objectData;
		objectData.model = model;
		commandBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(objectData), &objectData);
		commandBuffer.draw(vertexCount, 1, firstVerex, 0);
	}

	vertexCount = meshes->sizes.find(meshTypes::STAR)->second;
	firstVerex = meshes->offsets.find(meshTypes::STAR)->second;

	for (const auto& position : scene->starPositions) {
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		vkUtil::ObjectData objectData;
		objectData.model = model;
		commandBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(objectData), &objectData);
		commandBuffer.draw(vertexCount, 1, firstVerex, 0);
	}
	

	commandBuffer.endRenderPass();
	try {
		commandBuffer.end();
	}
	catch (vk::SystemError err) { if (debugMode) std::cerr << "Failed to finish recording buffer" << std::endl; }
}

void Engine::cleanup_swapchain()
{
	for (auto& frame : swapchainFrames) {
		device.waitIdle(); device.destroySemaphore(frame.imageAvailable);
		device.destroySemaphore(frame.renderFinished);
		device.destroyFence(frame.inFlight);
		device.destroyImageView(frame.imageView);
		device.destroyFramebuffer(frame.framebuffer);
	}

	device.destroySwapchainKHR(swapchain);
}

Engine::~Engine() {


	device.waitIdle();
	if (debugMode) { std::cout << "Goodbye see you!\n"; }

	if (!device) std::cerr << "Device is destroyed!\n";
	// delete meshes;

	device.destroyBuffer(meshes->vertexBuffer.buffer);
	device.freeMemory(meshes->vertexBuffer.bufferMemory);
	
	device.destroyCommandPool(commandPool);

	device.destroyRenderPass(renderPass);
	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(layout);
	
	cleanup_swapchain();
	device.destroy();
	instance.destroySurfaceKHR(surface);
	if (debugMode) instance.destroyDebugUtilsMessengerEXT(debugMessager, nullptr, dldi);
	instance.destroy();
	glfwTerminate();
}
void Engine::render(std::shared_ptr<Scene> scene)
{
	device.waitForFences(1, &swapchainFrames[frameNumber].inFlight, VK_TRUE, UINT64_MAX);

	try {
		auto acquiredImage = device.acquireNextImageKHR(swapchain, UINT64_MAX, swapchainFrames[frameNumber].imageAvailable, nullptr);
		imageIndex = acquiredImage.value;
	}
	catch (vk::OutOfDateKHRError) {
		std::cout << "Recreate swapchain" << std::endl;
		recreate_swapchain(); 
		return;
	}


	auto commandBuffer = swapchainFrames[frameNumber].commandBuffer;
	commandBuffer.reset();

	record_draw_commands(commandBuffer, imageIndex, scene);

	vk::SubmitInfo submitInfo = {};
	vk::Semaphore waitSemaphore[] = { swapchainFrames[frameNumber].imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	vk::Semaphore signalSemaphore[] = { swapchainFrames[frameNumber].renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphore;
	device.resetFences(1, &swapchainFrames[frameNumber].inFlight);
	try {
		graphicsQueue.submit(submitInfo, swapchainFrames[frameNumber].inFlight);
	}
	catch (vk::SystemError err) { if (debugMode) std::cerr << "Failed to submit draw command buffer" << std::endl; }
}

void Engine::present()
{
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &swapchainFrames[frameNumber].renderFinished;
	vk::SwapchainKHR swapchains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	vk::Result present;
	try {
		present = presentQueue.presentKHR(presentInfo);
	}
	catch (vk::OutOfDateKHRError err) {
		present = vk::Result::eErrorOutOfDateKHR;
	}

	if (present == vk::Result::eErrorOutOfDateKHR || present == vk::Result::eSuboptimalKHR) {
		std::cout << "Recreate swapchain" << std::endl;
		recreate_swapchain();

		return;
	}

	frameNumber = (frameNumber + 1) % maxFramesInFlight;
}
