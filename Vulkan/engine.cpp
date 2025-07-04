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
#include "descriptor.h"



Engine::Engine(const int& width, const int& height, std::shared_ptr<GLFWwindow> window, const bool& debugMode)
	: width(width), height(height), window(window), debugMode(debugMode)
{
	if (debugMode) { std::cout << "Making a graphic engine\n"; }
	create_instance();
	create_device();
	create_descriptor_set_layouts();
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
	physicalDevice = vkInit::choose_physical_device(instance);
	device = vkInit::create_logical_device(physicalDevice, surface);
	auto queues = vkInit::get_queues(physicalDevice, device, surface);
	graphicsQueue = queues[eGRAPHICS];
	presentQueue = queues[ePRESENTING];

	create_swapchain();

	frameNumber = 0;
	// vkInit::query_swapchain_support(physicalDevice, surface, debugMode);
}

void Engine::create_swapchain()
{
	auto bundle = vkInit::create_swapchain(device, physicalDevice, surface, width, height);
	swapchain = bundle.swapchain;
	swapchainFrames = bundle.frames;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;

	maxFramesInFlight = swapchainFrames.size();

	for (auto& frame : swapchainFrames) {
		frame.device = device;
		frame.physicalDevice = physicalDevice;
		frame.width = swapchainExtent.width;
		frame.height = swapchainExtent.height;

		frame.create_depth_resources();
	}
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
	create_frame_resources();
	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	vkInit::create_frame_command_buffers(commandBufferInput);

}

void Engine::create_descriptor_set_layouts()
{
	vkInit::descriptorSetLayoutData bindings;
	bindings.count = 2;

	bindings.indices.push_back(0);
	bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
	bindings.counts.push_back(1);
	bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

	bindings.indices.push_back(1);
	bindings.types.push_back(vk::DescriptorType::eStorageBuffer);
	bindings.counts.push_back(1);
	bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

	frameSetLayout = vkInit::create_descriptor_set_layout(device, bindings);

	bindings.count = 1;

	bindings.indices.clear();
	bindings.indices.push_back(0);

	bindings.types.clear();
	bindings.types.push_back(vk::DescriptorType::eCombinedImageSampler);

	bindings.counts.clear();
	bindings.counts.push_back(1);

	bindings.stages.clear();
	bindings.stages.push_back(vk::ShaderStageFlagBits::eFragment);


	meshSetLayout = vkInit::create_descriptor_set_layout(device, bindings);
}

void Engine::create_pipeline()
{
	vkInit::GraphicsPipelineInBundle specification = {};

	specification.device = device;
	specification.vertexFilePath = "Shaders/vertex.spv";
	specification.fragmentFilePath = "Shaders/fragment.spv";
	specification.swapchainExtent = swapchainExtent;
	specification.swapchainImageFormat = swapchainFormat;
	specification.depthFormat = swapchainFrames[0].depthFormat;
	specification.descriptorSetLayouts = { frameSetLayout, meshSetLayout };

	auto output = vkInit::create_graphics_pipeline(specification);
	pipelineLayout = output.layout;
	renderPass = output.renderpass;
	pipeline = output.pipeline;
}

void Engine::finalize_setup()
{
	create_framebuffers();
	
	commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);

	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	mainCommandBuffer = vkInit::make_command_buffer(commandBufferInput);
	vkInit::create_frame_command_buffers(commandBufferInput);

	create_frame_resources();
	
}

void Engine::create_framebuffers()
{
	vkInit::frameBufferInput frameBufferInput;
	frameBufferInput.device = device;
	frameBufferInput.renderpass = renderPass;
	frameBufferInput.swapchainExtent = swapchainExtent;

	vkInit::make_framebuffers(frameBufferInput, swapchainFrames);
}

void Engine::create_frame_resources()
{
	vkInit::descriptorSetLayoutData bindings;
	bindings.count = 2;
	bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
	bindings.types.push_back(vk::DescriptorType::eStorageBuffer);

	frameDescriptorPool = vkInit::create_descriptor_pool(device, static_cast<uint32_t>(swapchainFrames.size()), bindings);
	

	for (auto& frame : swapchainFrames) {
		frame.inFlight = vkInit::make_fence(device);
		frame.imageAvailable = vkInit::make_semaphore(device);
		frame.renderFinished = vkInit::make_semaphore(device);

		frame.create_descriptor_resources();
		frame.descriptorSet = vkInit::allocate_descriptor_set(device, frameDescriptorPool, frameSetLayout);
	}
}

void Engine::create_assets()
{
	meshes = std::make_unique<VertexManagerie>();

	std::vector<float> vertices = { {
		 0.0f, -0.1f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f, //0
		 0.1f,  0.1f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //1
		-0.1f,  0.1f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f  //2
	} };
	std::vector<uint32_t> indices = { {
			0, 1, 2
	} };
	meshTypes type = meshTypes::TRIANGLE;
	meshes->consume(type, vertices, indices);
	
	vertices = { {
		-0.1f,  0.1f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, //0
		-0.1f, -0.1f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, //1
		 0.1f, -0.1f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //2
		 0.1f,  0.1f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, //3
	} };
	indices = { {
			0, 1, 2,
			2, 3, 0
	} };
	type = meshTypes::SQUARE;
	meshes->consume(type, vertices, indices);

	vertices = { {
		 -0.1f, -0.05f, 1.0f, 1.0f, 1.0f, 0.0f, 0.25f, //0
		-0.04f, -0.05f, 1.0f, 1.0f, 1.0f, 0.3f, 0.25f, //1
		-0.06f,   0.0f, 1.0f, 1.0f, 1.0f, 0.2f,  0.5f, //2
		  0.0f,  -0.1f, 1.0f, 1.0f, 1.0f, 0.5f,  0.0f, //3
		 0.04f, -0.05f, 1.0f, 1.0f, 1.0f, 0.7f, 0.25f, //4
		  0.1f, -0.05f, 1.0f, 1.0f, 1.0f, 1.0f, 0.25f, //5
		 0.06f,   0.0f, 1.0f, 1.0f, 1.0f, 0.8f,  0.5f, //6
		 0.08f,   0.1f, 1.0f, 1.0f, 1.0f, 0.9f,  1.0f, //7
		  0.0f,  0.02f, 1.0f, 1.0f, 1.0f, 0.5f,  0.6f, //8
		-0.08f,   0.1f, 1.0f, 1.0f, 1.0f, 0.1f,  1.0f  //9
	} };
	indices = { {
			0, 1, 2,
			1, 3, 4,
			2, 1, 4,
			4, 5, 6,
			2, 4, 6,
			6, 7, 8,
			2, 6, 8,
			2, 8, 9
	} };
	type = meshTypes::STAR;
	meshes->consume(type, vertices, indices);

	FinalizationChunk finalizationInfo;
	finalizationInfo.device = device;
	finalizationInfo.physicalDevice = physicalDevice;
	finalizationInfo.commandBuffer = mainCommandBuffer;
	finalizationInfo.queue = graphicsQueue;
	meshes->finalize(finalizationInfo);

	// Materials
	std::unordered_map<meshTypes, std::string> filenames = {
		{meshTypes::TRIANGLE, "textures/face.jpg"},
		{meshTypes::SQUARE, "textures/haus.jpg"},
		{meshTypes::STAR, "textures/noroi.png"}
	};

	vkInit::descriptorSetLayoutData bindings;
	bindings.count = 1;
	bindings.types.push_back(vk::DescriptorType::eCombinedImageSampler);
	meshDescriptorPool = vkInit::create_descriptor_pool(device, static_cast<uint32_t>(filenames.size()), bindings);

	// make a descriptor pool
	vkImage::TextureInput textureInfo;
	textureInfo.commandBuffer = mainCommandBuffer;
	textureInfo.device = device;
	textureInfo.queue = graphicsQueue;
	textureInfo.physicalDevice = physicalDevice;
	textureInfo.layout = meshSetLayout; // TODO: change later!!
	textureInfo.descriptorPool = meshDescriptorPool; // TODO: change later!!

	for (const auto& [object, filename] : filenames) {
		textureInfo.filename = filename;
		materials[object] = std::make_unique<vkImage::Texture>(textureInfo);
	}
}

void Engine::prepare_scene(vk::CommandBuffer commandBuffer)
{
	auto vertexBuffers = { meshes->vertexBuffer.buffer };
	vk::DeviceSize offsets[] = {0};
	commandBuffer.bindVertexBuffers(0, 1, vertexBuffers.begin(), offsets);
	commandBuffer.bindIndexBuffer(meshes->indexBuffer.buffer, 0, vk::IndexType::eUint32);
}

void Engine::prepare_frame(uint32_t imageIndex, std::shared_ptr<Scene> scene)
{
	auto& frame = swapchainFrames[imageIndex];

	glm::vec3 eye = { 1.0f, 0.0f, -1.0f };
	glm::vec3 center = { 0.0f, 0.0f, 0.0f };
	glm::vec3 up = { 0.0f, 0.0f, -1.0f };
	glm::mat4 view = glm::lookAt(eye, center, up);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(swapchainExtent.width) / static_cast<float>(swapchainExtent.height), 0.1f, 10.0f);
	projection[1][1] *= -1;

	frame.cameraData.view = view;
	frame.cameraData.projection = projection;
	frame.cameraData.viewProjection = projection * view;

	memcpy(frame.cameraDataWriteLocation, &(frame.cameraData), sizeof(vkUtil::UBO));

	size_t i = 0;
	for (const auto& position : scene->trianglePositions) {
		frame.modelTransforms[i++] = glm::translate(glm::mat4(1.0f), position);
	}
	for (const auto& position : scene->squarePositions) {
		frame.modelTransforms[i++] = glm::translate(glm::mat4(1.0f), position);
	}
	for (const auto& position : scene->starPositions) {
		frame.modelTransforms[i++] = glm::translate(glm::mat4(1.0f), position);
	}

	memcpy(frame.modelTransformsWriteLocation, frame.modelTransforms.data(), i * sizeof(glm::mat4));

	frame.write_descriptor_set();
}

void Engine::render_objects(vk::CommandBuffer commandBuffer, meshTypes objectType, uint32_t& startInstance, uint32_t instanceCount)
{
	int indexCount = meshes->indexCounts.find(objectType)->second;
	int firstIndex = meshes->firstIndices.find(objectType)->second;
	materials[objectType]->use(commandBuffer, pipelineLayout);
	commandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, 0, startInstance);
	startInstance += instanceCount;
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

	// Clear color value to black

	// Clear depth value to 1.0f (max depth)
	
	

	vk::ClearValue colorClear;
	std::array<float, 4> colors = { 1.0f, 0.5f, 0.25f, 1.0f };
	colorClear.color = vk::ClearColorValue(colors);
	vk::ClearValue depthClear;

	depthClear.depthStencil = vk::ClearDepthStencilValue({ 1.0f, 0 });
	std::vector<vk::ClearValue> clearValues = { {colorClear, depthClear} };

	renderPassInfo.clearValueCount = clearValues.size();
	renderPassInfo.pClearValues = clearValues.data();

	commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, swapchainFrames[imageIndex].descriptorSet, nullptr);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

	prepare_scene(commandBuffer);

	uint32_t startInstance = 0;
	uint32_t instanceCount = static_cast<uint32_t>(scene->trianglePositions.size());
	render_objects(commandBuffer, meshTypes::TRIANGLE, startInstance, instanceCount);

	instanceCount = static_cast<uint32_t>(scene->squarePositions.size());
	render_objects(commandBuffer, meshTypes::SQUARE, startInstance, instanceCount);
	
	instanceCount = static_cast<uint32_t>(scene->starPositions.size());
	render_objects(commandBuffer, meshTypes::STAR, startInstance, instanceCount);
	
	commandBuffer.endRenderPass();
	try {
		commandBuffer.end();
	}
	catch (vk::SystemError err) { if (debugMode) std::cerr << "Failed to finish recording buffer" << std::endl; }
}

void Engine::cleanup_swapchain()
{
	for (auto& frame : swapchainFrames) {
		frame.destroy();
	}

	device.destroySwapchainKHR(swapchain);
	device.destroyDescriptorPool(frameDescriptorPool);
	
}

Engine::~Engine() {


	device.waitIdle();
	if (debugMode) { std::cout << "Goodbye see you!\n"; }

	if (!device) std::cerr << "Device is destroyed!\n";

	// delete meshes;
	meshes = nullptr;
	
	// delete textures
	for (auto& [key, texture] : materials) texture = nullptr;
	

	device.destroyCommandPool(commandPool);

	device.destroyRenderPass(renderPass);
	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(pipelineLayout);
	
	cleanup_swapchain();
	device.destroyDescriptorSetLayout(frameSetLayout);
	device.destroyDescriptorSetLayout(meshSetLayout);
	
	device.destroyDescriptorPool(meshDescriptorPool);
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

	prepare_frame(imageIndex, scene);

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
