#include "global_defs.h"
#include "os_info.h"

#pragma once

namespace utils
{
	namespace os_info
	{
		static ULONG dw_build_number{};

		bool initialize_os_version()
		{
			OSVERSIONINFOW os_version;
			utils::internal_functions::pfn_rtl_get_version  (&os_version);

			dw_build_number = os_version.dwBuildNumber;
			return true;
		}

		bool is_x64_system()
		{
			// 使用编译时常量判断当前代码是为 x64 平台编译
#if defined(_WIN64)
			return true;
#else
			return false;
#endif
		}

		bool is_x86_system()
		{
			// 使用编译时常量判断当前代码是为 x86 平台编译
#if defined(_WIN32) && !defined(_WIN64)
			return true;
#else
			return false;
#endif
		}

		ULONG get_build_number()
		{
			 
				 
			OSVERSIONINFOW os_version;
			utils::internal_functions::pfn_rtl_get_version(&os_version);
			dw_build_number = os_version.dwBuildNumber;
			 

			return dw_build_number;
		}

		BOOLEAN is_equal_static(UINT32 a_build_number)
		{
			if (!dw_build_number)
			{
				OSVERSIONINFOW os_version;
				utils::internal_functions::pfn_rtl_get_version(&os_version);
				dw_build_number = os_version.dwBuildNumber;
			}
			return dw_build_number == a_build_number ? TRUE : FALSE;
		}

		BOOLEAN is_less_static(UINT32 a_build_number)
		{
			if (!dw_build_number)
			{
				OSVERSIONINFOW os_version;
				utils::internal_functions::pfn_rtl_get_version(&os_version);
				dw_build_number = os_version.dwBuildNumber;
			}
			return dw_build_number < a_build_number ? TRUE : FALSE;
		}

		BOOLEAN is_or_less_static(UINT32 a_build_number)
		{
			if (!dw_build_number)
			{
				OSVERSIONINFOW os_version;
				utils::internal_functions::pfn_rtl_get_version(&os_version);
				dw_build_number = os_version.dwBuildNumber;
			}
			return dw_build_number <= a_build_number ? TRUE : FALSE;
		}

		BOOLEAN is_greater_static(UINT32 a_build_number)
		{
			if (!dw_build_number)
			{
				OSVERSIONINFOW os_version;
				utils::internal_functions::pfn_rtl_get_version(&os_version);
				dw_build_number = os_version.dwBuildNumber;
			}
			return dw_build_number > a_build_number ? TRUE : FALSE;
		}

		BOOLEAN is_or_greater_static(UINT32 a_build_number)
		{
			if (!dw_build_number)
			{
				OSVERSIONINFOW os_version;
				utils::internal_functions::pfn_rtl_get_version(&os_version);
				dw_build_number = os_version.dwBuildNumber;
			}
			return dw_build_number >= a_build_number ? TRUE : FALSE;
		}

#define ring0_basic_os_version_define_is_build_number_routines_macro(a_number) \
    BOOLEAN is##a_number##_static() \
    { \
        return is_equal_static(a_number); \
    } \
    \
    BOOLEAN is_less##a_number##_static() \
    { \
        return is_less_static(a_number); \
    } \
    \
    BOOLEAN is##a_number##_or_less_static() \
    { \
        return is_or_less_static(a_number); \
    } \
    \
    BOOLEAN is_greater##a_number##_static() \
    { \
        return is_greater_static(a_number); \
    } \
    \
    BOOLEAN is##a_number##_or_greater_static() \
    { \
        return is_or_greater_static(a_number); \
    }

		ring0_basic_os_version_define_is_build_number_routines_macro(2600);
		ring0_basic_os_version_define_is_build_number_routines_macro(3790);
		ring0_basic_os_version_define_is_build_number_routines_macro(6000);
		ring0_basic_os_version_define_is_build_number_routines_macro(6001);
		ring0_basic_os_version_define_is_build_number_routines_macro(6002);
		ring0_basic_os_version_define_is_build_number_routines_macro(7600);
		ring0_basic_os_version_define_is_build_number_routines_macro(7601);
		ring0_basic_os_version_define_is_build_number_routines_macro(9200);
		ring0_basic_os_version_define_is_build_number_routines_macro(9600);
		ring0_basic_os_version_define_is_build_number_routines_macro(10240);
		ring0_basic_os_version_define_is_build_number_routines_macro(10586);
		ring0_basic_os_version_define_is_build_number_routines_macro(14393);
		ring0_basic_os_version_define_is_build_number_routines_macro(15063);
		ring0_basic_os_version_define_is_build_number_routines_macro(16299);
		ring0_basic_os_version_define_is_build_number_routines_macro(17134);
		ring0_basic_os_version_define_is_build_number_routines_macro(17763);
		ring0_basic_os_version_define_is_build_number_routines_macro(18362);
		ring0_basic_os_version_define_is_build_number_routines_macro(18363);
		ring0_basic_os_version_define_is_build_number_routines_macro(19041);
		ring0_basic_os_version_define_is_build_number_routines_macro(19042);
		ring0_basic_os_version_define_is_build_number_routines_macro(19043);
		ring0_basic_os_version_define_is_build_number_routines_macro(19044);
		ring0_basic_os_version_define_is_build_number_routines_macro(22000);
		ring0_basic_os_version_define_is_build_number_routines_macro(22621);
		ring0_basic_os_version_define_is_build_number_routines_macro(22631);
		ring0_basic_os_version_define_is_build_number_routines_macro(25300);
	}
}
