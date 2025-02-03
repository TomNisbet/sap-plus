; Test the stack using the PHA/PLA and JSR/RTS instructions

STKTOP  equ     0xff
N       equ     10

        lai     STKTOP  ; initialize the stack to the top of memory
        tas
        lai     0       ; Test stack - load value to A and push, change A, pop

LOOP:
        out             ; output and save A
        pha
        jsr     ADDN    ; output and save A+N
        pha
        jsr     ADDN    ; output and save A+2N
        pha
        jsr     ADDN    ; output A+3N
        pla
        out             ; restore and output A+2N
        pla
        out             ; restore and output A+N
        pla
        out             ; restore and output A
        ina             ; increment A and repeat the pattern
        jmp     LOOP

ADDN:   ; Add N to A and output the new value
        adi     N
        out
        rts
