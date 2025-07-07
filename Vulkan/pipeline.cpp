#include "pipeline.h"
#include "shaders.h"
#include "render_structs.h"
#include "mesh.h"

vk::PipelineLayout vkInit::create_pipeline_layout(const vk::Device& device, std::vector<vk::DescriptorSetLayout> descriptorSetLayouts)
{
	vk::PipelineLayoutCreateInfo layoutInfo;
	layoutInfo.flags = vk::PipelineLayoutCreateFlags();
	layoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	layoutInfo.pSetLayouts = descriptorSetLayouts.data();

	/*layoutInfo.pushConstantRangeCount = 0;
	vk::PushConstantRange pushConstantInfo = create_push_constant_info();
	layoutInfo.pPushConstantRanges = &pushConstantInfo;*/

	try {
		return device.createPipelineLayout(layoutInfo);
	}
	catch (vk::SystemError err) {
#ifndef NDEBUG
		std::cerr << "Failed to create pipeline layout!" << std::endl;
#endif
	}
}

vk::RenderPass vkInit::create_renderpass(const vk::Device& device, const vk::Format& swapchainImageFormat, const vk::Format& depthFormat)
{
	std::vector<vk::AttachmentDescription> attachments;
	std::vector<vk::AttachmentReference> attachmentRefs;

	attachments.push_back(create_color_attachment(swapchainImageFormat));
	attachmentRefs.push_back(create_color_attachment_reference());

	attachments.push_back(create_depth_attachment(depthFormat));
	attachmentRefs.push_back(create_depth_attachment_reference());

	auto subpass = create_subpass(attachmentRefs);

	auto renderpassInfo = create_renderpass_info(attachments, subpass);

	try {
		return device.createRenderPass(renderpassInfo);
	}
	catch (vk::SystemError err) {
#ifndef NDEBUG
			std::cerr << "Failed to create renderpass!" << std::endl;
#endif
	}
}

vk::PipelineVertexInputStateCreateInfo vkInit::create_vertex_input_info(const std::vector<vk::VertexInputBindingDescription>& bindingDescriptions, const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions)
{
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	return vertexInputInfo;
}

vk::PipelineInputAssemblyStateCreateInfo vkInit::create_input_assembly_info()
{
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
	inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
	return inputAssemblyInfo;
}

vk::PipelineShaderStageCreateInfo vkInit::create_shader_info(const vk::ShaderModule& shaderModule, const vk::ShaderStageFlagBits& stage)
{
	vk::PipelineShaderStageCreateInfo shaderInfo = {};
	shaderInfo.flags = vk::PipelineShaderStageCreateFlags();
	shaderInfo.stage = stage;
	shaderInfo.module = shaderModule;
	shaderInfo.pName = "main";
	return shaderInfo;
}

vk::Viewport vkInit::create_viewport(const GraphicsPipelineInBundle& specification)
{
	vk::Viewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = specification.swapchainExtent.width;
	viewport.height = specification.swapchainExtent.height;
	viewport.minDepth = 0.0;
	viewport.maxDepth = 1.0f;
	return viewport;
}

vk::Rect2D vkInit::create_scissor(const GraphicsPipelineInBundle& specification)
{
	vk::Rect2D scissor = {};
	scissor.offset.x = 0.0f;
	scissor.offset.y = 0.0f;
	scissor.extent = specification.swapchainExtent;
	return scissor;
}

vk::PipelineViewportStateCreateInfo vkInit::create_viewport_state(const vk::Viewport& viewport, const vk::Rect2D& scissor)
{
	vk::PipelineViewportStateCreateInfo viewportState = {};
	viewportState.flags = vk::PipelineViewportStateCreateFlags();
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	
	return viewportState;
}

vk::PipelineRasterizationStateCreateInfo vkInit::create_rasterizer_info()
{
	vk::PipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace = vk::FrontFace::eClockwise;
	rasterizer.depthBiasClamp = VK_FALSE;
	return rasterizer;
}

vk::PipelineMultisampleStateCreateInfo vkInit::create_multisampling_info()
{
	vk::PipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
	return multisampling;
}

vk::PipelineColorBlendAttachmentState vkInit::create_color_blend_attachment_state()
{
	vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_FALSE;
	return colorBlendAttachment;
}

vk::PipelineColorBlendStateCreateInfo vkInit::create_color_blend_attachment_stage(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment)
{
	vk::PipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
	return colorBlending;
}


vk::PushConstantRange vkInit::create_push_constant_info()
{
	vk::PushConstantRange pushConstantInfo;
	pushConstantInfo.offset = 0;
	pushConstantInfo.size = sizeof(vkUtil::ObjectData);
	pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;
	
	return pushConstantInfo;
}

vk::AttachmentDescription vkInit::create_color_attachment(const vk::Format& swapchainImageFormat)
{
	vk::AttachmentDescription colorAttachment = {};
	colorAttachment.flags = vk::AttachmentDescriptionFlags();
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;
	return colorAttachment;
}

vk::AttachmentDescription vkInit::create_depth_attachment(const vk::Format& depthFormat)
{
	vk::AttachmentDescription depthAttachment = {};
	depthAttachment.flags = vk::AttachmentDescriptionFlags();
	depthAttachment.format = depthFormat;
	depthAttachment.samples = vk::SampleCountFlagBits::e1;
	depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
	depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	return depthAttachment;
}

vk::AttachmentReference vkInit::create_color_attachment_reference()
{
	vk::AttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
	return colorAttachmentRef;
}

vk::AttachmentReference vkInit::create_depth_attachment_reference()
{
	vk::AttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
	return depthAttachmentRef;
}

vk::SubpassDescription vkInit::create_subpass(const std::vector<vk::AttachmentReference>& attachmentRefs)
{
	vk::SubpassDescription subpass = {};
	subpass.flags = vk::SubpassDescriptionFlags();
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachmentRefs[0];
	subpass.pDepthStencilAttachment = &attachmentRefs[1];
	return subpass;
}

vk::RenderPassCreateInfo vkInit::create_renderpass_info(const std::vector<vk::AttachmentDescription>& attachements, const vk::SubpassDescription subpass)
{
	vk::RenderPassCreateInfo renderpassInfo = {};
	renderpassInfo.flags = vk::RenderPassCreateFlags();
	renderpassInfo.attachmentCount = attachements.size();
	renderpassInfo.pAttachments = attachements.data();
	renderpassInfo.subpassCount = 1;
	renderpassInfo.pSubpasses = &subpass;
	return renderpassInfo;
}

vkInit::GraphicsPipelineOutBundle vkInit::create_graphics_pipeline(const GraphicsPipelineInBundle& specification)
{
	vk::GraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.flags = vk::PipelineCreateFlags();

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

	auto bindingDescription = vkMesh::getPosColorBindingDescription();
	auto attributeDescriptions = vkMesh::getPosColorAttributeDescriptions();
	auto vertexInputInfo = create_vertex_input_info(bindingDescription, attributeDescriptions);
	pipelineInfo.pVertexInputState = &vertexInputInfo;

	auto inputAssemblyInfo = create_input_assembly_info();
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

#ifndef NDEBUG
	std::cout << "Create vertex shader module" << std::endl;
#endif

	auto vertexShader = vkUtil::createModule(specification.vertexFilePath, specification.device);
	auto vertexShaderInfo = create_shader_info(vertexShader, vk::ShaderStageFlagBits::eVertex);
	shaderStages.push_back(vertexShaderInfo);

	auto fragmentShader = vkUtil::createModule(specification.fragmentFilePath, specification.device);
	auto fragmentShaderInfo = create_shader_info(fragmentShader, vk::ShaderStageFlagBits::eFragment);
	shaderStages.push_back(fragmentShaderInfo);

	auto viewport = create_viewport(specification);
	auto scissor = create_scissor(specification);
	auto viewportState = create_viewport_state(viewport, scissor);
	pipelineInfo.pViewportState = &viewportState;

	auto rasterizer = create_rasterizer_info();
	pipelineInfo.pRasterizationState = &rasterizer;


	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();

	vk::PipelineDepthStencilStateCreateInfo depthState;
	depthState.flags = vk::PipelineDepthStencilStateCreateFlags();
	depthState.depthTestEnable = true;
	depthState.depthWriteEnable = true;
	depthState.depthCompareOp = vk::CompareOp::eLessOrEqual; // TODO: play with this
	depthState.depthBoundsTestEnable = false;
	depthState.stencilTestEnable = false;
	pipelineInfo.pDepthStencilState = &depthState;

	auto multisampling = create_multisampling_info();
	pipelineInfo.pMultisampleState = &multisampling;

	auto colorBlendAttachment = create_color_blend_attachment_state();
	auto colorBlending = create_color_blend_attachment_stage(colorBlendAttachment);
	pipelineInfo.pColorBlendState = &colorBlending;


#ifndef NDEBUG
	std::cout << "Create Pipeline layout" << std::endl;
#endif

	auto pipelineLayout = create_pipeline_layout(specification.device, specification.descriptorSetLayouts);
	pipelineInfo.layout = pipelineLayout;


#ifndef NDEBUG
		std::cout << "Create RenderPass" << std::endl;
#endif

	auto renderpass = create_renderpass(specification.device, specification.swapchainImageFormat, specification.depthFormat);
	pipelineInfo.renderPass = renderpass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = nullptr;

#ifndef NDEBUG
		std::cout << "Create Graphics Pipeline" << std::endl;
#endif

	vk::Pipeline graphicsPipeline;
	try {
		graphicsPipeline = specification.device.createGraphicsPipeline(nullptr, pipelineInfo).value;
	}
	catch (vk::SystemError err) {
#ifndef NDEBUG
			std::cerr << "Failed to create Graphics Pipeline!" << std::endl;
#endif
	}


	GraphicsPipelineOutBundle output = { pipelineLayout, renderpass, graphicsPipeline };

	specification.device.destroyShaderModule(vertexShader);
	specification.device.destroyShaderModule(fragmentShader);
	return output;
}
