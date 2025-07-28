#pragma once

/**
 * @brief Bitmap that marks MSRs which will cause #GP (General Protection Fault)
 */
extern unsigned  long long* g_invalid_msr_bitmap;

/**
 * @brief Bitmap for MSRs in the synthetic range (0x40000000 ~ 0x400000FF)
 * Used to record which synthetic MSRs cause #GP
 */
extern 	unsigned  long long* g_invalid_synthetic_msr_bitmap;


extern COMPATIBILITY_CHECKS_STATUS g_compatibility_check;