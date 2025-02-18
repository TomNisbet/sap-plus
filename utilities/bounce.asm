; Simple test program to load a series of values into the A register.
; Requires only a minimal system with no ALU, Output Register, or stack.
        nop             ; start with a NOP to avoid reset/clock issues
        lai     1    
LOOP:
        lai     2    
        lai     4    
        lai     8    
        lai     16    
        lai     32    
        lai     64    
        lai     128    
        nop 
        lai     64    
        lai     32    
        lai     16    
        lai     8    
        lai     4    
        lai     2    
        lai     1    
        nop   
        jmp     LOOP

