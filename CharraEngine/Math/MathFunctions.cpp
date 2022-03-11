#include "MathFunctions.hpp"

namespace Charra
{
	Mat4X4 getOrthographicMatrix(float far, float near, float left, float right, float top, float bottom)
	{
		Mat4X4 mat;

		//mat[0][0] = 2 / (right - left);
		//mat[1][1] = 2 / (top - bottom);
		//mat[2][2] = -1 / (far - near);
		//mat[3][0] = (right + left) / (right - left);
		//mat[3][1] = (top + bottom) / (top - bottom);
		//mat[3][2] = (far + near) / (far - near);
		//mat[3][3] = 1.0f;

		mat[0][0] = 2 / (right - left);
		mat[1][1] = 2 / (bottom - top);
		mat[2][2] = -1 / (far - near);
		mat[3][0] = -(right + left) / (right - left);
		mat[3][1] = -(bottom + top) / (bottom - top);
		mat[3][2] = (far + near) / (far - near);
		mat[3][3] = 1.0f;

		return mat;
	}

	fVec3 mulMatrix(Mat4X4 mat, fVec3 point)
	{
		return mulMatrix(mat, fVec4(point, 1.0f)).vecABC;
	}

	fVec4 mulMatrix(Mat4X4 mat, fVec4 point)
	{
		fVec4 result;

		result.x = mat[0][0] * point.x;
		result.y = mat[0][1] * point.x;
		result.z = mat[0][2] * point.x;
		result.w = mat[0][3] * point.x;

		result.x += mat[1][0] * point.y;
		result.y += mat[1][1] * point.y;
		result.z += mat[1][2] * point.y;
		result.w += mat[1][3] * point.y;

		result.x += mat[2][0] * point.z;
		result.y += mat[2][1] * point.z;
		result.z += mat[2][2] * point.z;
		result.w += mat[2][3] * point.z;

		result.x += mat[3][0] * point.w;
		result.y += mat[3][1] * point.w;
		result.z += mat[3][2] * point.w;
		result.w += mat[3][3] * point.w;

		return result;
	}
}