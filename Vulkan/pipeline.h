#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "config.h"

namespace vkInit {
	struct GraphicsPipelineInBundle {
		vk::Device device;
		std::string vertexFilePath;
		std::string fragmentFilePath;
		vk::Extent2D swapchainExtent;
		vk::Format swapchainImageFormat, depthFormat;
		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
	};

	struct GraphicsPipelineOutBundle {
		vk::PipelineLayout& layout;
		vk::RenderPass& renderpass;
		vk::Pipeline& pipeline;
	};
	

	vk::PipelineLayout create_pipeline_layout(const vk::Device& device, std::vector<vk::DescriptorSetLayout> descriptorSetLayouts);

	vk::RenderPass create_renderpass(const vk::Device& device, const vk::Format& swapchainImageFormat, const vk::Format& depthFormat);

	vk::PipelineVertexInputStateCreateInfo create_vertex_input_info(const std::vector<vk::VertexInputBindingDescription>& bindingDescription, const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions);
	vk::PipelineInputAssemblyStateCreateInfo create_input_assembly_info();
	vk::PipelineShaderStageCreateInfo create_shader_info(const vk::ShaderModule& shaderModule, const vk::ShaderStageFlagBits& stage);
	vk::Viewport create_viewport(const GraphicsPipelineInBundle& specification);
	vk::Rect2D create_scissor(const GraphicsPipelineInBundle& specification);
	vk::PipelineViewportStateCreateInfo create_viewport_state(const vk::Viewport& viewport, const vk::Rect2D& scissor);
	vk::PipelineRasterizationStateCreateInfo create_rasterizer_info();
	vk::PipelineMultisampleStateCreateInfo create_multisampling_info();
	vk::PipelineColorBlendAttachmentState create_color_blend_attachment_state();
	vk::PipelineColorBlendStateCreateInfo create_color_blend_attachment_stage(const vk::PipelineColorBlendAttachmentState& colorBlendAttachment);
	
	vk::PushConstantRange create_push_constant_info();
	vk::AttachmentDescription create_color_attachment(const vk::Format& swapchainImageFormat);
	vk::AttachmentDescription create_depth_attachment(const vk::Format& depthFormat);
	vk::AttachmentReference create_color_attachment_reference();
	vk::AttachmentReference create_depth_attachment_reference();
	vk::SubpassDescription create_subpass(const std::vector<vk::AttachmentReference>& attachmentRefs);
	vk::RenderPassCreateInfo create_renderpass_info(const std::vector<vk::AttachmentDescription>& attachements, const vk::SubpassDescription subpass);


	GraphicsPipelineOutBundle create_graphics_pipeline(const GraphicsPipelineInBundle& specification);
}