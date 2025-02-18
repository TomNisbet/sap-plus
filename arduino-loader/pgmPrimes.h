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
  0x00,       // 00         nop             ; start with a NOP to avoid reset/clock issues
              //    START:
  0x02, 0x01, // 01         lai     1       ; store 1 as the first prime and output it
  0x04, 0xff, // 03         sam     PRIMES
  0x01,       // 05         out
  0x07,       // 06         ina             ; start with 2 as the first number to test for prime
  0x04, 0x00, // 07         sam     NUM
              //    
  0x02, 0xff, // 09         lai     PRIMES  ; initialize the last prime to be the first and only entry
  0x08,       // 0b         dca             ; ENDIX points to the next entry that will be used, not the
  0x04, 0x02, // 0c         sam     ENDIX   ; last existing entry, so SP=ENDIX means no entries remain
              //    NXTPRM:
              //            ; Check if the current value of NUM is prime by dividing it by all primes smaller than NUM/2
  0x02, 0xff, // 0e         lai     PRIMES  ; SP used as the pointer into the PRIMES array.
  0x05,       // 10         tas             ; The first prime is one, so decrement SP to start dividing by the second prime.
  0x1e,       // 11         dcs             ;   The second prime won't exist the first time through the loop.
              //    NXTDIV:
              //            ; Check if NUM is prime by dividing it by the current prime value pointed to by SP
  0x06,       // 12         tsa             ; If SP = ENDIX (counts down) then no more primes to test against, so NUM is prime
  0x29, 0x02, // 13         cpm     ENDIX
  0x12, 0x2e, // 15         jeq     ISPRM   ; SP = ENDIX
  0x0e,       // 17         lax             ; Get divisor from primes list and store in DIV
  0x1e,       // 18         dcs             ; point SP to next divisor
  0x04, 0x01, // 19         sam     DIV     ; If DIV > 127 or DIV > (NUM/2) then it is too big to divide any possible
  0x0a,       // 1b         asl             ;   value of NUM.  This means that all useful DIVs have been tested, so NUM is prime.
              //                            ; The DIV=(NUM/2) only catches DIV=2,NUM=4. Larger values of NUM are caught would during division when DIV=2.
  0x11, 0x2e, // 1c         jc      ISPRM   ; DIV > 127
  0x29, 0x00, // 1e         cpm     NUM
  0x12, 0x38, // 20         jeq     NEXT    ; DIV = (NUM/2)
  0x11, 0x2e, // 22         jge     ISPRM   ; DIV >= (NUM/2)
  0x03, 0x00, // 24         lam     NUM     ; Load NUM into A to prepare for repeated subtraction loop
              //    ISDIV:
              //            ; Check if NUM is evenly divisible by DIV using repeated subtraction.
  0x23, 0x01, // 26         sbm     DIV     ; subtract DIV from what remains of NUM in A
  0x12, 0x38, // 28         jz      NEXT    ; zero means it is divisible, so this number is not prime
  0x13, 0x12, // 2a         jnc     NXTDIV  ; carry clear indicates underflow, NUM / DIV not evenly divisible, try the next DIV
  0x10, 0x26, // 2c         jmp     ISDIV   ; division not done, keep subtracting
              //    
              //    ISPRM:
  0x03, 0x02, // 2e         lam     ENDIX   ; point SP to the next available entry in the PRIMES list
  0x05,       // 30         tas
  0x03, 0x00, // 31         lam     NUM     ; output NUM and add it to the end of the list
  0x01,       // 33         out
  0x15,       // 34         pha             ; store NUM in the next entry in the PRIMES array and decrement SP
  0x06,       // 35         tsa             ; store new index for the next available list entry
  0x04, 0x02, // 36         sam     ENDIX
              //    
              //    NEXT:
  0x03, 0x00, // 38         lam     NUM     ; check the next highest number for prime
  0x07,       // 3a         ina
  0x11, 0x41, // 3b         jc      SPIN    ; all primes found
  0x04, 0x00, // 3d         sam     NUM
  0x10, 0x0e, // 3f         jmp     NXTPRM
              //    
              //    SPIN:
  0x02, 0x55, // 41         lai     0x55    ; Loop here forever, but at least look cool while doing it
              //    SPIN2:
  0x3f,       // 43         cyn
  0x10, 0x43, // 44         jmp     SPIN2
};
