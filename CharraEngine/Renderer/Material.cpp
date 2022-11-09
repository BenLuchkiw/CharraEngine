#include "Material.hpp"

namespace Charra
{
	Material::Material(Device& deviceRef, Renderpass& renderpassRef, const std::string& vertShader, const std::string& fragShader)
	: m_deviceRef(deviceRef), m_renderPassRef(renderpassRef), 
	  m_pipeline(deviceRef, renderpassRef, vertShader, fragShader, m_vertAttribs, m_attribDesc) 
	{

	}
	
	Material::~Material()
	{

	}
}