#pragma once
#include "vmexit_handler.h"

void vmm_entrypoint();
void vmx_restore_state();
void vmx_save_state();

EXTERN_C void _setac();

EXTERN_C void _clearac();