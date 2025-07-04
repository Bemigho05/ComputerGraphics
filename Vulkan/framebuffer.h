#pragma once
#include "config.h"
#include "frame.h"

namespace vkInit {
	struct frameBufferInput {
		vk::Device device;
		vk::RenderPass renderpass;
		vk::Extent2D swapchainExtent;
	};

	void make_framebuffers(frameBufferInput inputChunk, std::vector<vkUtil::SwapChainFrame>& frames) {
		int i = 0;
		for (auto& frame : frames) {
			std::vector<vk::ImageView> attachements = { frame.imageView, frame.depthBufferView };
			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.flags = vk::FramebufferCreateFlags();
			framebufferInfo.renderPass = inputChunk.renderpass;
			framebufferInfo.attachmentCount = attachements.size();
			framebufferInfo.pAttachments = attachements.data();
			framebufferInfo.width = inputChunk.swapchainExtent.width;
			framebufferInfo.height = inputChunk.swapchainExtent.height;
			framebufferInfo.layers = 1;

			try {
				frame.framebuffer = inputChunk.device.createFramebuffer(framebufferInfo);
#ifndef NDEBUG
				std::cout << "Created framebuffer for frame " << i << std::endl;
#endif // !NDEBUG

			}
			catch (vk::SystemError err) { 
#ifndef NDEBUG
				std::cerr << "Failed to create framebuffer for frame " << i << std::endl;
#endif // !NDEBUG
			}
			i++;
		}
	}
}