.CODE                                                                                                                                                                            

__writecr2 proc
    mov cr2,rcx
    ret
__writecr2 endp

__read_ldtr proc
    sldt ax
    ret
__read_ldtr endp

__read_tr proc
    str ax
    ret
__read_tr endp

__read_cs proc
    mov ax, cs
    ret
__read_cs endp

__read_ss proc
    mov ax, ss
    ret
__read_ss endp

__read_ds proc
    mov ax, ds
    ret
__read_ds endp

__read_es proc
    mov ax, es              
    ret
__read_es endp

__read_fs proc
    mov ax, fs
    ret
__read_fs endp

__read_gs proc
    mov ax, gs
    ret
__read_gs endp

__sgdt proc
    sgdt qword ptr [rcx]
    ret
__sgdt endp

__sidt proc
    sidt qword ptr [rcx]
    ret
__sidt endp

__load_ar proc
    lar rax, rcx
    jz no_error
    xor rax, rax
no_error:
    ret
__load_ar endp

 

?vmx_vmcall@hv@@YA_KAEAUhypercall_input@1@@Z proc
  ; move input into registers
  mov rax, [rcx]       ; code
  mov rdx, [rcx + 10h] ; args[1]
  mov r8,  [rcx + 18h] ; args[2]
  mov r9,  [rcx + 20h] ; args[3]
  mov r10, [rcx + 28h] ; args[4]
  mov r11, [rcx + 30h] ; args[5]
  mov rcx, [rcx + 08h] ; args[0]

  vmcall

  ret
?vmx_vmcall@hv@@YA_KAEAUhypercall_input@1@@Z endp

 

__reload_gdtr PROC
	push rcx
	shl rdx, 48
	push rdx
	lgdt fword ptr [rsp+6]
	pop rax
	pop rax
	ret
__reload_gdtr ENDP


__reload_idtr PROC
	push rcx
	shl	 rdx, 48
	push rdx
	lidt fword ptr [rsp+6]
	pop	rax
	pop	rax
	ret
__reload_idtr ENDP

__invept PROC
    invept rcx,oword ptr[rdx]
    ret
__invept ENDP

__invvpid PROC
    invvpid rcx,oword ptr[rdx]
    ret
__invvpid ENDP




END
