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
	}
}