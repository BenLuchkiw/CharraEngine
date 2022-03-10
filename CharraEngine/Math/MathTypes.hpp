#pragma once

#include <cstdint>

#define LOWER_UINT64(val) (uint32_t) (val & 0xFFFFFFUL);
#define UPPER_UINT64(val) (uint32_t) ((val >> 32) & 0xFFFFFFUL);

namespace Charra
{
	union fVec2
	{
		struct
		{
			float x, y;
		};
		struct
		{
			float offset, size;
		};
		struct 
		{
			float width, height;
		};
		float elements[2];
	};
	
	union Vec2
	{
		struct
		{
			uint32_t x, y;
		};
		struct
		{
			uint32_t offset, size;
		};
		struct
		{
			uint32_t width, height;
		};
		uint32_t elements[2];
	};

	union fVec3
	{
		struct 
		{
			float x, y, z;
		};
		struct
		{
			float r, g, b;
		};
		struct
		{
			float width, height, length;
		};
		float elements[3];
	};
	
	union Vec3
	{
		struct
		{
			uint32_t x, y, z;
		};
		struct
		{
			uint32_t r, g, b;
		};
		struct
		{
			uint32_t width, height, length;
		};
		uint32_t elements[3];
	};

	union fVec4
	{
		struct 
		{
			float x, y, z, w;
		};
		struct 
		{
			float r, g, b, a;
		};
		struct
		{
			float elements[4];
		};
		
	};
	
	union Vec4
	{
		struct 
		{
			uint32_t x, y, z, w;
		};
		struct
		{
			uint32_t r, g, b, a;
		};
		struct
		{
			uint32_t elements[4];
		};
		
	};
}