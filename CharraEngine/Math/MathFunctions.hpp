#pragma once

#include "MathTypes.hpp"

namespace Charra
{
	Mat4X4 getOrthographicMatrix(float far, float near, float left, float right, float top, float bottom);

	fVec3 mulMatrix(Mat4X4 mat, fVec3 point);
	fVec4 mulMatrix(Mat4X4 mat, fVec4 point);
}; 