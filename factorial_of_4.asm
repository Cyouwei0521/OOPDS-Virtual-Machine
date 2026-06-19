; ===================================================
; Program 3: Factorial of 4
; Description: Calculates 4! (4 * 3 * 2 * 1) and displays 24
; ===================================================

MOV R1, 1     ; R1 will hold the running product (initial value = 1)
MOV R2, 4     ; R2 is counter/multiplier starting at 4

; --- Loop Start ---
MUL R1, R2    ; R1 = R1 * R2 (First iteration: 1 * 4 = 4)
SUB R2, 1     ; Decrement multiplier by 1 (R2 becomes 3)

MUL R1, R2    ; R1 = R1 * R2 (Second iteration: 4 * 3 = 12)
SUB R2, 1     ; Decrement multiplier by 1 (R2 becomes 2)

MUL R1, R2    ; R1 = R1 * R2 (Third iteration: 12 * 2 = 24)
SUB R2, 1     ; Decrement multiplier by 1 (R2 becomes 1)

MUL R1, R2    ; R1 = R1 * R2 (Fourth iteration: 24 * 1 = 24)
SUB R2, 1     ; Decrement multiplier by 1 (R2 becomes 0, loop finished)

DISPLAY R1    ; Display the final result (24)