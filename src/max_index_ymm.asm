; CSC612M MCO1 - Element-wise maximum with index tracking
; Bunyi, Chan, Umali

bits 64
default rel

global max_index_ymm

section .text
; rcx - remaining element count
; rdx - A pointer
; r8  - B pointer
; r9  - C pointer
; r10 - idx pointer
; rax - current element offset
; ymm0 - A elements (8 packed floats)
; ymm1 - B elements (8 packed floats)
; ymm2 - comparison mask / shifted indices
; ymm3 - maximum elements
max_index_ymm:
    xor rax, rax
    mov r10, [rsp + 8*5]       
    test rcx, rcx
    je done

vector_loop:
    cmp rcx, 8
    jb tail

    ; load and store eight floats
    vmovups ymm0, [rdx + rax*4]
    vmovups ymm1, [r8 + rax*4]

    ; select the maximum and derive 0/1 indices from the strict B > A mask.
    vmaxps ymm3, ymm0, ymm1
    vmovups [r9 + rax*4], ymm3
    vcmpgtps ymm2, ymm1, ymm0 ; -1 if B > A, else 0
    vpsrld ymm2, ymm2, 31     ; -1 to 1 and 0 to 0
    vmovdqu [r10 + rax*4], ymm2

    add rax, 8
    sub rcx, 8
    jnz vector_loop
    jmp done

tail:
    ; process the remaining zero to seven elements without over-reading.
    test rcx, rcx
    je done

tail_loop:
    vmovss xmm0, [rdx + rax*4]
    vmovss xmm1, [r8 + rax*4]
    vucomiss xmm0, xmm1
    jae tail_a_wins

    vmovss [r9 + rax*4], xmm1
    mov dword [r10 + rax*4], 1
    jmp tail_next

tail_a_wins:
    vmovss [r9 + rax*4], xmm0
    mov dword [r10 + rax*4], 0

tail_next:
    inc rax
    dec rcx
    jne tail_loop

done:
    ; clear AVX upper halves before returning to caller code.
    vzeroupper
    ret
