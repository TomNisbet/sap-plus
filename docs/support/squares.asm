; Find all squares between 0.255 and display on the Output Register
;
; This does not require any stack operations, so SP is used as a
; general-purpose register.

        data
NUM:    byte

        code
START:
        lai     0       ; output zero as the first square
        out
        sam     NUM     ; save as first value of NUM
NEXT:
        lam     NUM     ; find the next square.  Don't need to check overflow
        ina             ;   here because NUM * NUM will overflow before NUM
        sam     NUM
        tas             ; Add NUM to itself NUM times.  SP is the add counter
MULT:
        dcs             ; see if more adds are needed
        jz      MDONE   ; done - NUM has been added to itself NUM times
                        ; could check for borrow here and return 0 to check for multiply by zero
        adm     NUM     ; add current sum to NUM
        jnc     MULT    ; keep adding if no overflow
        jmp     NOMORE  ; NUM is too big to square

MDONE:
        out             ; multiplication complete, output result
        jmp     NEXT    ; do next value of NUM

NOMORE:
        lai     0xf0    ; no more squares to find.  Wait here for a moment,
        cyn             ;   blink the lights, then start over
        cyn
        jmp     START
