; --- 1. Test Input and Basic Math ---
INPUT R0            ; Test User Input (e.g., input 50)
MOV R1, 2           ; Test Immediate Move
MUL R0, R1          ; 50 * 2 = 100
DISPLAY R0          ; Expected Output: 100

; --- 2. Test Arithmetic Overflow (OF) ---
ADD R0, 30          ; 100 + 30 = 130. Since 130 > 127, this triggers Overflow (OF)
DISPLAY R0          ; Expected Output: -126 (130 overflows 8-bit signed)

; --- 3. Test Stack Operations ---
PUSH R0             ; Push -126 onto the stack
POP R2              ; Pop into R2. R2 should now hold -126
DISPLAY R2

; --- 4. Test Memory (Direct and Indirect) ---
STORE 10, R2        ; Store -126 into Memory Address 10
LOAD R3, [10]       ; Load Address 10 into R3
DISPLAY R3          ; Expected Output: -126

; --- 5. Test Shifts and Rotation ---
MOV R4, 1           ; Load 1
SHL R4, 1           ; Shift Left by 1. R4 becomes 2
DISPLAY R4          ; Expected Output: 2

ROL R4, 1           ; Rotate Left by 1. 2 -> 4
DISPLAY R4          ; Expected Output: 4

; --- 6. Test Flag Reset ---
RESET OF            ; Clear the overflow flag we triggered earlier
RESET ZF            ; Clear the Zero flag