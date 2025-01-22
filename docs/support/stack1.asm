    LAI     0xff
    TAS
    LAI     0       

LOOP:
    OUT
    PHA
    ACI     3
    OUT
    PLA
    OUT
    INA
    JMP     LOOP
