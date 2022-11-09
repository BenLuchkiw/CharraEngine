#include "Square.hpp"

#include "Math/MathFunctions.hpp"

namespace Charra
{
	GUI_Square::GUI_Square(fVec3 coords, fVec2 size, fVec4 colour, Mat4X4 mat)
	: m_coords(coords), m_size(size), m_colour(colour)
	{
		updateVertices(mat);
	}

	GUI_Square::GUI_Square(fVec3 coords, fVec2 size, fVec4 colour)
	: m_coords(coords), m_size(size), m_colour(colour)
	{
		
	}

	GUI_Square::~GUI_Square()
	{

	}

	std::array<uint32_t, 6> GUI_Square::getIndices(uint32_t i)
	{
		// i = indexOffset
		return {0 + i, 1 + i, 2 + i, 2 + i, 3 + i, 0 + i};
	}

	void GUI_Square::updateVertices(fVec3 coords, fVec2 size, fVec4 colour, Mat4X4 mat)
	{
		m_coords = coords;
		m_size = size;
		m_colour = colour;
		updateVertices(mat);
	}

	void GUI_Square::updateVertices(Mat4X4 mat)
	{
		m_vertices[0].position = mulMatrix(mat, m_coords);
		m_vertices[1].position = mulMatrix(mat, {m_coords.x + m_size.width, m_coords.y, m_coords.z});
		m_vertices[2].position = mulMatrix(mat, {m_coords.x + m_size.width, m_coords.y + m_size.height, m_coords.z});
		m_vertices[3].position = mulMatrix(mat, {m_coords.x, m_coords.y + m_size.height, m_coords.z});

		m_vertices[0].colour = m_colour;
		m_vertices[1].colour = m_colour;
		m_vertices[2].colour = m_colour;
		m_vertices[3].colour = m_colour;
	}
}