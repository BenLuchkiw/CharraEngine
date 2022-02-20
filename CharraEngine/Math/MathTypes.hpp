#pragma once

#include <cstdint>

#define LOWER_UINT64(val) (uint32_t) (val & 0xFFFFFFUL);
#define UPPER_UINT64(val) (uint32_t) ((val >> 32) & 0xFFFFFFUL);

namespace Charra
{
	union fVec2
	{
		float elements[2];
		struct
		{
			union 
			{
				float x, width;
			};
			union
			{
				float y, height;
			};
		};
	};

	union Vec2
	{
		uint32_t elements[2];
		struct
		{
			union 
			{
				uint32_t x, width;
			};
			union
			{
				uint32_t y, height;
			};
		};
	};

	union fVec3
	{
		float elements[3];
		struct 
		{
			union 
			{
				float x, r;
			};
			union 
			{
				float y, g;
			};
			union
			{
				float z, b;
			};
			
		};
		
	};

	union Vec3
	{
		uint32_t elements[3];
		struct 
		{
			union 
			{
				uint32_t x, r;
			};
			union 
			{
				uint32_t y, g;
			};
			union
			{
				uint32_t z, b;
			};
			
		};
		
	};
}