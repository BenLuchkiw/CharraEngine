#pragma once

#include <vector>
#include <string>

#include "Vulkan/Allocator/Allocator.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Pipeline.hpp"
#include "Vulkan/Renderpass.hpp"

namespace Charra
{

	class Material
	{
	public:
		Material(Device& deviceRef, Renderpass& renderpassRef, VkVertexInputBindingDescription vertexAttribs, const std::vector<VkVertexInputAttributeDescription>& attribDescriptions,
				 const std::string& vertShader, const std::string& fragShader);
		~Material();

		inline GraphicsPipeline& getPipeline() { return m_pipeline; }

	private: // Methods

	private: // Members
		Device& m_deviceRef;
		Renderpass& m_renderPassRef;

		VkVertexInputBindingDescription m_vertexBindingAttributes;
		std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;

		GraphicsPipeline m_pipeline;
	};
}