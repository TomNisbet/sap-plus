// Instruction opcodes.
enum {
    N_NOP = 0x00,  //   no operation
    N_OUT = 0x01,  //   output A
    N_LAI = 0x02,  //   load A immediate
    N_LAM = 0x03,  //   load A from memory
    N_SAM = 0x04,  //   store A to memory
    N_TAS = 0x05,  //   transfer A to SP
    N_TSA = 0x06,  //   transfer SP to A
    N_INA = 0x07,  //   increment A
    N_DCA = 0x08,  //   decrement A
    N_NOT = 0x09,  //   NOT A
    N_ASL = 0x0a,  //   arithmetic shift left A
    N_TST = 0x0b,  //   test A
    N_CLC = 0x0c,  //   clear carry flag
    N_SEC = 0x0d,  //   set carry flag
    N_JMP = 0x10,  //   jump unconditional
    N_JC = 0x11,  //   jump on Carry
    N_JZ = 0x12,  //   jump on Zero
    N_JNC = 0x13,  //   jump on no Carry
    N_JNZ = 0x14,  //   jump on no Zero
    N_PHA = 0x15,  //   push A
    N_PLA = 0x16,  //   pull A
    N_JSR = 0x17,  //   jump to subroutine
    N_RTS = 0x18,  //   return from subroutine
    N_ADI = 0x20,  //   add  immediate to A
    N_ADM = 0x21,  //   add memory to A
    N_SBI = 0x22,  //   subtract immediate from A
    N_SBM = 0x23,  //   subtract memory from A
    N_ACI = 0x24,  //   add immediate to A with carry
    N_ACM = 0x25,  //   add memory to A with carry
    N_SCI = 0x26,  //   subtract immediate from A with borrow
    N_SCM = 0x27,  //   subtract memory from A with borrow
    N_CPI = 0x28,  //   compare immediate to A
    N_CPM = 0x29,  //   compare memory to A
};