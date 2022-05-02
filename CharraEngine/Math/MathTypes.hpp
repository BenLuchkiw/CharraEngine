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
		uint32_t& operator[](size_t index) { return elements[index]; }

		iVec2 operator+(const iVec2& other) { return iVec2(other.x+x, other.y+y); }
		iVec2 operator-(const iVec2& other) { return iVec2(other.x-x, other.y-y); }
		iVec2 operator*(const iVec2& other) { return iVec2(other.x*x, other.y*y); }
		iVec2 operator/(const iVec2& other) { return iVec2(other.x/x, other.y/y); }

		void operator+=(const iVec2& other) { x+=other.x; y+=other.y; }
		void operator-=(const iVec2& other) { x-=other.x; y-=other.y; }
		void operator*=(const iVec2& other) { x*=other.x; y*=other.y; }
		void operator/=(const iVec2& other) { x/=other.x; y/=other.y; }

		iVec2 add(iVec2& other) { return *this+other; }
		iVec2 subtract(iVec2& other) { return *this-other; }
		iVec2 multiply(iVec2& other) { return *this*other; }
		iVec2 divide(iVec2& other) { return *this/other; }

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

	class iVec3
	{
	public:
		iVec3(iVec2 vec, uint32_t c) :x(vec.x), y(vec.y), z(c) {}
		iVec3(uint32_t a, iVec2 vec) :x(a), y(vec.y), z(vec.x) {}

		iVec3(uint32_t a, uint32_t b, uint32_t c) :x(a), y(b), z(c) {}
		iVec3() :x(0), y(0), z(0) {}

		void operator=(const iVec3& val) { x=val.x, y=val.y, z=val.z; }
		uint32_t& operator[](size_t size) { return elements[size]; }

		iVec3 operator+(iVec3& other) { return iVec3(x+other.x, y+other.y, z+other.z); }
		iVec3 operator-(iVec3& other) { return iVec3(x-other.x, y-other.y, z-other.z); }
		iVec3 operator*(iVec3& other) { return iVec3(x*other.x, y*other.y, z*other.z); }
		iVec3 operator/(iVec3& other) { return iVec3(x/other.x, y/other.y, z/other.z); }

		void operator+=(iVec3& other) { x+=other.x; y+=other.y; z+=other.z; }
		void operator-=(iVec3& other) { x-=other.x; y-=other.y; z-=other.z; }
		void operator*=(iVec3& other) { x*=other.x; y*=other.y; z*=other.z; }
		void operator/=(iVec3& other) { x/=other.x; y/=other.y; z/=other.z; }

		iVec3 add(iVec3& other) { return *this+other; }
		iVec3 subtract(iVec3& other) { return *this-other; }
		iVec3 multiply(iVec3& other) { return *this*other; }
		iVec3 divide(iVec3& other) { return *this/other; }

		union
		{
			struct
			{
				uint32_t x, y, z;
			};
			struct
			{
				uint32_t r, g, b;
			};
			uint32_t elements[3];
		};
	private:

	};

	class iVec4
	{
	public:
		// iVec2 creation
		iVec4(iVec2 vecA, iVec2 vecB) :x(vecA.x), y(vecB.x), w(vecB.y) {}
		iVec4(iVec2 vec, uint32_t c, uint32_t d) :x(vec.x), y(vec.y), z(c), w(d) {}
		iVec4(uint32_t a, iVec2 vec, uint32_t d) :x(a), y(vec.x), z(vec.y), w(d) {}
		iVec4(uint32_t a, uint32_t b, iVec2 vec) :x(a), y(b), z(vec.x), w(vec.y) {}

		// iVec3 creation
		iVec4(iVec3 vec, uint32_t d) :x(vec.x), y(vec.y), z(vec.z), w(d) {}
		iVec4(uint32_t a, iVec3 vec) :x(a), y(vec.x), z(vec.y), w(vec.z) {}

		// Regualar creation
		iVec4(uint32_t a, uint32_t b, uint32_t c, uint32_t d) :x(a), y(b), z(c), w(d) {}
		iVec4() :x(0), y(0), z(0), w(0) {}

		void operator=(const iVec4& val) { x=val.x, y=val.y, z=val.z, w=val.w; }
		uint32_t& operator[](size_t size) { return elements[size]; }

		iVec4 operator+(iVec4& other) { return iVec4(x+other.x, y+other.y, z+other.z, w+other.w); }
		iVec4 operator-(iVec4& other) { return iVec4(x-other.x, y-other.y, z-other.z, w-other.w); }
		iVec4 operator*(iVec4& other) { return iVec4(x*other.x, y*other.y, z*other.z, w*other.w); }
		iVec4 operator/(iVec4& other) { return iVec4(x/other.x, y/other.y, z/other.z, w/other.w); }

		void operator+=(iVec4& other) { x+=other.x; y+=other.y; z+=other.z; w+=other.w; }
		void operator-=(iVec4& other) { x-=other.x; y-=other.y; z-=other.z; w-=other.w; }
		void operator*=(iVec4& other) { x*=other.x; y*=other.y; z*=other.z; w*=other.w; }
		void operator/=(iVec4& other) { x/=other.x; y/=other.y; z/=other.z; w/=other.w; }

		iVec4 add(iVec4& other) { return *this+other; }
		iVec4 subtract(iVec4& other) { return *this-other; }
		iVec4 multiply(iVec4& other) { return *this*other; }
		iVec4 divide(iVec4& other) { return *this/other; }

		union
		{
			struct
			{
				uint32_t x, y, z, w;
			};
			struct
			{
				uint32_t r, g, b, a;
			};
			uint32_t elements[4];
		};
	private:

	};

	class fVec2
	{
	public:
		fVec2(float a, float b) :x(a), y(b) {}
		fVec2() :x(0.0f), y(0.0f) {}

		void operator=(const fVec2& val) { x=val.x, y=val.y; }
		float& operator[](size_t size) { return elements[size]; }

		fVec2 operator+(fVec2& other) { return fVec2(other.x+x, other.y+y); }
		fVec2 operator-(fVec2& other) { return fVec2(other.x-x, other.y-y); }
		fVec2 operator*(fVec2& other) { return fVec2(other.x*x, other.y*y); }
		fVec2 operator/(fVec2& other) { return fVec2(other.x/x, other.y/y); }

		void operator+=(fVec2& other) { x+=other.x; y+=other.y; }
		void operator-=(fVec2& other) { x-=other.x; y-=other.y; }
		void operator*=(fVec2& other) { x*=other.x; y*=other.y; }
		void operator/=(fVec2& other) { x/=other.x; y/=other.y; }

		fVec2 add(fVec2& other) { return *this+other; }
		fVec2 subtract(fVec2& other) { return *this-other; }
		fVec2 multiply(fVec2& other) { return *this*other; }
		fVec2 divide(fVec2& other) { return *this/other; }

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
		fVec3(fVec2 vec, float c) :x(vec.x), y(vec.y), z(c) {}
		fVec3(float a, fVec2 vec) :x(a), y(vec.x), z(vec.y) {}

		fVec3(float a, float b, float c) :x(a), y(b), z(c) {}
		fVec3() :x(0.0f), y(0.0f), z(0.0f) {}

		void operator=(const fVec3& val) { x=val.x, y=val.y, z=val.z; }
		float& operator[](size_t size) { return elements[size]; }

		fVec3 operator+(fVec3& other) { return fVec3(x+other.x, y+other.y, z+other.z); }
		fVec3 operator-(fVec3& other) { return fVec3(x-other.x, y-other.y, z-other.z); }
		fVec3 operator*(fVec3& other) { return fVec3(x*other.x, y*other.y, z*other.z); }
		fVec3 operator/(fVec3& other) { return fVec3(x/other.x, y/other.y, z/other.z); }

		void operator+=(fVec3& other) { x+=other.x; y+=other.y; z+=other.z; }
		void operator-=(fVec3& other) { x-=other.x; y-=other.y; z-=other.z; }
		void operator*=(fVec3& other) { x*=other.x; y*=other.y; z*=other.z; }
		void operator/=(fVec3& other) { x/=other.x; y/=other.y; z/=other.z; }

		fVec3 add(fVec3& other) { return *this+other; }
		fVec3 subtract(fVec3& other) { return *this-other; }
		fVec3 multiply(fVec3& other) { return *this*other; }
		fVec3 divide(fVec3& other) { return *this/other; }

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
		// fVec2 creation
		fVec4(fVec2 vecA, fVec2 vecB) :x(vecA.x), y(vecB.x), w(vecB.y) {}
		fVec4(fVec2 vec, float c, float d) :x(vec.x), y(vec.y), z(c), w(d) {}
		fVec4(float a, fVec2 vec, float d) :x(a), y(vec.x), z(vec.y), w(d) {}
		fVec4(float a, float b, fVec2 vec) :x(a), y(b), z(vec.x), w(vec.y) {}

		// fVec3 creation
		fVec4(fVec3 vec, float d) :x(vec.x), y(vec.y), z(vec.z), w(d) {}
		fVec4(float a, fVec3 vec) :x(a), y(vec.x), z(vec.y), w(vec.z) {}

		// Regualar creation
		fVec4(float a, float b, float c, float d) :x(a), y(b), z(c), w(d) {}
		fVec4() :x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

		void operator=(const fVec4& val) { x=val.x, y=val.y, z=val.z, w=val.w; }
		float& operator[](size_t size) { return elements[size]; }

		fVec4 operator+(fVec4& other) { return fVec4(x+other.x, y+other.y, z+other.z, w+other.w); }
		fVec4 operator-(fVec4& other) { return fVec4(x-other.x, y-other.y, z-other.z, w-other.w); }
		fVec4 operator*(fVec4& other) { return fVec4(x*other.x, y*other.y, z*other.z, w*other.w); }
		fVec4 operator/(fVec4& other) { return fVec4(x/other.x, y/other.y, z/other.z, w/other.w); }

		void operator+=(fVec4& other) { x+=other.x; y+=other.y; z+=other.z; w+=other.w; }
		void operator-=(fVec4& other) { x-=other.x; y-=other.y; z-=other.z; w-=other.w; }
		void operator*=(fVec4& other) { x*=other.x; y*=other.y; z*=other.z; w*=other.w; }
		void operator/=(fVec4& other) { x/=other.x; y/=other.y; z/=other.z; w/=other.w; }

		fVec4 add(fVec4& other) { return *this+other; }
		fVec4 subtract(fVec4& other) { return *this-other; }
		fVec4 multiply(fVec4& other) { return *this*other; }
		fVec4 divide(fVec4& other) { return *this/other; }

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
			struct
			{
				fVec3 vecABC;
				float w;
			};
			
			float elements[4];
		};
	private:

	};

	//class Mat3X3
	//{
	//public:
	//	Mat3X3() : a(), b(), c() {}
	//	Mat3X3(fVec3 A, fVec3 B, fVec3 C) :a(A), b(B), c(C) {}
	//
	//	void operator=(Mat3X3& copy) { a=copy.a; b=copy.b; c=copy.c; }
	//	Mat3X3& operator[](size_t pos) { *(fVec3*)&elements[pos*3]; }
	//
	//private:
	//	union
	//	{
	//		struct
	//		{
	//			fVec3 a, b, c;
	//		};
	//		float elements[9];
	//	};
	//};

	class Mat4X3
	{
	public:
		Mat4X3(fVec3 A, fVec3 B, fVec3 C, fVec3 D) :a(A), b(B), c(C), d(D) {}
		Mat4X3() :a(), b(), c(), d() {}

		void operator=(Mat4X3& copy) { a=copy.a; b=copy.b; c=copy.c; d=copy.d; }
		fVec3& operator[](size_t size) { return *(fVec3*)&elements[size*3]; }

	private:
		// Row major
		union
		{
			struct
			{
				fVec3 a, b, c, d;
			};
			float elements[12];
		};
	};

	class Mat4X4
	{
	public:
		Mat4X4() :a(), b(), c(), d() {}
		Mat4X4(fVec4 A, fVec4 B, fVec4 C, fVec4 D) :a(A), b(B), c(C), d(D) {}

		void operator=(Mat4X4& val) { a=val.a; b=val.b; c=val.c; d=val.d; }
		fVec4& operator[](size_t pos) { return *(fVec4*)&elements[4*pos]; }
		
		Mat4X3& getMat4X3() { return matA; }

	private:
		// Row major
		union
		{
			struct
			{
				fVec4 a, b, c, d;
			};
			struct
			{
				Mat4X3 matA;
				fVec4 vecD;
			};
			float elements[16];
		};
	};
}