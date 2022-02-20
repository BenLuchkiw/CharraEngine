#pragma once

#include <memory>
#include <string>

#include "Math/MathTypes.hpp"
#include "Core/Events.hpp"

namespace Charra
{
	struct RendererImplData;
	class Renderer
	{
	public: 
		Renderer(const std::string& mainWindowName, Events* eventHandler);
		~Renderer();

		void draw();

	private: // Methods

	private: // Members
		std::unique_ptr<RendererImplData> m_pImpl;
	};
}