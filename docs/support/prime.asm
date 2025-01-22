        data
NUM:    byte            ; number value to be tested for prime
DIV:    byte            ; current divisor value for prime test
DIVIX:  byte            ; index in the primes array of DIV
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

        lai     PRIMES  ; initialize the current and last prime to the list start
        sam     DIVIX ; --- IS DIVIX NEEDED or can it be SP
        sam     ENDIX
NXTPRM:
        ; Check if the current value of NUM is prime by dividing it by all primes smaller than NUM/2
        lai     PRIMES  ; No stack needed, so use the SP as the pointer into the PRIMES array
        dca
        tas
NXTDIV:
        ; Check if NUM is prime by dividing it by the current prime value pointed to by SP
        tsa             ; If SP < ENDIX (counts down) then no more primes to test against, so NUM is prime
        cpm     ENDIX
        jc      ISPRM   ; Found one!
        pla             ; Get divisor from primes list; also decrements SP to be ready to grab the next DIV
        sam     DIV     ; If DIV > 127 or DIV > (NUM/2) then it is too big to evenly divide any possible value
        asl             ;   of NUM.  This means that all useful DIVs have been tested, so NUM is prime.
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
;when doing looping to find divisors, stop if the div is more than a/2.  Don't have an a/2 operator
;but can do div*2 instead ad compare to A.  e.g. once we hit 11, then 23 is known prime.  No need
;to try 13, 17, and 19.

ISPRM:
        lam     ENDIX   ; point SP to the end of the PRIMES list
        tas
        lam     NUM     ; output NUM and add it to the end of the list
        out
        pha             ; bumps the SP down to the next entry and store NUM
        tsa             ; update ENDIX with new SP value
        sam     ENDIX

NEXT:
        lam     NUM     ; check the next highest number for prime
        ina
        jc      SPIN    ; all primes found
        sam     NUM
        jmp     NXTPRM

SPIN:
        lai     0x55    ; Loop here forever, but at least look cool while doing it
SPIN2:
        not
        jmp     SPIN2

