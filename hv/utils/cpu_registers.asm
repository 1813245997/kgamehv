.code

asm_read_rflags PROC
    pushfq
    pop rax
    ret
asm_read_rflags ENDP

asm_cli PROC
cli
ret
asm_cli ENDP


asm_sti PROC
sti
ret
asm_sti ENDP

asm_read_cr8 PROC
mov rax,cr8
ret
asm_read_cr8 ENDP


asm_write_cr8 PROC
mov cr8,rcx
ret
asm_write_cr8 ENDP

END
