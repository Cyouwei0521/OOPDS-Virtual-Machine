; ===========================
; Program 3: Factorial of 4 
; ===========================

MOV R1, 1       ; Product accumulator
MOV R2, 4       ; Counter

; --- Factorial Loop via manual unrolling ---
MUL R1, R2      ; 1 * 4 = 4
SUB R2, 1       ; 3

MUL R1, R2      ; 4 * 3 = 12
SUB R2, 1       ; 2

MUL R1, R2      ; 12 * 2 = 24
SUB R2, 1       ; 1

MUL R1, R2      ; 24 * 1 = 24
SUB R2, 1       ; 0 (R1 now holds 24)

; --- Test DIV and RESET ---
MOV R3, 2
DIV R1, R3      ; 24 / 2 = 12 (Testing DIV opcode)
MOV R4, 2
MUL R1, R4      ; 12 * 2 = 24 (Back to 24)

RESET OF          ; Reset system flags to clear OF
RESET UF          ; Reset system flags to clear UF
RESET CF          ; Reset system flags to clear CF
RESET ZF          ; Reset system flags to clear ZF

DISPLAY R1      ; Display 24