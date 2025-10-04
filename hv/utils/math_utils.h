#pragma once
#include "math.h"
 
typedef struct tagPOINT
{
	int  x;
	int  y;
} POINT;
namespace utils {
	namespace math {
		float  round(float x);
		//double atof_kernel(const char* str);
		bool world_to_screen(const _In_ Vector3* v, _In_opt_ Vector3* out, _In_  matrix4x4_t* view_matrix, const  _In_ POINT& screen_size);

		float roundf(float a);
		
		float myfloorf(float a);

	}
}