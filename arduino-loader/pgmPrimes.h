static const uint8_t pgmPrimes[] = {
              //    ; when doing looping to find divisors, stop if DIV is more than NUM/2.  Don't have an easy way to
              //    ; compute NUM/2, but can do DIV*2 instead and compare to NUM.  For example, there is no need to test
              //    ; past 11 to know that 23 is prime.  No reason to divide by 13, 17, and 19.
              //    ;
              //    ; This program shows how the SP can be used as an index register with the PHA and LAX instructions.
              //    
              //            data
              // 00 NUM:    byte            ; number value to be tested for prime
              // 01 DIV:    byte            ; current divisor value for prime test
              // 02 ENDIX:  byte            ; address of most recently found prime
              //    
              //            data    0xff
              // ff PRIMES: byte            ; array of prime numbers.  Starts at the top of memory and grows down
              //    
              //            code
              //    START:
  0x02, 0x01, // 00         lai     1       ; store 1 as the first prime and output it
  0x04, 0xff, // 02         sam     PRIMES
  0x01,       // 04         out
  0x07,       // 05         ina             ; start with 2 as the first number to test for prime
  0x04, 0x00, // 06         sam     NUM
              //    
  0x02, 0xff, // 08         lai     PRIMES  ; initialize the last prime to be the first and only entry
  0x08,       // 0a         dca             ; ENDIX points to the next entry that will be used, not the
  0x04, 0x02, // 0b         sam     ENDIX   ; last existing entry, so SP=ENDIX means no entries remain
              //    NXTPRM:
              //            ; Check if the current value of NUM is prime by dividing it by all primes smaller than NUM/2
  0x02, 0xff, // 0d         lai     PRIMES  ; SP used as the pointer into the PRIMES array.
  0x05,       // 0f         tas             ; The first prime is one, so decrement SP to start dividing by the second prime.
  0x1e,       // 10         dcs             ;   The second prime won't exist the first time through the loop.
              //    NXTDIV:
              //            ; Check if NUM is prime by dividing it by the current prime value pointed to by SP
  0x06,       // 11         tsa             ; If SP = ENDIX (counts down) then no more primes to test against, so NUM is prime
  0x29, 0x02, // 12         cpm     ENDIX
  0x12, 0x2d, // 14         jeq     ISPRM   ; SP = ENDIX
  0x0e,       // 16         lax             ; Get divisor from primes list and store in DIV
  0x1e,       // 17         dcs             ; point SP to next divisor
  0x04, 0x01, // 18         sam     DIV     ; If DIV > 127 or DIV > (NUM/2) then it is too big to divide any possible
  0x0a,       // 1a         asl             ;   value of NUM.  This means that all useful DIVs have been tested, so NUM is prime.
              //                            ; The DIV=(NUM/2) only catches DIV=2,NUM=4. Larger values of NUM are caught would during division when DIV=2.
  0x11, 0x2d, // 1b         jc      ISPRM   ; DIV > 127
  0x29, 0x00, // 1d         cpm     NUM
  0x12, 0x37, // 1f         jeq     NEXT    ; DIV = (NUM/2)
  0x11, 0x2d, // 21         jge     ISPRM   ; DIV >= (NUM/2)
  0x03, 0x00, // 23         lam     NUM     ; Load NUM into A to prepare for repeated subtraction loop
              //    ISDIV:
              //            ; Check if NUM is evenly divisible by DIV using repeated subtraction.
  0x23, 0x01, // 25         sbm     DIV     ; subtract DIV from what remains of NUM in A
  0x12, 0x37, // 27         jz      NEXT    ; zero means it is divisible, so this number is not prime
  0x13, 0x11, // 29         jnc     NXTDIV  ; carry clear indicates underflow, NUM / DIV not evenly divisible, try the next DIV
  0x10, 0x25, // 2b         jmp     ISDIV   ; division not done, keep subtracting
              //    
              //    ISPRM:
  0x03, 0x02, // 2d         lam     ENDIX   ; point SP to the next available entry in the PRIMES list
  0x05,       // 2f         tas
  0x03, 0x00, // 30         lam     NUM     ; output NUM and add it to the end of the list
  0x01,       // 32         out
  0x15,       // 33         pha             ; store NUM in the next entry in the PRIMES array and decrement SP
  0x06,       // 34         tsa             ; store new index for the next available list entry
  0x04, 0x02, // 35         sam     ENDIX
              //    
              //    NEXT:
  0x03, 0x00, // 37         lam     NUM     ; check the next highest number for prime
  0x07,       // 39         ina
  0x11, 0x40, // 3a         jc      SPIN    ; all primes found
  0x04, 0x00, // 3c         sam     NUM
  0x10, 0x0d, // 3e         jmp     NXTPRM
              //    
              //    SPIN:
  0x02, 0x55, // 40         lai     0x55    ; Loop here forever, but at least look cool while doing it
              //    SPIN2:
  0x3f,       // 42         cyn
  0x10, 0x42, // 43         jmp     SPIN2
};
