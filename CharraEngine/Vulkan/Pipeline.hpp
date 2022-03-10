#pragma once

#include "vulkan/vulkan.h"

#include "Device.hpp"
#include "Renderpass.hpp"
#include "Shader.hpp"

#include <string>
#include <array>

namespace Charra
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(Device* deviceRef, Renderpass* renderpassRef, const std::string& vertexFilename, const std::string& fragmentFilename,
		const VkVertexInputBindingDescription& bindingDescription, const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
		~GraphicsPipeline();

		//void init(Window* windowRef, const std::string& computeFileName);
		void init();
		void destroy();

		VkPipeline getPipeline() { return m_pipeline; }

	private: // Methods
		void createGraphicsPipeline(const VkVertexInputBindingDescription& bindingDescription, const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
		//void createComputePipeline();
	private: // Members
		Device* m_deviceRef;
		Renderpass* m_renderpassRef;

		Shader m_vertexShader;
		Shader m_fragmentShader;

		VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_pipeline = VK_NULL_HANDLE;
	};

// TODO: compute

	class ComputePipeline
	{
	public:
		
	private: // Methods

	private: // Members
	};
}