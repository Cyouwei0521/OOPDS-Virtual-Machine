; ===================================================
; Program 1: Summing 5 values
; Description: Reads 5 inputs from user and sums them up
; ===================================================

INPUT R1      ; Read 1st number into R1
MOV R0, R1    ; Copy 1st number to R0 (R0 will store the running total)

INPUT R2      ; Read 2nd number into R2
ADD R0, R2    ; R0 = R0 + R2

INPUT R3      ; Read 3rd number into R3
ADD R0, R3    ; R0 = R0 + R3

INPUT R4      ; Read 4th number into R4
ADD R0, R4    ; R0 = R0 + R4

INPUT R5      ; Read 5th number into R5 
ADD R0, R5    ; R0 = R0 + R5

DISPLAY R0    ; Display the final total sum to screen

