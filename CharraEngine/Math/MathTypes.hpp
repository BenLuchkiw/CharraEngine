#pragma once

#include <cstdint>

#define LOWER_UINT64(val) (uint32_t) (val & 0xFFFFFFUL);
#define UPPER_UINT64(val) (uint32_t) ((val >> 32) & 0xFFFFFFUL);

namespace Charra
{
	class iVec2
	{
	public:
		iVec2() :x(0), y(0) {}
		iVec2(uint32_t a, uint32_t b) :x(a), y(b) {}

		void operator=(const iVec2& val) { x=val.x, y=val.y; }

		union
		{
			struct
			{
				uint32_t x, y;
			};
			struct
			{
				uint32_t width, height;
			};		
			struct
			{
				uint32_t offset, size;
			};		
			uint32_t elements[2];
		};
		
	private:

	};

	class fVec2
	{
	public:
		fVec2(float a, float b) :x(a), y(b) {}
		fVec2() :x(0.0f), y(0.0f) {}

		void operator=(const fVec2& val) { x=val.x, y=val.y; }

		union
		{
			struct
			{
				float x, y;
			};
			struct
			{
				float width, height;
			};
			float elements[2];
		};
		
	private:

	};

	class fVec3
	{
	public:
		fVec3(float a, float b, float c) :x(a), y(b), z(c) {}
		fVec3() :x(0.0f), y(0.0f), z(0.0f) {}

		void operator=(const fVec3& val) { x=val.x, y=val.y, z=val.z; }

		union
		{
			struct
			{
				float x, y, z;
			};
			struct
			{
				float r, g, b;
			};
			float elements[3];
		};
	private:

	};

	class fVec4
	{
	public:
		fVec4(float a, float b, float c, float d) :x(a), y(b), z(c), w(d) {}
		fVec4() :x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

		void operator=(const fVec4& val) { x=val.x, y=val.y, z=val.z, w=val.w; }

		union
		{
			struct
			{
				float x, y, z, w;
			};
			struct
			{
				float r, g, b, a;
			};
			float elements[4];
		};
	private:

	};
}