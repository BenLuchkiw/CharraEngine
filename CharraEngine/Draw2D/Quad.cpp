#include "Quad.hpp"

#include "Math/MathFunctions.hpp"

namespace Charra
{
	Quad::Quad()
	{

	}

	Quad::Quad(fVec3 coords, fVec2 size, fVec4 colour)
	: m_coords(coords), m_size(size), m_colour(colour)
	{

	}

	Quad::~Quad()
	{

	}

	std::array<uint32_t, 6> Quad::getIndices(uint32_t i)
	{
		// indices go in 012 231 order
		return {0+i, 1+i, 2+i, 2+i, 3+i, 0+i};
	}

	void Quad::updateVertices(Mat4X4 orthoMatrix)
	{
		m_vertices[0].position = mulMatrix(orthoMatrix, m_coords);
		m_vertices[1].position = mulMatrix(orthoMatrix, {m_coords.x + m_size.width, m_coords.y, m_coords.z});
		m_vertices[2].position = mulMatrix(orthoMatrix, {m_coords.x + m_size.width, m_coords.y + m_size.height, m_coords.z});
		m_vertices[3].position = mulMatrix(orthoMatrix, {m_coords.x, m_coords.y + m_size.height, m_coords.z});

		m_vertices[0].colour = m_colour;
		m_vertices[1].colour = m_colour;
		m_vertices[2].colour = m_colour;
		m_vertices[3].colour = m_colour;
	}
}