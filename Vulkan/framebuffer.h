#pragma once
#include "config.h"
#include "frame.h"

namespace vkInit {
	struct frameBufferInput {
		vk::Device device;
		vk::RenderPass renderpass;
		vk::Extent2D swapchainExtent;
	};

	void make_framebuffers(frameBufferInput inputChunk, std::vector<vkUtil::SwapChainFrame>& frames, const bool debug) {
		int i = 0;
		for (auto& frame : frames) {
			std::vector<vk::ImageView> attachements = { frame.imageView };
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
				if (debug) std::cout << "Created framebuffer for frame " << i << std::endl;

			}
			catch (vk::SystemError err) { if (debug) std::cout << "Failed to create framebuffer for frame " << i << std::endl; }
			i++;
		}
	}
}