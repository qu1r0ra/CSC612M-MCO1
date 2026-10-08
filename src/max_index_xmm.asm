; CSC612M MCO1 - Element-wise maximum with index tracking
; Bunyi, Chan, Umali

bits 64
default rel

global max_index_xmm

section .text
; rcx - remaining element count
; rdx - A pointer
; r8  - B pointer
; r9  - C pointer
; r10 - idx pointer
; xmm0 - A elements (4 packed floats)
; xmm1 - B elements (4 packed floats)
; xmm2 - comparison mask / shifted indices
; xmm3 - max elements
max_index_xmm:
    xor rax, rax
    mov r10, [rsp + 8*5]
    cmp rcx, 0
    je done

L1:
    cmp rcx, 4
    jb TAIL
    ; load elements
    vmovups xmm0, [rdx + rax*4]
    vmovups xmm1, [r8 + rax*4]

    ; get max elements
    vmaxps xmm3, xmm0, xmm1
    vmovups [r9 + rax*4], xmm3

    ; saving mask for index tracking
    vcmpgtps xmm2, xmm1, xmm0       ; -1 if B > A, else 0
    vpsrld xmm2, xmm2, 31           ; -1 to 1 and 0 to 0
    vmovdqu [r10 + rax*4], xmm2

   
    add rax, 4
    sub rcx, 4

    jg L1

    jmp done


TAIL:
    cmp rcx, 0
    je done

L2:
    movss xmm0, [rdx + rax*4]
    movss xmm1, [r8 + rax*4]
    ucomiss xmm0, xmm1
    jae L3
    movss [r9 + rax*4], xmm1
    mov dword [r10 + rax*4], 1
    jmp L4

L3:
    movss [r9 + rax*4], xmm0
    mov dword [r10 + rax*4], 0

L4:
    inc rax
    dec rcx
    jne L2

done:
    ret
