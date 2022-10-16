#include "Renderpass.hpp"

#include "Core/Logging.hpp"

namespace Charra
{
	Renderpass::Renderpass(Device& deviceRef, VkFormat format)
		: m_deviceRef(deviceRef),
		m_renderpass(VK_NULL_HANDLE),
		m_format(format)
	{
		createRenderpass();
	}

	Renderpass::~Renderpass()
	{
		vkDestroyRenderPass(m_deviceRef.getDevice(), m_renderpass, NULL);
	}

	void Renderpass::createRenderpass()
	{
		VkAttachmentDescription attachmentDescription{};
		attachmentDescription.flags = NULL;
		attachmentDescription.format = m_format;
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference attachmentReference{};
		attachmentReference.attachment = 0;
		attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription{};
		subpassDescription.flags = NULL;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = NULL;
		subpassDescription.pInputAttachments = NULL;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &attachmentReference;
		subpassDescription.pResolveAttachments = NULL;
		subpassDescription.pDepthStencilAttachment = NULL;
		subpassDescription.preserveAttachmentCount = NULL;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = NULL;
		renderPassCreateInfo.flags = NULL;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &attachmentDescription;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = NULL;
		renderPassCreateInfo.pDependencies = NULL;

		CHARRA_LOG_ERROR(vkCreateRenderPass(m_deviceRef.getDevice(), &renderPassCreateInfo, nullptr, &m_renderpass), "Vulkan failed to create a render pass");
	}
}