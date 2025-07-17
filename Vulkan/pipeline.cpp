#include "pipeline.h"
#include "shaders.h"
#include "render_structs.h"
#include "mesh.h"

void vkInit::PipelineBuilder::resetVertexFormat()
{
	vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;
}

void vkInit::PipelineBuilder::resetShaderModules()
{
	if (vertexShader) { device.destroyShaderModule(vertexShader); }
	if (fragmentShader) { device.destroyShaderModule(fragmentShader); }
	shaderStages.clear();
}

void vkInit::PipelineBuilder::resetRenderpassAttachments()
{
	attachmentDescriptions.clear();
	attachmentReferences.clear();
}

vk::AttachmentDescription vkInit::PipelineBuilder::createRenderpassAttachment(const vk::Format& format, vk::ImageLayout finalLayout)
{
	vk::AttachmentDescription attachment = {};

	attachment.flags = vk::AttachmentDescriptionFlags();
	attachment.format = format;
	attachment.samples = vk::SampleCountFlagBits::e1;
	attachment.loadOp = vk::AttachmentLoadOp::eClear;
	attachment.storeOp = vk::AttachmentStoreOp::eStore;
	attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	attachment.initialLayout = vk::ImageLayout::eUndefined;
	attachment.finalLayout = finalLayout;

	return attachment;
}

vk::AttachmentReference vkInit::PipelineBuilder::createAttachmentReference(uint32_t attachmentIndex, vk::ImageLayout layout)
{

	vk::AttachmentReference attachmentRef = {};
	attachmentRef.attachment = attachmentIndex;
	attachmentRef.layout = layout;
	return attachmentRef;
}

void vkInit::PipelineBuilder::configureInputAssembly()
{
	inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
	inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
}

vk::PipelineShaderStageCreateInfo vkInit::PipelineBuilder::createShaderInfo(const vk::ShaderModule& shaderModule, const vk::ShaderStageFlagBits& stage)
{
	vk::PipelineShaderStageCreateInfo shaderInfo = {};
	shaderInfo.flags = vk::PipelineShaderStageCreateFlags();
	shaderInfo.stage = stage;
	shaderInfo.module = shaderModule;
	shaderInfo.pName = "main";
	return shaderInfo;
}

vk::PipelineViewportStateCreateInfo vkInit::PipelineBuilder::createViewportState()
{
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.0;
	viewport.maxDepth = 1.0f;

	scissor.offset.x = 0.0f;
	scissor.offset.y = 0.0f;
	scissor.extent = swapchainExtent;

	viewportState.flags = vk::PipelineViewportStateCreateFlags();
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	return viewportState;
}

void vkInit::PipelineBuilder::configureRasterization()
{
	rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
	rasterizer.depthBiasClamp = VK_FALSE;
}

void vkInit::PipelineBuilder::configureMultisampling()
{
	multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
}

void vkInit::PipelineBuilder::configureColorBlending()
{
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_FALSE;

	colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
}

vk::PipelineLayout vkInit::PipelineBuilder::createPipelineLayout()
{
	vk::PipelineLayoutCreateInfo layoutInfo;
	layoutInfo.flags = vk::PipelineLayoutCreateFlags();
	layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	layoutInfo.pSetLayouts = descriptorSetLayouts.data();
	layoutInfo.pushConstantRangeCount = 0;
	
	// vk::PushConstantRange pushConstantInfo = create_push_constant_info();
	// layoutInfo.pPushConstantRanges = &pushConstantInfo;

	try {
		return device.createPipelineLayout(layoutInfo);
	}
	catch (vk::SystemError err) {
#ifndef NDEBUG
		std::cerr << "Failed to create pipeline layout!" << std::endl;
#endif
	}
}


vk::RenderPass vkInit::PipelineBuilder::createRenderpass()
{
	flattenedAttachmentDescriptions.clear();
	flattenedAttachmentReferences.clear();

	size_t attachmentCount = attachmentDescriptions.size();
	flattenedAttachmentDescriptions.resize(attachmentCount);
	flattenedAttachmentReferences.resize(attachmentCount);

	for (size_t i = 0; i < attachmentCount; ++i) {
		flattenedAttachmentDescriptions[i] = attachmentDescriptions[i];
		flattenedAttachmentReferences[i] = attachmentReferences[i];
	}

	auto subpass = createSubpass(flattenedAttachmentReferences);
	auto renderpassInfo = createRenderpassInfo(flattenedAttachmentDescriptions, subpass);

	try {
		return device.createRenderPass(renderpassInfo);
	}
	catch(vk::SystemError err) {
#ifndef NDEBUG
		std::cerr << "Failed to create renderpass!";
#endif // !NDEBUG
	}
}

vk::SubpassDescription vkInit::PipelineBuilder::createSubpass(const std::vector<vk::AttachmentReference>& attachmentRefs)
{
	vk::SubpassDescription subpass = {};
	subpass.flags = vk::SubpassDescriptionFlags();
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachmentRefs[0];
	if (attachmentRefs[0].layout == vk::ImageLayout::ePresentSrcKHR) {
#ifndef NDEBUG
		std::cerr << "Present" << std::endl;
#endif // !NDEBUG

	}
	subpass.pDepthStencilAttachment = &attachmentRefs[1];
	return subpass;
}

vk::RenderPassCreateInfo vkInit::PipelineBuilder::createRenderpassInfo(const std::vector<vk::AttachmentDescription>& attachments, const vk::SubpassDescription& subpass)
{
	vk::RenderPassCreateInfo renderpassInfo = {};
	renderpassInfo.flags = vk::RenderPassCreateFlags();
	renderpassInfo.attachmentCount = attachments.size();
	renderpassInfo.pAttachments = attachments.data();
	renderpassInfo.subpassCount = 1;
	renderpassInfo.pSubpasses = &subpass;
	return renderpassInfo;
}

vkInit::PipelineBuilder::PipelineBuilder(vk::Device device) : device(device)
{
	reset();

	configureInputAssembly();
	configureRasterization();
	configureMultisampling();
	configureColorBlending();

	pipelineInfo.basePipelineHandle = nullptr;
}
void vkInit::PipelineBuilder::reset() {
	pipelineInfo.flags = vk::PipelineCreateFlags();

	resetVertexFormat();
	resetShaderModules();
	resetRenderpassAttachments();
	resetDescriptorsetLayouts();
}

void vkInit::PipelineBuilder::specify_vertex_format(std::vector<vk::VertexInputBindingDescription> bindingDescriptions, std::vector<vk::VertexInputAttributeDescription> attributeDescriptions)
{
	this->bindingDescriptions = bindingDescriptions;
	this->attributeDescriptions = attributeDescriptions;

	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(this->bindingDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = this->bindingDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(this->attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = this->attributeDescriptions.data();
}

void vkInit::PipelineBuilder::specify_vertex_shader(const char* filename)
{
	if (vertexShader) { device.destroyShaderModule(vertexShader); }
#ifndef NDEBUG
	std::cerr << "Create vertex shader module\n";
#endif // !NDEBUG
	vertexShader = vkUtil::createModule(filename, device);
	vertexShaderInfo = createShaderInfo(vertexShader, vk::ShaderStageFlagBits::eVertex);
	shaderStages.push_back(vertexShaderInfo);
}

void vkInit::PipelineBuilder::specify_fragment_shader(const char* filename)
{
	if (fragmentShader) { device.destroyShaderModule(fragmentShader); }
#ifndef NDEBUG
	std::cerr << "Create fragment shader module\n";
#endif // !NDEBUG
	fragmentShader = vkUtil::createModule(filename, device);
	fragmentShaderInfo = createShaderInfo(fragmentShader, vk::ShaderStageFlagBits::eFragment);
	shaderStages.push_back(fragmentShaderInfo);

}

void vkInit::PipelineBuilder::specify_swapchain_extent(vk::Extent2D screen_size)
{
	swapchainExtent = screen_size;
}

void vkInit::PipelineBuilder::specify_depth_attachment(const vk::Format& depthFormat, uint32_t attachment_index)
{
	depthState.flags = vk::PipelineDepthStencilStateCreateFlags();
	depthState.depthTestEnable = true;
	depthState.depthWriteEnable = true;
	depthState.depthCompareOp = vk::CompareOp::eLessOrEqual; // TODO: check if this works
	depthState.depthBoundsTestEnable = false;
	depthState.stencilTestEnable = false;

	pipelineInfo.pDepthStencilState = &depthState;
	attachmentDescriptions.insert({ attachment_index, createRenderpassAttachment(depthFormat, vk::ImageLayout::eDepthStencilAttachmentOptimal) });
	attachmentReferences.insert({ attachment_index, createAttachmentReference(attachment_index, vk::ImageLayout::eDepthStencilAttachmentOptimal) });
}

void vkInit::PipelineBuilder::clearDepthAttachment()
{
	pipelineInfo.pDepthStencilState = nullptr;
}

void vkInit::PipelineBuilder::addColorAttachment(const vk::Format& format, uint32_t attachment_index)
{
	attachmentDescriptions.insert({ attachment_index, createRenderpassAttachment(format, vk::ImageLayout::ePresentSrcKHR) });
	attachmentReferences.insert({ attachment_index, createAttachmentReference(attachment_index, vk::ImageLayout::eColorAttachmentOptimal) });
}

vkInit::GraphicsPipelineOutBundle vkInit::PipelineBuilder::build()
{
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

	createViewportState();
	pipelineInfo.pViewportState = &viewportState;

	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();

	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;

#ifndef NDEBUG
	std::cerr << "Create Pipeline Layout\n";
#endif // !NDEBUG

	auto pipelineLayout = createPipelineLayout();
	pipelineInfo.layout = pipelineLayout;

#ifndef NDEBUG
	std::cerr << "Create RenderPass\n";
#endif // !NDEBUG
	auto renderpass = createRenderpass();
	pipelineInfo.renderPass = renderpass;
	pipelineInfo.subpass = 0;

#ifndef NDEBUG
	std::cerr << "Create Graphics Pipeline\n";
#endif // !NDEBUG
	vk::Pipeline grahicsPipeline;
	try {
		grahicsPipeline = (device.createGraphicsPipeline(nullptr, pipelineInfo)).value;
	}
	catch (vk::SystemError err) {
#ifndef NDEBUG
		std::cerr << "Failed to create Pipeline\n";
#endif // !NDEBUG
	}
	
	resetShaderModules();
	
	GraphicsPipelineOutBundle output = { pipelineLayout, renderpass, grahicsPipeline };

	return output;
}

void vkInit::PipelineBuilder::addDescriptorsetLayout(vk::DescriptorSetLayout descriptorSetLayout)
{
	descriptorSetLayouts.push_back(descriptorSetLayout);
}

void vkInit::PipelineBuilder::resetDescriptorsetLayouts()
{
	descriptorSetLayouts.clear();
}



// vk::PushConstantRange vkInit::PipelineBuilder::create_push_constant_info()
// {
//	 vk::PushConstantRange pushConstantInfo;
//	 pushConstantInfo.offset = 0;
//	 pushConstantInfo.size = sizeof(vkUtil::ObjectData);
//	 pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;
//	
//	 return pushConstantInfo;
// }