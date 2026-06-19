; ===================================================
; Program 2: Average of 4 values
; Description: Reads 4 inputs, sums them, and divides by 4 using SHR
; ===================================================

INPUT R1      ; Read 1st number
MOV R0, R1    ; Initialize total sum in R0 with the 1st number

INPUT R2      ; Read 2nd number
ADD R0, R2    ; R0 = R0 + R2

INPUT R3      ; Read 3rd number
ADD R0, R3    ; R0 = R0 + R3

INPUT R4      ; Read 4th number
ADD R0, R4    ; R0 = R0 + R4 (R0 now holds the total sum of 4 numbers)

; Divide the total sum by 4 by shifting right twice using SHR command
SHR R0, 2     ; R0 = R0 >> 2 (shifting right by 2 bits is exactly dividing by 4)

DISPLAY R0    ; Display the final average value

