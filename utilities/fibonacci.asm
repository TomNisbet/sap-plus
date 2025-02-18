; Display the fibonacci sequence on the Output Register
        data
NUM1:   byte
NUM2:   byte

        code
        nop             ; start with a NOP to avoid reset/clock issues
START:
        lai     1       ; Start with the first number in the sequence and save as NUM1
        sam     NUM1
        out
LOOP:
        sam     NUM2    ; Save current sum to NUM2 and output it
        out
        adm     NUM1    ; Add current sum to NUM1 and save as NUM1
        jc      START   ; Restart if sum exceeds 255
        sam     NUM1
        out
        adm     NUM2    ; Add current sum to NUM2
        jc      START   ; Restart if sum exceeds 255
        jmp     LOOP
