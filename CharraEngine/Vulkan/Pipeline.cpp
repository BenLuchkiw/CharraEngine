#include "Pipeline.hpp"

#include "Core/Logging.hpp"

namespace Charra
{
	GraphicsPipeline::GraphicsPipeline(Device& deviceRef, Renderpass& renderpassRef, const std::string& vertexFilename, const std::string& fragmentFilename,
		const VkVertexInputBindingDescription& bindingDescription, const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
		: m_deviceRef(deviceRef),
		m_renderpassRef(renderpassRef),
		m_vertexShader(deviceRef, vertexFilename, ShaderType::CHARRA_SHADER_TYPE_VERTEX),
		m_fragmentShader(deviceRef, fragmentFilename, ShaderType::CHARRA_SHADER_TYPE_FRAGMENT),
		m_pipelineLayout(VK_NULL_HANDLE),
		m_pipeline(VK_NULL_HANDLE)
	{
		createGraphicsPipeline(bindingDescription, attributeDescriptions);
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
		vkDestroyPipeline(m_deviceRef.getDevice(), m_pipeline, NULL);
		vkDestroyPipelineLayout(m_deviceRef.getDevice(), m_pipelineLayout, NULL);
	}

	void GraphicsPipeline::createGraphicsPipeline(const VkVertexInputBindingDescription& bindingDescription, const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
	{
		VkPipelineShaderStageCreateInfo vertexShaderStage{};
		vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStage.pNext = NULL;
		vertexShaderStage.flags = 0;
		vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStage.module = *m_vertexShader.getShaderModule();
		vertexShaderStage.pName = "main";
		vertexShaderStage.pSpecializationInfo = NULL;

		VkPipelineShaderStageCreateInfo fragmentShaderStage{};
		fragmentShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStage.pNext = NULL;
		fragmentShaderStage.flags = 0;
		fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderStage.module = *m_fragmentShader.getShaderModule();
		fragmentShaderStage.pName = "main";
		fragmentShaderStage.pSpecializationInfo = NULL;

		VkPipelineShaderStageCreateInfo graphicsShaders[2] = {vertexShaderStage, fragmentShaderStage};

		VkPipelineVertexInputStateCreateInfo vertexCreateInfo{};
		vertexCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexCreateInfo.pNext = NULL;
		vertexCreateInfo.flags = NULL;
		vertexCreateInfo.vertexBindingDescriptionCount = 1;
		vertexCreateInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
		vertexCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
		inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyCreateInfo.pNext = NULL;
		inputAssemblyCreateInfo.flags = NULL;
		inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.minDepth = -1;
		viewport.maxDepth = 1;
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = 0;
		viewport.height = 0;

		VkRect2D scissor{};
		scissor.extent = { 0,0 };
		scissor.offset = { 0,0 };

		VkPipelineViewportStateCreateInfo viewportCreateInfo{};
		viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportCreateInfo.pNext = NULL;
		viewportCreateInfo.flags = NULL;
		viewportCreateInfo.viewportCount = 1;
		viewportCreateInfo.pViewports = &viewport;
		viewportCreateInfo.scissorCount = 1;
		viewportCreateInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.pNext = NULL;
		rasterizationStateCreateInfo.flags = NULL;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationStateCreateInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.pNext = NULL;
		multisampleStateCreateInfo.flags = NULL;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleStateCreateInfo.minSampleShading = 1.0f;
		multisampleStateCreateInfo.pSampleMask = nullptr;
		multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
		depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCreateInfo.pNext = NULL;
		depthStencilStateCreateInfo.flags = NULL;
		depthStencilStateCreateInfo.depthTestEnable = NULL;
		depthStencilStateCreateInfo.depthWriteEnable = NULL;
		depthStencilStateCreateInfo.depthCompareOp;
		depthStencilStateCreateInfo.depthBoundsTestEnable = NULL;
		depthStencilStateCreateInfo.stencilTestEnable = NULL;
		depthStencilStateCreateInfo.front;
		depthStencilStateCreateInfo.back;
		depthStencilStateCreateInfo.minDepthBounds = NULL;
		depthStencilStateCreateInfo.maxDepthBounds = NULL;

		VkPipelineColorBlendAttachmentState colorBlendAttachmentStateCreateInfo{};
		colorBlendAttachmentStateCreateInfo.blendEnable = VK_TRUE;
		colorBlendAttachmentStateCreateInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachmentStateCreateInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachmentStateCreateInfo.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentStateCreateInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentStateCreateInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentStateCreateInfo.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentStateCreateInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.pNext = NULL;
		colorBlendStateCreateInfo.flags = NULL;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentStateCreateInfo;
		colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = NULL;
		dynamicStateCreateInfo.flags = NULL;
		dynamicStateCreateInfo.dynamicStateCount = 2;
		dynamicStateCreateInfo.pDynamicStates = dynamicStates;

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = NULL;
		pipelineLayoutCreateInfo.flags = NULL;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		CHARRA_LOG_ERROR(vkCreatePipelineLayout(m_deviceRef.getDevice(), &pipelineLayoutCreateInfo, NULL, &m_pipelineLayout) != VK_SUCCESS, "Vulkan failed to create a pipeline layout");

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.pNext = NULL;
		pipelineCreateInfo.flags = NULL;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = graphicsShaders;
		pipelineCreateInfo.pVertexInputState = &vertexCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
		pipelineCreateInfo.pTessellationState = NULL;
		pipelineCreateInfo.pViewportState = &viewportCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
		pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.layout = m_pipelineLayout;
		pipelineCreateInfo.renderPass = m_renderpassRef.getRenderPass();
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;

		CHARRA_LOG_ERROR(vkCreateGraphicsPipelines(m_deviceRef.getDevice(), NULL, 1, &pipelineCreateInfo, NULL, &m_pipeline), "Vulkan failed to create a pipeline");
	}
}