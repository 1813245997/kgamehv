#pragma once
namespace utils
{
    namespace os_info
    {
        extern  ULONG dw_build_number;
        /**
         * @brief Initializes the OS version information.
         *
         * This method initializes the operating system version information, such as major and minor versions, and stores it for further use.
         *
         * @return Returns `true` if the initialization was successful, otherwise returns `false`.
         */
         bool initialize_os_version();

        /**
         * @brief Determines whether the current operating system is x64.
         *
         * This method checks if the current operating system is running on an x64 architecture.
         *
         * @return Returns `true` if the system is x64, otherwise returns `false`.
         */
          bool is_x64_system();

        /**
         * @brief Determines whether the current operating system is x86.
         *
         * This method checks if the current operating system is running on an x86 architecture.
         *
         * @return Returns `true` if the system is x86, otherwise returns `false`.
         */
         bool is_x86_system();

        /**
         * @brief Returns the build number of the current operating system.
         *
         * This method returns the stored build number of the current operating system.
         *
         * @return Returns the build number as a `DWORD`.
         */
          ULONG get_build_number();


        /**
         * @brief Compares the stored build number with the provided build number.
         *
         * This method checks if the stored build number is equal to the provided build number.
         *
         * @param a_build_number The build number to compare with the stored build number.
         * @return Returns `TRUE` if the stored build number is equal to the provided build number, otherwise returns `FALSE`.
         */
          BOOLEAN is_equal_static(UINT32 a_build_number);

        /**
         * @brief Compares the stored build number with the provided build number.
         *
         * This method checks if the stored build number is less than the provided build number.
         *
         * @param a_build_number The build number to compare with the stored build number.
         * @return Returns `TRUE` if the stored build number is less than the provided build number, otherwise returns `FALSE`.
         */
          BOOLEAN is_less_static(UINT32 a_build_number);

        /**
         * @brief Compares the stored build number with the provided build number.
         *
         * This method checks if the stored build number is less than or equal to the provided build number.
         *
         * @param a_build_number The build number to compare with the stored build number.
         * @return Returns `TRUE` if the stored build number is less than or equal to the provided build number, otherwise returns `FALSE`.
         */
         BOOLEAN is_or_less_static(UINT32 a_build_number);

        /**
         * @brief Compares the stored build number with the provided build number.
         *
         * This method checks if the stored build number is greater than the provided build number.
         *
         * @param a_build_number The build number to compare with the stored build number.
         * @return Returns `TRUE` if the stored build number is greater than the provided build number, otherwise returns `FALSE`.
         */
          BOOLEAN is_greater_static(UINT32 a_build_number);

        /**
         * @brief Compares the stored build number with the provided build number.
         *
         * This method checks if the stored build number is greater than or equal to the provided build number.
         *
         * @param a_build_number The build number to compare with the stored build number.
         * @return Returns `TRUE` if the stored build number is greater than or equal to the provided build number, otherwise returns `FALSE`.
         */
          BOOLEAN is_or_greater_static(UINT32 a_build_number);

#define ring0_basic_os_version_declare_is_build_number_routines_macro(a_number) \
      BOOLEAN is##a_number##_static(); \
      BOOLEAN is_less##a_number##_static(); \
      BOOLEAN is##a_number##_or_less_static(); \
      BOOLEAN is_greater##a_number##_static(); \
      BOOLEAN is##a_number##_or_greater_static();


        ring0_basic_os_version_declare_is_build_number_routines_macro(2600);
        ring0_basic_os_version_declare_is_build_number_routines_macro(3790);
        ring0_basic_os_version_declare_is_build_number_routines_macro(6000);
        ring0_basic_os_version_declare_is_build_number_routines_macro(6001);
        ring0_basic_os_version_declare_is_build_number_routines_macro(6002);
        ring0_basic_os_version_declare_is_build_number_routines_macro(7600);
        ring0_basic_os_version_declare_is_build_number_routines_macro(7601);
        ring0_basic_os_version_declare_is_build_number_routines_macro(9200);
        ring0_basic_os_version_declare_is_build_number_routines_macro(9600);
        ring0_basic_os_version_declare_is_build_number_routines_macro(10240);
        ring0_basic_os_version_declare_is_build_number_routines_macro(10586);
        ring0_basic_os_version_declare_is_build_number_routines_macro(14393);
        ring0_basic_os_version_declare_is_build_number_routines_macro(15063);
        ring0_basic_os_version_declare_is_build_number_routines_macro(16299);
        ring0_basic_os_version_declare_is_build_number_routines_macro(17134);
        ring0_basic_os_version_declare_is_build_number_routines_macro(17763);
        ring0_basic_os_version_declare_is_build_number_routines_macro(18362);
        ring0_basic_os_version_declare_is_build_number_routines_macro(18363);
        ring0_basic_os_version_declare_is_build_number_routines_macro(19041);
        ring0_basic_os_version_declare_is_build_number_routines_macro(19042);
        ring0_basic_os_version_declare_is_build_number_routines_macro(19043);
        ring0_basic_os_version_declare_is_build_number_routines_macro(19044);
        ring0_basic_os_version_declare_is_build_number_routines_macro(22000);
        ring0_basic_os_version_declare_is_build_number_routines_macro(22621);
        ring0_basic_os_version_declare_is_build_number_routines_macro(22631);
        ring0_basic_os_version_declare_is_build_number_routines_macro(25300);
    }
}
