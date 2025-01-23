; when doing looping to find divisors, stop if DIV is more than NUM/2.  Don't have an easy way to
; compute NUM/2, but can do DIV*2 instead and compare to NUM.  For example, there is no need to test
; past 11 to know that 23 is prime.  No reason to divide by 13, 17, and 19.

x123    equ      222
        data
NUM:    byte            ; number value to be tested for prime
DIV:    byte            ; current divisor value for prime test
DIVIX:  byte            ; index in the primes array of DIV    ***  NOT NEEDED????? ***
ENDIX:  byte            ; address of most recently found prime

        data    0xff
PRIMES: byte            ; array of prime numbers.  Starts at the top of memory and grows down

        code
START:
        lai     1       ; store 1 as the first prime and output it
        sam     PRIMES
        out
        ina             ; start with 2 as the first number to test for prime
        sam     NUM

        lai     PRIMES  ; initialize the current and last prime to be the first and only entry
        sam     DIVIX ; --- IS DIVIX NEEDED or can it just be SP
        sam     ENDIX
NXTPRM:
        ; Check if the current value of NUM is prime by dividing it by all primes smaller than NUM/2
        lai     PRIMES  ; No stack needed, so use the SP as the pointer into the PRIMES array
        tas             ; The first prime is one, but PLA decrements SP before reading, so the first
                        ;   DIV used will be the second prime (which won't exist the first time through)
NXTDIV:
        ; Check if NUM is prime by dividing it by the current prime value pointed to by SP
        tsa             ; If SP = ENDIX (counts down) then no more primes to test against, so NUM is prime
        cpm     ENDIX
        jz      ISPRM   ; SP = ENDIX
        pla             ; Decrement SP and get next divisor from primes list.  Store it in DIV.
        sam     DIV     ; If DIV > 127 or DIV > (NUM/2) then it is too big to evenly divide any possible
        asl             ;   value of NUM.  This means that all useful DIVs have been tested, so NUM is prime.
                        ; Note there is no need to check DIV=(NUM/2) because that would have been caught when DIV=2.
        jc      ISPRM   ; DIV > 127
        cpm     NUM
        jc      ISPRM   ; DIV > (NUM/2)
        lam     NUM     ; Load NUM into A to prepare for repeated subtraction loop
ISDIV:
        ; Check if NUM is evenly divisible by DIV using repeated subtraction.
        sbm     DIV     ; subtract DIV from what remains of NUM in A
        jz      NEXT    ; zero means it is divisible, so this number is not prime
        jnc     NXTDIV  ; carry clear indicates underflow, not evenly divisible, try the next DIV
        jmp     ISDIV   ; division not done, keep subtracting

ISPRM:
        lam     ENDIX   ; point SP to the end of the PRIMES list
        dca             ;   and update it to add a new entry
        sam     ENDIX
        tas
        lam     NUM     ; output NUM and add it to the end of the list
        out
        pha             ; store NUM in the next entry in the PRIMES array and decrement SP

NEXT:
        lam     NUM     ; check the next highest number for prime
        ina
        jc      SPIN    ; all primes found
        sam     NUM
        jmp     NXTPRM

SPIN:
        lai     0x55    ; Loop here forever, but at least look cool while doing it
SPIN2:
        cyn
        jmp     SPIN2

