; CSC612M MCO1 - Element-wise maximum with index tracking
; Bunyi, Chan, Umali

bits 64
default rel

global max_index_scalar

section .text
; rcx - remaining element count
; rdx - A pointer
; r8 - B pointer
; r9 - C pointer
; r10 - idx pointer
; xmm0 - A element
; xmm1 - B element
max_index_scalar:
    mov r10, [rsp + 8*5]
    cmp rcx, 0
    je done

L1:
    movss xmm0, [rdx]
    movss xmm1, [r8]
    ucomiss xmm0, xmm1
    jae L2
    movss [r9], xmm1
    mov dword [r10], 1
    jmp L3

L2:
    movss [r9], xmm0
    mov dword [r10], 0

L3:
    add rdx, 4
    add r8, 4
    add r9, 4
    add r10, 4
    dec rcx
    jne L1

done:
    ret
