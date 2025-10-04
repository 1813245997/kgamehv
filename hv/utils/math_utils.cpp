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

 

		bool world_to_screen(const Vector3* v, Vector3* out, matrix4x4_t* view_matrix, const POINT& screen_size)
		{
			matrix4x4_t  Pview = *view_matrix;
			Vector3  pos{};
			pos.x = v->x;
			pos.y = v->y;
			pos.z = v->z;
			Vector4 clipCoords;

			 
			clipCoords.x = pos.x * Pview[0] + pos.y * Pview[1] + pos.z * Pview[2] + Pview[3];
			clipCoords.y = pos.x * Pview[4] + pos.y * Pview[5] + pos.z * Pview[6] + Pview[7];
			clipCoords.z = pos.x * Pview[8] + pos.y * Pview[9] + pos.z * Pview[10] + Pview[11];
			clipCoords.w = pos.x * Pview[12] + pos.y * Pview[13] + pos.z * Pview[14] + Pview[15];

			 
			if (clipCoords.w < 0.01f)
				return false;

			 
			Vector3  NDC;
			NDC.x = clipCoords.x / clipCoords.w;
			NDC.y = clipCoords.y / clipCoords.w;
			NDC.z = clipCoords.z / clipCoords.w;

		 
			out->x = (screen_size.x / 2 * NDC.x) + (NDC.x + screen_size.x / 2);
			out->y = -(screen_size.y / 2 * NDC.y) + (NDC.y + screen_size.y / 2);
			out->z = v->z;   

			if (out->x < 0 || out->x > screen_size.x || out->y < 0 || out->y > screen_size.y)
				return false;

			return true;
		}

		float roundf(float a) {
			int ia = (int)a;
			float xs = a - (float)ia;
			if (xs >= .5f) {
				return a + 1.f;
			}
			return a;
		}

		float myfloorf(float a) {
			int ia = (int)a;
			float xs = a - (float)ia;
			if (xs > 0.f) {
				return a + 1.f;
			}
			return a;
		}
	}
}