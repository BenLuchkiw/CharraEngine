#pragma once

#include "vulkan/vulkan.h"

#include "Device.hpp"

#include <string>

namespace Charra
{
	enum class ShaderType
	{
		CHARRA_SHADER_TYPE_VERTEX = 0,
		//CHARRA_SHADER_TYPE_TESSELLATION = 1,
		//CHARRA_SHADER_TYPE_GEOMETRY = 2,
		CHARRA_SHADER_TYPE_FRAGMENT = 3,
		CHARRA_SHADER_TYPE_COMPUTE = 4,
		CHARRA_SHADER_TYPE_UNKNOWN = 5
	};

	class Shader
	{
	public:
		Shader(Device* deviceRef, const std::string& filename, ShaderType shaderType);
		~Shader();

		void createShader(const std::string& filename);

		VkShaderModule* getShaderModule() { return &m_shaderModule; }
	private: // Methods

	private: // Members
		Device* m_deviceRef;
		VkShaderModule m_shaderModule = VK_NULL_HANDLE;

		ShaderType m_shaderType;
	};
}