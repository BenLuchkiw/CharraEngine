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
		Material(Device& deviceRef, Renderpass& renderpassRef, const std::string& vertShader, const std::string& fragShader);
		~Material();

		inline GraphicsPipeline& getPipeline() { return m_pipeline; }

	private: // Methods

	private: // Members
		Device& m_deviceRef;
		Renderpass& m_renderPassRef;

		VkVertexInputBindingDescription m_vertAttribs{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
		std::vector<VkVertexInputAttributeDescription> m_attribDesc = {
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
			{1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, colour)}
		};

		GraphicsPipeline m_pipeline;
	};
}