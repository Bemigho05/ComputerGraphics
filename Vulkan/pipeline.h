#pragma once
#include "config.h"
#include "shaders.h"
#include "render_structs.h"

namespace vkInit {
	struct GraphicsPipelineInBundle {
		vk::Device device;
		const std::string& vertexFilePath;
		const std::string& fragmentFilePath;
		vk::Extent2D swapchainExtent;
		vk::Format swapchainImageFormat;
	};

	struct GraphicsPipelineOutBundle {
		vk::PipelineLayout& layout;
		vk::RenderPass& renderpass;
		vk::Pipeline& pipeline;
	};

	vk::PipelineLayout make_pipeline_layout(const vk::Device& device, const bool debug = false) {

		vk::PipelineLayoutCreateInfo layoutInfo;
		layoutInfo.flags = vk::PipelineLayoutCreateFlags();
		layoutInfo.setLayoutCount = 0;

		layoutInfo.pushConstantRangeCount = 1;
		vk::PushConstantRange pushConstantInfo;
		pushConstantInfo.offset = 0;
		pushConstantInfo.size = sizeof(vkUtil::ObjectData);
		pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;
		layoutInfo.pPushConstantRanges = &pushConstantInfo;

		try {
			return device.createPipelineLayout(layoutInfo);
		}
		catch (vk::SystemError err) {
			if (debug) std::cerr << "Failed to create pipeline layout!" << std::endl;
		}

	}

	vk::RenderPass make_renderpass(const vk::Device& device, const vk::Format& swapchainImageFormat, const bool debug = false) {
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

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.flags = vk::SubpassDescriptionFlags();
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		vk::RenderPassCreateInfo renderpassInfo = {};
		renderpassInfo.flags = vk::RenderPassCreateFlags();
		renderpassInfo.attachmentCount = 1;
		renderpassInfo.pAttachments = &colorAttachment;
		renderpassInfo.subpassCount = 1;
		renderpassInfo.pSubpasses = &subpass;

		try {
			return device.createRenderPass(renderpassInfo);
		}
		catch (vk::SystemError err) { if (debug) std::cerr << "Failed to create renderpass!" << std::endl; }
	}

	GraphicsPipelineOutBundle make_graphics_pipeline(const GraphicsPipelineInBundle& specification, const bool debug = false) {
		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.flags = vk::PipelineCreateFlags();
		

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

		
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		pipelineInfo.pVertexInputState = &vertexInputInfo;

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
		inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
		inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

		if (debug) std::cout << "Create vertex shader module" << std::endl;
		
		vk::ShaderModule vertexShader = vkUtil::createModule(specification.vertexFilePath, specification.device, debug);
		vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
		vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
		vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertexShaderInfo.module = vertexShader;
		vertexShaderInfo.pName = "main";
		shaderStages.push_back(vertexShaderInfo);

		
		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = specification.swapchainExtent.width;
		viewport.height = specification.swapchainExtent.height;
		viewport.minDepth = 0.0;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor = {};
		scissor.offset.x = 0.0f;
		scissor.offset.y = 0.0f;
		scissor.extent = specification.swapchainExtent;

		vk::PipelineViewportStateCreateInfo viewportState = {};
		viewportState.flags = vk::PipelineViewportStateCreateFlags();
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;
		pipelineInfo.pViewportState = &viewportState;

		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasClamp = VK_FALSE;
		pipelineInfo.pRasterizationState = &rasterizer;


		vk::ShaderModule fragmentShader = vkUtil::createModule(specification.fragmentFilePath, specification.device, debug);
		vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
		fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
		fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragmentShaderInfo.module = fragmentShader;
		fragmentShaderInfo.pName = "main";
		shaderStages.push_back(fragmentShaderInfo);

		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();

		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		pipelineInfo.pMultisampleState = &multisampling;

		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;

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
		pipelineInfo.pColorBlendState = &colorBlending;


		if (debug) { std::cout << "Create Pipeline layout" << std::endl; }
		auto layout = make_pipeline_layout(specification.device, debug);
		pipelineInfo.layout = layout;


		if (debug) std::cout << "Create RenderPass" << std::endl;
		auto renderpass = make_renderpass(specification.device, specification.swapchainImageFormat, debug);
		pipelineInfo.renderPass = renderpass;
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineHandle = nullptr;

		if (debug) std::cout << "Create Graphics Pipeline" << std::endl;

		vk::Pipeline graphicsPipeline;
		try {
			graphicsPipeline = specification.device.createGraphicsPipeline(nullptr, pipelineInfo).value;
		}
		catch (vk::SystemError err) {
			if (debug) std::cerr << "Failed to create Graphics Pipeline!" << std::endl;
		}
		

		GraphicsPipelineOutBundle output = { layout, renderpass, graphicsPipeline };

		specification.device.destroyShaderModule(vertexShader);
		specification.device.destroyShaderModule(fragmentShader);
		return output;
	}
}