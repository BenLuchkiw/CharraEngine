#pragma once

#include <array>

#include "Math/MathTypes.hpp"
#include "Math/RendererTypes.hpp"

namespace Charra
{
	class GUI_Square
	{
	public:
		GUI_Square() {};
		GUI_Square(fVec2 coords, fVec2 size, fVec4 colour, Mat4X4 matrix);
		~GUI_Square();

		void updateVertices(fVec2 coords, fVec2 size, fVec4 colour, Mat4X4 matrix);

		std::array<Vertex, 4>& getVertices() { return m_vertices; }
		std::array<uint32_t, 6> getIndices(uint32_t indexOffset);
	private: // Methods
		void updateVertices(Mat4X4 matrix);

	private: // Members
		fVec2 m_coords;
		fVec2 m_size;
		fVec4 m_colour;

		std::array<Vertex, 4> m_vertices;
		std::array<uint32_t, 6> m_indices = {0,1,2,2,3,0};
	};
}