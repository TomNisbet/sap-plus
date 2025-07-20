; Output all 8-bit prime numbers using the Sieve of Eratosthenes.
;
; The primes are found by filling memory locations 2..255 with the values 2..255.  This uses 254 of the
; available RAM locations. The other two locations store the next number to test and the next non-prime
; number to be marked.
;
; The algoritm works as follows:
;
; start at 2, output it as a prime, and then mark all multiples of 2 (4, 6, 8, 10,...)
; as non-prime by writing a zero to their memory locations.
;
; find the next non-zero memory location, which is 3.  Output it and mark multiples (6, 9, 12, ...)
; with zero.
;
; continue until the end of memory is reached
;
; This program shows how the SP can be used as an index register with the LAX and SAX instructions.

        data    0
NUM:    byte            ; number value to be tested for prime
ZEROIX: byte            ; next location to write zero

        data    2
SIEVE:  byte            ; array of numbers 2..255 stored in memory at address 2..255

        code
START:
        lai     2       ; store the starting NUM
        sam     NUM
        tas
FILL:
        sax             ; store the values 2..255 into memory locations 2..255
        ins
        tsa
        jne     FILL
        ; find the next non-zero entry and print it as a prime
        lam     NUM     ; try the next number
NXTPRM:
        tas             ; read from memory location [NUM]
        lax             ; check for zero value
        tst             ;
        jeq     NEXT    ; not prime if zero at this location
        out             ; non-zero means prime, so display
ZERO:
        sam     ZEROIX  ; zero out all multiples of NUM
        lai     0
        sax
        lam     ZEROIX
        adm     NUM
        tas
        jnc     ZERO

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
