INPUT R1          
INPUT R2          
INPUT R3  
INPUT R4         
       
MOV R0, 0          
ADD R0, R1          
ADD R0, R2         
ADD R0, R3  
ADD R0, R4        

SHR R0, 2
DISPLAY R0   
STORE 30, R0

SHL R0, 1
STORE 31, R0
   