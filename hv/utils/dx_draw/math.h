#pragma once
#include <cmath>
#define M_PI 3.14159265358979323846
#define M_PI_F ((float)(M_PI))
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))

 
 extern "C" float mysqrt(float x);
 extern "C" float __cosf(float a);
 extern "C" float __sinf(float a);
 extern "C" float __atan2f(float a, float b);
#define sqrt mysqrt
#define sqrtf mysqrt
#define abs myabs


 inline float myabs(float x) {
	 if (x < 0)
		 return -x;
	 return x;
 }

inline float __atanf(float a) {
	return __atan2f(a, 1.f);
}

inline float __acosf(float a) {
	return __atan2f(sqrtf(1.f - a * a), a);
}

 
struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};


//vector 3d

struct Vector3
{
	float x;
	float y;
	float z;
};


//class Vector3
//{
//public:
//	float x = 0, y = 0, z = 0;
//
//	Vector3()
//	{
//		x = y = z = 0.0f;
//	}
//
//	Vector3(float X, float Y, float Z)
//	{
//		x = X; y = Y; z = Z;
//	}
//
//	Vector3(float XYZ)
//	{
//		x = XYZ; y = XYZ; z = XYZ;
//	}
//
//	Vector3(float* v)
//	{
//		x = v[0]; y = v[1]; z = v[2];
//	}
//
//	Vector3(const float* v)
//	{
//		x = v[0]; y = v[1]; z = v[2];
//	}
//
//	inline Vector3& operator=(const Vector3& v)
//	{
//		x = v.x; y = v.y; z = v.z; return *this;
//	}
//
//	inline Vector3& operator=(const float* v)
//	{
//		x = v[0]; y = v[1]; z = v[2]; return *this;
//	}
//
//	inline float& operator[](int i)
//	{
//		return ((float*)this)[i];
//	}
//
//	inline float operator[](int i) const
//	{
//		return ((float*)this)[i];
//	}
//
//	void Vectorf(volatile float* v)
//	{
//		x = v[0];
//		y = v[1];
//		z = v[2];
//	}
//
//	inline Vector3& operator+=(const Vector3& v)
//	{
//		x += v.x; y += v.y; z += v.z; return *this;
//	}
//
//	inline Vector3& operator-=(const Vector3& v)
//	{
//		x -= v.x; y -= v.y; z -= v.z; return *this;
//	}
//
//	inline Vector3& operator*=(const Vector3& v)
//	{
//		x *= v.x; y *= v.y; z *= v.z; return *this;
//	}
//
//	inline Vector3& operator/=(const Vector3& v)
//	{
//		x /= v.x; y /= v.y; z /= v.z; return *this;
//	}
//
//	inline Vector3& operator+=(float v)
//	{
//		x += v; y += v; z += v; return *this;
//	}
//
//	inline Vector3& operator-=(float v)
//	{
//		x -= v; y -= v; z -= v; return *this;
//	}
//
//	inline Vector3& operator*=(float v)
//	{
//		x *= v; y *= v; z *= v; return *this;
//	}
//
//	inline Vector3& operator/=(float v)
//	{
//		x /= v; y /= v; z /= v; return *this;
//	}
//
//	inline Vector3 operator-() const
//	{
//		return Vector3(-x, -y, -z);
//	}
//
//	inline Vector3 operator+(const Vector3& v) const
//	{
//		return Vector3(x + v.x, y + v.y, z + v.z);
//	}
//
//	inline Vector3 operator-(const Vector3& v) const
//	{
//		return Vector3(x - v.x, y - v.y, z - v.z);
//	}
//
//	inline Vector3 operator*(const Vector3& v) const
//	{
//		return Vector3(x * v.x, y * v.y, z * v.z);
//	}
//
//	inline Vector3 operator/(const Vector3& v) const
//	{
//		return Vector3(x / v.x, y / v.y, z / v.z);
//	}
//
//	inline Vector3 operator+(float v) const
//	{
//		return Vector3(x + v, y + v, z + v);
//	}
//
//	inline Vector3 operator-(float v) const
//	{
//		return Vector3(x - v, y - v, z - v);
//	}
//
//	inline Vector3 operator*(float v) const
//	{
//		return Vector3(x * v, y * v, z * v);
//	}
//
//	inline Vector3 operator/(float v) const
//	{
//		return Vector3(x / v, y / v, z / v);
//	}
//
//	inline float Length() const
//	{
//		return (float)sqrtf(x * x + y * y + z * z);
//	}
//
//	float LengthXY() const {
//		return sqrtf(x * x + y * y);
//	}
//
//
//};

struct Vector2
{
	float x;
	float y;
	 
};

//class Vector2
//{
//public:
//	float x = 0, y = 0;
//
//	Vector2()
//	{
//		x = y = 0.0f;
//	}
//
//	Vector2(float X, float Y)
//	{
//		x = X; y = Y;
//	}
//
//	Vector2(float XYZ)
//	{
//		x = XYZ; y = XYZ;
//	}
//
//	Vector2(float* v)
//	{
//		x = v[0]; y = v[1];
//	}
//
//	Vector2(const float* v)
//	{
//		x = v[0]; y = v[1];
//	}
//
//	inline Vector2& operator=(const Vector2& v)
//	{
//		x = v.x; y = v.y; return *this;
//	}
//
//	inline Vector2& operator=(const float* v)
//	{
//		x = v[0]; y = v[1]; return *this;
//	}
//
//	inline float& operator[](int i)
//	{
//		return ((float*)this)[i];
//	}
//
//	inline float operator[](int i) const
//	{
//		return ((float*)this)[i];
//	}
//
//	void Vectorf(volatile float* v)
//	{
//		x = v[0];
//		y = v[1];
//	}
//
//	inline Vector2& operator+=(const Vector2& v)
//	{
//		x += v.x; y += v.y; return *this;
//	}
//
//	inline Vector2& operator-=(const Vector2& v)
//	{
//		x -= v.x; y -= v.y; return *this;
//	}
//
//	inline Vector2& operator*=(const Vector2& v)
//	{
//		x *= v.x; y *= v.y; return *this;
//	}
//
//	inline Vector2& operator/=(const Vector2& v)
//	{
//		x /= v.x; y /= v.y; return *this;
//	}
//
//	inline Vector2& operator+=(float v)
//	{
//		x += v; y += v; return *this;
//	}
//
//	inline Vector2& operator-=(float v)
//	{
//		x -= v; y -= v; return *this;
//	}
//
//	inline Vector2& operator*=(float v)
//	{
//		x *= v; y *= v; return *this;
//	}
//
//	inline Vector2& operator/=(float v)
//	{
//		x /= v; y /= v; return *this;
//	}
//
//	inline Vector2 operator-() const
//	{
//		return Vector2(-x, -y);
//	}
//
//	inline Vector2 operator+(const Vector2& v) const
//	{
//		return Vector2(x + v.x, y + v.y);
//	}
//
//	inline Vector2 operator-(const Vector2& v) const
//	{
//		return Vector2(x - v.x, y - v.y);
//	}
//
//	inline Vector2 operator*(const Vector2& v) const
//	{
//		return Vector2(x * v.x, y * v.y);
//	}
//
//	inline Vector2 operator/(const Vector2& v) const
//	{
//		return Vector2(x / v.x, y / v.y);
//	}
//
//	inline Vector2 operator+(float v) const
//	{
//		return Vector2(x + v, y + v);
//	}
//
//	inline Vector2 operator-(float v) const
//	{
//		return Vector2(x - v, y - v);
//	}
//
//	inline Vector2 operator*(float v) const
//	{
//		return Vector2(x * v, y * v);
//	}
//
//	inline Vector2 operator/(float v) const
//	{
//		return Vector2(x / v, y / v);
//	}
//
//	inline float Length() const
//	{
//		return (float)sqrtf(x * x + y * y);
//	}
//
//	float LengthXY() const {
//		return sqrtf(x * x + y * y);
//	}
//
//};
//

 

// 4x4 矩阵
struct matrix4x4_t {
	float view[16];

	float operator[](int index) {
		return view[index];
	}
};

//base math
//class Math
//{
//public:
//	static float Dot(const Vector3& v1, Vector3& v2) {
//		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
//	}
//	static Vector3 CalcAngle(const Vector3& Src, const Vector3& Dst)
//	{
//		auto delta = Src - Dst;
//		auto AimAngles = Vector3{
//			RAD2DEG(__atanf(delta.z / delta.LengthXY())),
//			RAD2DEG(__atanf(delta.y / delta.x)),
//			0.f
//		};
//
//		if (delta.x >= 0.f)
//			AimAngles.y += 180.f;
//
//		return AimAngles;
//	}
//
//	static void VectorTransform(const Vector3& in1, const matrix3x4_t& in2, Vector3& out) {
//		out.x = in1.x * in2[0][0] + in1.y * in2[0][1] + in1.z * in2[0][2] + in2[0][3];
//		out.y = in1.x * in2[1][0] + in1.y * in2[1][1] + in1.z * in2[1][2] + in2[1][3];
//		out.z = in1.x * in2[2][0] + in1.y * in2[2][1] + in1.z * in2[2][2] + in2[2][3];
//	}
//};


inline bool world_to_screen(_In_ Vector3* v, _In_opt_ Vector3 * out, _In_  matrix4x4_t* view_matrix, _In_ POINT& screen_size)
{

	matrix4x4_t  Pview = *view_matrix;
	Vector3  pos{};
	pos.x = v->x;
	pos.y = v->y;
	pos.z = v->z;
	Vector4 clipCoords;

	// 计算裁剪坐标
	clipCoords.x = pos.x * Pview[0] + pos.y * Pview[1] + pos.z * Pview[2] + Pview[3];
	clipCoords.y = pos.x * Pview[4] + pos.y * Pview[5] + pos.z * Pview[6] + Pview[7];
	clipCoords.z = pos.x * Pview[8] + pos.y * Pview[9] + pos.z * Pview[10] + Pview[11];
	clipCoords.w = pos.x * Pview[12] + pos.y * Pview[13] + pos.z * Pview[14] + Pview[15];

	// 如果 w 小于一个很小的值，返回 false
	if (clipCoords.w < 0.01f)
		return false;

	// 计算 NDC（规范化设备坐标）
	Vector3  NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	// 计算最终屏幕坐标
	out->x = (screen_size.x / 2 * NDC.x) + (NDC.x + screen_size.x / 2);
	out->y = -(screen_size.y / 2 * NDC.y) + (NDC.y + screen_size.y / 2);
	out->z = v->z;  // 保持原 z 坐标

	return true;
}