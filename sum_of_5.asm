; =============================
; Program 1: Summing 5 values 
; =============================

; --- 1. Get 5 inputs and push them to Stack ---
INPUT R1
PUSH R1
INPUT R1
PUSH R1
INPUT R1
PUSH R1
INPUT R1
PUSH R1
INPUT R1
PUSH R1

; --- 2. Pop and Sum them up ---
MOV R0, 0       ; Initialize sum = 0

POP R2          ; Get 5th number
ADD R0, R2

POP R2          ; Get 4th number
ADD R0, R2

POP R2          ; Get 3rd number
ADD R0, R2

POP R2          ; Get 2nd number
ADD R0, R2

POP R2          ; Get 1st number
ADD R0, R2      ; R0 now holds the total sum

; --- 3. Test INC and DEC just to cover opcodes ---
INC R0          ; sum = sum + 1
DEC R0          ; sum = sum - 1 (back to original sum)

DISPLAY R0      ; Display the final sum