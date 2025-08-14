#include "global_defs.h"
#include "math_utils.h"


namespace utils {
	namespace math {
		 
		float  round(float x) {
			int i = (int)x;
			float frac = x - (float)i;

			if (x >= 0.0f) {
				if (frac >= 0.5f)
					return (float)(i + 1);
				else
					return (float)i;
			}
			else {
				if (frac <= -0.5f)
					return (float)(i - 1);
				else
					return (float)i;
			}
		}

		//double atof_kernel(const char* str)
		//{
		//	if (!str)
		//		return 0.0;

		//	double result = 0.0;
		//	int sign = 1;

		//	// 跳过空格
		//	while (*str == ' ' || *str == '\t')
		//		str++;

		//	// 处理符号
		//	if (*str == '-')
		//	{
		//		sign = -1;
		//		str++;
		//	}
		//	else if (*str == '+')
		//	{
		//		str++;
		//	}

		//	// 处理整数部分
		//	while (*str >= '0' && *str <= '9')
		//	{
		//		result = result * 10.0 + (*str - '0');
		//		str++;
		//	}

		//	// 处理小数部分
		//	if (*str == '.')
		//	{
		//		str++;
		//		double frac = 0.0;
		//		double base = 0.1;
		//		while (*str >= '0' && *str <= '9')
		//		{
		//			frac += (*str - '0') * base;
		//			base *= 0.1;
		//			str++;
		//		}
		//		result += frac;
		//	}

		//	// 不处理指数形式（e/E），可以按需扩展

		//	return result * sign;
		//}

		bool world_to_screen(const Vector3* v, Vector3* out, matrix4x4_t* view_matrix, const POINT& screen_size)
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

			if (out->x < 0 || out->x > screen_size.x || out->y < 0 || out->y > screen_size.y)
				return false;

			return true;
		}
	}
}