        data
NUM1:   byte
NUM2:   byte

        code
START:
        lai     1
        sam     NUM1
        out
LOOP:
        sam     NUM2
        out
        adm     NUM1
        jc      START
        sam     NUM1
        out
        adm     NUM2
        jc      START
        jmp     LOOP
