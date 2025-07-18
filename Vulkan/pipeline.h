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
		vk::PipelineLayout layout;
		vk::RenderPass renderpass;
		vk::Pipeline pipeline;
	};

	class PipelineBuilder {
	public:
		PipelineBuilder(vk::Device device);

		void reset();

		void specify_vertex_format(std::vector<vk::VertexInputBindingDescription> bindingDescription, std::vector<vk::VertexInputAttributeDescription> attributeDescriptions);
		void specify_vertex_shader(const char* filename);
		void specify_fragment_shader(const char* filename);
		void specify_swapchain_extent(vk::Extent2D screen_size);
		void specify_depth_attachment(const vk::Format& depthFormat, uint32_t attachment_index);
		void clearDepthAttachment();
		void addColorAttachment(const vk::Format& format, uint32_t attachment_index);


		GraphicsPipelineOutBundle build();

		void addDescriptorsetLayout(vk::DescriptorSetLayout descriptorSetLayout);
		void resetDescriptorsetLayouts();

	private:
		vk::Device device;
		vk::GraphicsPipelineCreateInfo pipelineInfo = {};

		std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
		std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
		vk::ShaderModule vertexShader = nullptr, fragmentShader = nullptr;
		vk::PipelineShaderStageCreateInfo vertexShaderInfo, fragmentShaderInfo;

		vk::Extent2D swapchainExtent;
		vk::Viewport viewport = {};
		vk::Rect2D scissor = {};
		vk::PipelineViewportStateCreateInfo viewportState = {};
		vk::PipelineRasterizationStateCreateInfo rasterizer = {};

		vk::PipelineDepthStencilStateCreateInfo depthState;
		std::unordered_map<uint32_t, vk::AttachmentDescription> attachmentDescriptions;
		std::unordered_map<uint32_t, vk::AttachmentReference> attachmentReferences;
		std::vector<vk::AttachmentDescription> flattenedAttachmentDescriptions;
		std::vector<vk::AttachmentReference> flattenedAttachmentReferences;

		vk::PipelineMultisampleStateCreateInfo multisampling = {};

		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		vk::PipelineColorBlendStateCreateInfo colorBlending = {};

		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
		void resetVertexFormat();
		void resetShaderModules();
		void resetRenderpassAttachments();

		vk::AttachmentDescription createRenderpassAttachment(const vk::Format& format, vk::ImageLayout finalLayout);
		vk::AttachmentReference createAttachmentReference(uint32_t attachment_index, vk::ImageLayout layout);

		void configureInputAssembly();

		vk::PipelineShaderStageCreateInfo createShaderInfo(const vk::ShaderModule& shaderModule, const vk::ShaderStageFlagBits& stage);
		vk::PipelineViewportStateCreateInfo createViewportState();

		void configureRasterization();
		void configureMultisampling();
		void configureColorBlending();

		vk::PipelineLayout createPipelineLayout();
		vk::RenderPass createRenderpass();
		vk::SubpassDescription createSubpass(const std::vector<vk::AttachmentReference>& attachments);
		vk::RenderPassCreateInfo createRenderpassInfo(const std::vector<vk::AttachmentDescription>& attachments,const vk::SubpassDescription& subpass);
	};
}