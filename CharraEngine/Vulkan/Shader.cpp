#include "Shader.hpp"

#include "Renderer/Renderer.hpp"

#include "Platform/Platform.hpp"
#include "Core/Logging.hpp"

namespace Charra
{
	Shader::Shader(Device* deviceRef,const std::string& filename, ShaderType shaderType)
		: m_deviceRef(deviceRef), 
		m_shaderModule(VK_NULL_HANDLE),
		m_shaderType(shaderType)
	{
		createShader(filename);
	}

	Shader::~Shader()
	{
		vkDestroyShaderModule(m_deviceRef->getDevice(), m_shaderModule, NULL);
	}

	void Shader::createShader(const std::string& filename)
	{
		std::string fileLocation = std::string(std::string(ROOT_DIR) + std::string("\\CharraEngine\\Shaders\\") + filename);

		uint32_t size = 0;
		std::vector<char> file = Platform::readFile(fileLocation.c_str(), &size);

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext;
		createInfo.flags;
		createInfo.codeSize = size;
		createInfo.pCode = reinterpret_cast<uint32_t*>(file.data());

		CHARRA_LOG_ERROR(vkCreateShaderModule(m_deviceRef->getDevice(), &createInfo, NULL, &m_shaderModule) != VK_SUCCESS, "Vulkan failed to create shader module");
	}
}