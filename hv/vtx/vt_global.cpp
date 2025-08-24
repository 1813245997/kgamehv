#include "../utils/global_defs.h"
#include "compatibility_checks.h"
#include "vt_global.h"

/**
 * @brief Bitmap that marks MSRs which will cause #GP (General Protection Fault)
 */
unsigned  long long* g_invalid_msr_bitmap{};

/**
 * @brief Bitmap for MSRs in the synthetic range (0x40000000 ~ 0x400000FF)
 * Used to record which synthetic MSRs cause #GP
 */
unsigned  long long* g_invalid_synthetic_msr_bitmap{};


COMPATIBILITY_CHECKS_STATUS g_compatibility_check{};