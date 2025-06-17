

.code
mysqrt proc
    sqrtss xmm0, xmm0
    ret
mysqrt endp


; float __cosf(float a)
__cosf proc
    sub     rsp, 28h                         ; 16字节栈对齐 + 本地变量
    movss   dword ptr [rsp + 10h], xmm0      ; 把 a 从 xmm0 存到栈上

    fld     dword ptr [rsp + 10h]            ; 加载 float a 到 FPU
    fcos                                     ; 计算 cos(a)
    fstp    dword ptr [rsp + 10h]            ; 存储结果
    movss   xmm0, dword ptr [rsp + 10h]      ; 结果放回 xmm0 作为返回值

    add     rsp, 28h
    ret
__cosf endp


; float __sinf(float a)
__sinf proc
    sub     rsp, 28h
    movss   dword ptr [rsp + 10h], xmm0

    fld     dword ptr [rsp + 10h]
    fsin
    fstp    dword ptr [rsp + 10h]
    movss   xmm0, dword ptr [rsp + 10h]

    add     rsp, 28h
    ret
__sinf endp

 
; float __atan2f(float a, float b)
__atan2f proc
    sub     rsp, 28h                          ; 为栈对齐和局部变量预留空间（16字节对齐）
    movss   dword ptr [rsp + 10h], xmm0       ; 把 a 放到 [rsp+10h]
    movss   dword ptr [rsp + 14h], xmm1       ; 把 b 放到 [rsp+14h]

    fld     dword ptr [rsp + 14h]             ; 加载 b
    fld     dword ptr [rsp + 10h]             ; 加载 a
    fpatan                                     ; 计算 atan2(a, b)

    fstp    dword ptr [rsp + 10h]             ; 存回结果
    movss   xmm0, dword ptr [rsp + 10h]       ; 返回值放到 xmm0

    add     rsp, 28h
    ret
__atan2f endp


end