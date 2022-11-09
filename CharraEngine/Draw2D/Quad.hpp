#pragma once

#include "Math/MathTypes.hpp"
#include "Math/RendererTypes.hpp"

#include <array>

namespace Charra
{
	class Quad
	{
	public:
		Quad();
		Quad(fVec3 coords, fVec2 size, fVec4 colour);
		~Quad();

		fVec3& getCoords() { return m_coords; }
		fVec2& getSize()   { return m_size;   }
		fVec4& getColour() { return m_colour; }
		uint32_t getWindowID() { return m_windowID; }

		std::array<Vertex, 4>& getVertices() { return m_vertices; }
		std::array<uint32_t, 6> getIndices(uint32_t indexOffset);

		// Usually done during drawing
		void updateVertices(Mat4X4 mat);
		void assignWindow(uint32_t windowID) { m_windowID = windowID; }

	private: // Methods
	private: // Members
		// Automatically goes to main window to simplify single window projects
		uint32_t m_windowID = 0;
		fVec3 m_coords = {};
		fVec2 m_size   = {};
		fVec4 m_colour = {};

		std::array<Vertex, 4> m_vertices;
	};
}