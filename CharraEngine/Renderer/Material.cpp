#include "Material.hpp"

namespace Charra
{
	Material::Material(Device& deviceRef, Renderpass& renderpassRef, VkVertexInputBindingDescription vertexAttribs, const std::vector<VkVertexInputAttributeDescription>& attribDescriptions,
					   const std::string& vertShader, const std::string& fragShader)
	: m_deviceRef(deviceRef), m_renderPassRef(renderpassRef), 
	  m_vertexBindingAttributes(vertexAttribs),
	  m_attributeDescriptions(attribDescriptions),
	  m_pipeline(deviceRef, renderpassRef, vertShader, fragShader, vertexAttribs, attribDescriptions) 
	{

	}
	
	Material::~Material()
	{

	}
}