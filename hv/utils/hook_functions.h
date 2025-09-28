#pragma once
#include "hook_utils.h"
namespace hook_functions
{

    
    /**
     * @brief Hook for KiPreprocessFault.
     */
    BOOLEAN __fastcall hook_ki_preprocess_fault(
        _Inout_ PEXCEPTION_RECORD ExceptionRecord,
        _Inout_ PCONTEXT ContextRecord,
        _In_ KPROCESSOR_MODE PreviousMode
    );

    extern BOOLEAN(__fastcall* original_ki_preprocess_fault)(
        _Inout_ PEXCEPTION_RECORD ExceptionRecord,
        _Inout_ PCONTEXT ContextRecord,
        _In_ KPROCESSOR_MODE PreviousMode
        );

  
}
