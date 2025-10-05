#include "global_defs.h"
#include "global_initializer.h"
 

namespace utils
{
	namespace  global
	{
		/**
 * @brief Bitmap that marks MSRs which will cause #GP (General Protection Fault)
 */
		 


		bool initialize_all_globals()
		{
			utils::os_info::dw_build_number = 0;

			utils::ssdt::g_ssdt_table =0;
			utils::ssdt::g_win32k_table=0;
			utils::kernel_dwm_drawing::g_dwm_render_lock = 0;
			utils::auth::g_license_expire_timestamp = 0;



			 memory::g_pte_base = 0;
			 memory::g_pde_base = 0;
			 memory::g_ppe_base = 0;
			 memory::g_pxe_base = 0;
			 return true;
		}
	}
}