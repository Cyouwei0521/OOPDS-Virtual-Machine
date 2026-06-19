; ===============================
; Program 2: Average of 4 values 
; ===============================

INPUT R1
MOV R0, R1      ; Start sum with 1st number

INPUT R2
ADD R0, R2

INPUT R3
ADD R0, R3

INPUT R4
ADD R0, R4      ; R0 = total sum of 4 numbers

; --- Test Memory Access ---
MOV R5, 10      ; Memory address 10
STORE 10, R0    ; Store total sum into memory address 10
MOV R0, 0       ; Clear R0 to prove LOAD works
LOAD R0, 10     ; Load total sum back into R0 from address 10

; --- Test Bitwise Fun (SHL, ROL, ROR) to cover opcodes ---
SHL R0, 1       ; Multiply by 2 (just for testing)
SHR R0, 1       ; Divide by 2 (back to original sum)
ROL R0, 2       ; Rotate left 2 positions
ROR R0, 2       ; Rotate right 2 positions (back to original sum)

; --- Final Average Calculation ---
SHR R0, 2       ; Divide by 4 using SHR to get the average

DISPLAY R0      ; Display final average