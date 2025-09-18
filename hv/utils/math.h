#pragma once

 
#include <cmath>

#define M_PI 3.14159265358979323846
#define M_PI_F ((float)(M_PI))
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#ifndef INFINITY
#define INFINITY   (*(float*)&(unsigned int){0x7F800000})
#endif

#ifndef NAN
#define NAN        (*(float*)&(unsigned int){0x7FC00000})
#endif
 
 

 

 extern "C" float mysqrt(float x);
 extern "C" float __cosf(float a);
 extern "C" float __sinf(float a);
 extern "C" float __atan2f(float a, float b);
#define sqrt mysqrt
#define sqrtf mysqrt
#define abs myabs
#define absd myabsd


 inline float myabs(float x) {
	 return x < 0.0f ? -x : x;
 }

 inline double myabsd(double x) {
	 return x < 0.0 ? -x : x;
 }

inline float __atanf(float a) {
	return __atan2f(a, 1.f);
}

inline float __acosf(float a) {
	return __atan2f(sqrtf(1.f - a * a), a);
}
// 单精度快速 exp2
__forceinline float fast_exp2f(float p)
{
	float clipp = (p < -126.0f) ? -126.0f : p;
	union { unsigned long i; float f; } v;
	int w = (int)clipp;
	float z = clipp - (float)w;
	float t = (1.513906801f + z * (0.554305555f + z * 0.057578601f));
	v.i = (unsigned long)((w + 127) << 23) | ((*(unsigned long*)&t) & 0x7FFFFF);
	return v.f;
}



// 单精度 fabs
__forceinline float myabsf(float x) {
	unsigned long bits = *(unsigned long*)&x;
	bits &= 0x7FFFFFFFU;
	return *(float*)&bits;
}

// 内核版 modff
__forceinline float my_modff(float x, float* iptr) {
	int i = (int)x;
	*iptr = (float)i;
	return x - *iptr;
}

 

// 内核版 truncf（截取整数部分）
__forceinline float my_truncf(float x) {
	int i = (int)x; // 截取整数部分（向零舍入）
	return (float)i;
}

// 内核 / 驱动可用 fmodf
__forceinline float fmodf_drv(float x, float y) {
	if (y == 0.0f) return 0.0f; // 避免除零
	float div = x / y;
	float div_trunc = my_truncf(div);
	return x - y * div_trunc;
}

// 内核/驱动可用 cosf
__forceinline float cosf_drv(float x) {
	// 角度归约到 [-pi, pi]
	const float PI = 3.14159265f;
	const float TWO_PI = 6.28318531f;
	while (x > PI)  x -= TWO_PI;
	while (x < -PI) x += TWO_PI;

	// 再归约到 [-pi/2, pi/2]，使用 cos(-x) = cos(x)
	int sign = 1;
	if (x < -PI / 2) { x = -PI - x; sign = -1; }
	else if (x > PI / 2) { x = PI - x; sign = -1; }

	float x2 = x * x;

	// 泰勒展开 / 多项式逼近 cos(x) ≈ 1 - x^2/2! + x^4/4! - x^6/6!
	float result = 1.0f;
	float term = 1.0f;

	term *= -x2 / (2.0f);      result += term; // -x^2/2!
	term *= -x2 / (3.0f * 4.0f); result += term; // x^4/4!
	term *= -x2 / (5.0f * 6.0f); result += term; // -x^6/6!
	term *= -x2 / (7.0f * 8.0f); result += term; // x^8/8!

	return result * sign;
}

__forceinline float powf_drv(float base, float exponent) {
	/*UNREFERENCED_PARAMETER(base);
	UNREFERENCED_PARAMETER(exponent);*/
	//// 内部定义驱动可用 INFINITY 和 NAN
	//static const unsigned long drv_INFINITY_bits = 0x7F800000U;
	//static const unsigned long drv_NAN_bits = 0x7FC00000U;

	//static const float drv_INFINITY_f = *(const float*)&drv_INFINITY_bits;
	//static const float drv_NAN_f = *(const float*)&drv_NAN_bits;

	//// 特例处理
	//if (exponent == 0.0f) return 1.0f;
	//if (base == 0.0f) return (exponent > 0.0f) ? 0.0f : drv_INFINITY_f;
	//if (base < 0.0f) {
	//	// 负底数 & 非整数指数 → NaN
	//	int ei = (int)exponent;
	//	if ((float)ei != exponent) return drv_NAN_f;
	//	// 整数指数走快速幂
	//	float res = 1.0f;
	//	int n = (ei < 0) ? -ei : ei;
	//	float b = -base;
	//	while (n) {
	//		if (n & 1) res *= b;
	//		b *= b;
	//		n >>= 1;
	//	}
	//	return (ei < 0) ? 1.0f / res : res;
	//}

	//// 内核/驱动版 logf（多项式逼近）
	//float x = myabsf(base);
	//// 简单快速 logf 逼近
	//union FloatUInt32 {
	//	float f;
	//	uint32_t x;
	//};

	//FloatUInt32 vx;
	//vx.f = x;  // 用 float 初始化
	// 
	//float log2 = (float)(((vx.x >> 23) & 0xFF) - 127); // 指数部分
	//vx.x &= 0x7FFFFF;
	//vx.x |= 0x3F000000; // 尾数映射到 0.5~1
	//float y = vx.f;
	//float log2_frac = (y - 0.70710678f) / (y + 0.70710678f); // 简化逼近
	//float logf_val = 0.69314718f * (log2 + log2_frac * (2.0f + log2_frac * log2_frac * 0.6666667f));

	//// 内核/驱动版 expf（多项式逼近）
	//float z = exponent * logf_val;
	//float ip = (float)(int)z;
	//float fp = z - ip;
	//int e = (int)ip + 127;
	//if (e <= 0) return 0.0f;
	//if (e >= 255) return drv_INFINITY_f;
	//unsigned long ie = (unsigned long)e << 23;
	//union { unsigned long i; float f; } vb = { ie };
	//float base_exp = vb.f;
	//float poly = 1.0f + fp * (0.69314718f + fp * (0.24022651f + fp * (0.05550411f + fp * 0.00961813f)));

	//return base_exp * poly;

	return 0;
}



struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};


//vector 3d

class Vector3
{
public:

	float x;
	float y;
	float z;


public:
	float calculate_distance(const Vector3& point) const 
	{
		float dx = point.x - x;
		float dy = point.y - y;
		float dz = point.z - z;
		return (float)sqrtf(dx * dx + dy * dy + dz * dz);
	}


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


struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};
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


 
 