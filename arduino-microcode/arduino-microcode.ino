#include "Arduino.h"

static const char * MY_VERSION = "1.1";

// IO lines for the EEPROM device control
// Pins D2..D9 are used for the data bus.
#define WE              A0
#define CE              A1
#define OE              A2
#define ADDR_CLK_HI     A3
#define ADDR_CLK_LO     A4
#define ADDR_DATA       A5

#define RED_LED         13


void burnMicrocodeRoms();
void disableSoftwareWriteProtect();
byte readByte(uint32_t address);
bool burnByte(byte value, uint32_t address);
bool burnBlock(byte data[], uint32_t len, uint32_t address);
bool waitForWriteCycleEnd(byte lastValue);
void setByte(byte value, uint32_t address);
void setAddress(uint32_t address);
void setAddressRegister(uint8_t clkPin, byte addr);
bool writeData(byte data[], uint32_t len, uint32_t address);
void setDataBusMode(uint8_t mode);
byte readDataBus();
void writeDataBus(byte data);

// Set the status of the device control pins
static void enableChip()       { digitalWrite(CE, LOW); }
static void disableChip()      { digitalWrite(CE, HIGH);}
static void enableOutput()     { digitalWrite(OE, LOW); }
static void disableOutput()    { digitalWrite(OE, HIGH);}
static void enableWrite()      { digitalWrite(WE, LOW); }
static void disableWrite()     { digitalWrite(WE, HIGH);}


void setup() {
    // The address control pins are always outputs.
    pinMode(ADDR_DATA, OUTPUT);
    pinMode(ADDR_CLK_LO, OUTPUT);
    pinMode(ADDR_CLK_HI, OUTPUT);
    digitalWrite(ADDR_DATA, LOW);
    digitalWrite(ADDR_CLK_LO, LOW);
    digitalWrite(ADDR_CLK_HI, LOW);
    DDRB |= 0x1c; // Set D10..D12 as outputs

    // To save time, the setAddress only writes the hi byte if it has changed.
    // The value used to detect the change is initialized to a non-zero value,
    // so set an initial address to avoid the the case where the first address
    // written is the 'magic' initial address.
    setAddress(0x0000);

    // Define the data bus as input initially so that it does not put out a
    // signal that could collide with output on the data pins of the EEPROM.
    setDataBusMode(INPUT);

    // Define the EEPROM control pins as output, making sure they are all
    // in the disabled state.
    digitalWrite(OE, HIGH);
    pinMode(OE, OUTPUT);
    digitalWrite(CE, HIGH);
    pinMode(CE, OUTPUT);
    digitalWrite(WE, HIGH);
    pinMode(WE, OUTPUT);

    pinMode(RED_LED, OUTPUT);

    Serial.begin(115200);
    Serial.print("\nBurning SAP-Plus microcode version ");
    Serial.println(MY_VERSION);
    disableSoftwareWriteProtect();
    delay(100);
    burnMicrocodeRoms();
    Serial.println(F("burn complete"));
}

void loop() {
    digitalWrite(RED_LED, HIGH);
    delay(500);
    digitalWrite(RED_LED, LOW);
    delay(500);
}

void fail() {
    Serial.println(F("Write FAILED"));
    while (1) {
      digitalWrite(RED_LED, HIGH);
      delay(200);
      digitalWrite(RED_LED, LOW);
      delay(200);
      digitalWrite(RED_LED, HIGH);
      delay(200);
      digitalWrite(RED_LED, LOW);
      delay(200);
      digitalWrite(RED_LED, HIGH);
      delay(200);
      digitalWrite(RED_LED, LOW);
      delay(600);
    }
}

// Get rid of Arduino macro definitions that conflict with control signal names
#undef PI

//               ROM2    ROM1    ROM0
//             +------++------+
// ROM1         WWWWRRR76543210
#define W3   0b0100000000000000L  // 1 Write bit 3
#define W2   0b0010000000000000L  // 1 Write bit 2
#define W1   0b0001000000000000L  // 1 Write bit 1
#define W0   0b0000100000000000L  // 1 Write bit 0
#define R2   0b0000010000000000L  // 8 Read bit 2
#define R1   0b0000001000000000L  // 8 Read bit 1
#define R0   0b0000000100000000L  // 8 Read bit 0

// ROM0
#define SC   0b0000000000100000L  // 20 Stack Pointer count enable
#define CX   0b0000000000010000L  // 10 Carry in (multi use flag)
#define BI   0b0000000000001000L  // 08 B register invert
#define FL   0b0000000000000100L  // 04 Flags register load
#define N    0b0000000000000010L  // 02 Next Instruction (clear RC)
#define PI   0b0000000000000001L  // 01 PC Increment

#define WA   W0             // 1 Write A
#define WB   W1             // 2 Write B
#define WO   W1|W0          // 3 Write Output
#define WP   W2             // 4 Write PC (Jump)
#define WR   W2|W0          // 5 Write RAM
#define WS   W2|W1          // 6 Write SP
#define WX   W2|W1|W0       // 7 Write D (not implemented)
#define WX8  W3             // 8
#define WM   W3|W0          // 9 Write MAR (Memory Address)
#define WI   W3|W1          // A Write IR

#define RA   R0             // 1 Read A
#define RB   R1             // 2 Read B
#define RL   R1|R0          // 3 Read L (ALU result)
#define RP   R2             // 4 Read PC
#define RR   R2|R0          // 5 Read RAM
#define RS   R2|R1          // 6 Read SP
#define RD   R2|R1|R0       // 7 Read D (not implemented)

// Memory Architecture
// The CX bit allows the memory to be divided into two 256-byte blocks.  This allows
// the code, data, and stack to all share the same space or for one of the three to
// have its own dedicated space.
// Place the code in the default block with the data and stack in the extended block.
#define RRC   RR
#define WRC   WR
#define RRD   RR|CX
#define WRD   WR|CX
#define RRS   RR|CX
#define WRS   WR|CX

// Microinstruction bit aliases
#define FAA   RP | PI | WM  // Fetch argument (operand) address
#define SPD   SC | CX       // Stack Pointer decrement
#define SPI   SC            // Stack Pointer increment
#define LCI   CX            // CX used as ALU carry in


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
    N_CLF = 0x0c,  //   clear carry and zero flags
    N_SEF = 0x0d,  //   set carry and zero flags
    N_JMP = 0x10,  //   jump unconditional
    N_JC  = 0x11,  //   jump on Carry
    N_JZ  = 0x12,  //   jump on Zero
    N_JNC = 0x13,  //   jump on no Carry
    N_JNZ = 0x14,  //   jump on no Zero
    N_PHA = 0x15,  //   push A
    N_PLA = 0x16,  //   pull A
    N_JSR = 0x17,  //   jump to subroutine
    N_RTS = 0x18,  //   return from subroutine
    N_RC  = 0x19,  //   return if carry
    N_RZ  = 0x1a,  //   return if zero
    N_RNC = 0x1b,  //   return if not carry
    N_RNZ = 0x1c,  //   return if not zero
    N_ADI = 0x20,  //   add  immediate to A
    N_ADM = 0x21,  //   add memory to A
    N_SBI = 0x22,  //   subtract immediate from A
    N_SBM = 0x23,  //   subtract memory from A
    N_ACI = 0x24,  //   add immediate to A with carry
    N_ACM = 0x25,  //   add memory to A with carry
    N_SCI = 0x26,  //   subtract immediate from A with borrow
    N_SCM = 0x27,  //   subtract memory from A with borrow
    N_CPI = 0x28,  //   compare immediate to A
    N_CPM = 0x29   //   compare memory to A
};



// ROM addressing 8K = 13 address bits
//  rffii iiiissss
//  r                2 ROM chips (16 bit microinstruction word)
//   CZ              2 flag bits (4 combinations)
//      ii iiii      64 instructions
//             ssss  16 steps (max) per instruction
//
// To simplify microcode ROM burning and management, the microcode for all four chips is
// burned into every chip and the ROMs have the uppermost two address bits hardwired to
// determine which part of the microcode a chip is providing.  This means that all chips
// can be burned identically and that any chip can be installed in any slot.

enum {
    NUM_ROMS = 2,
    NUM_FLAG_COMBOS = 4,
    NUM_INSTRUCTIONS = 64,
    NUM_STEPS = 16,
    NUM_TEMPLATE_STEPS = 8, // 2 hard-coded fetch steps plus 8 template steps
};

enum {
    // Flag bit positions starting at the LSB.  These are shifted left 10 bits to
    // form the ROM address.
    FL_Z = 1,
    FL_C = 2
};

typedef uint16_t microcode_t;
typedef microcode_t template_t[64][NUM_TEMPLATE_STEPS];

// Note that steps zero and one of all instructions are hard-coded to the instruction
// fetch, so this table contains steps 2..9.  No instructions currently have more than ten
// steps.  If a small number of new instructions are added that need additional steps, it
// would be best to just build those few instructions manually in the buildInstruction code
// instead of increasing this table to add more steps.
const template_t template0 PROGMEM = {
//  2           3           4           5           6           7           8           9
  { N,          0,          0,          0,          0,          0,          0,          0    }, // 00 N_NOP   3
  { RA|WO|N,    0,          0,          0,          0,          0,          0,          0    }, // 01 N_OUT   3
  { FAA,        RRC|WA|N,   0,          0,          0,          0,          0,          0    }, // 02 N_LAI   4
  { FAA,        RRC|WM,     RRD|WA|N,   0,          0,          0,          0,          0    }, // 03 N_LAM   5
  { FAA,        RRC|WM,     RA|WRD|N,   0,          0,          0,          0,          0    }, // 04 N_SAM   5
  { RA|WS|N,    0,          0,          0,          0,          0,          0,          0    }, // 05 N_TAS   3
  { RS|WA|N,    0,          0,          0,          0,          0,          0,          0    }, // 06 N_TSA   3
  { WB,         RL|WA|LCI|N,0,          0,          0,          0,          0,          0    }, // 07 N_INA   4
  { WB,         RL|WA|BI|LCI|N,  0,     0,          0,          0,          0,          0    }, // 08 N_DCA   4
  { RA|WB,      RB|WA|BI|N, 0,          0,          0,          0,          0,          0    }, // 09 N_NOT   4
  { RA|WB,      RL|WA|N,    0,          0,          0,          0,          0,          0    }, // 0a N_ASL   4
  { WB,         RL|BI|FL|LCI|N,  0,     0,          0,          0,          0,          0    }, // 0b N_TST   2
  { RA|WB,      RL|BI|FL|N, 0,          0,          0,          0,          0,          0    }, // 0c N_CLF   2
  { RA|WB,      RL|BI|FL|LCI|N,         0,          0,          0,          0,          0    }, // 0d N_SEF   2
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 0e
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 0f

  { FAA,        RRC|WP|N,   0,          0,          0,          0,          0,          0    }, // 10 N_JMP   4
  { FAA,        RRC|WP|N,   0,          0,          0,          0,          0,          0    }, // 11 N_JC    4
  { FAA,        RRC|WP|N,   0,          0,          0,          0,          0,          0    }, // 12 N_JZ    4
  { FAA,        RRC|WP|N,   0,          0,          0,          0,          0,          0    }, // 13 N_JNC   4
  { FAA,        RRC|WP|N,   0,          0,          0,          0,          0,          0    }, // 14 N_JNZ   4
  { RS|WM,      RA|WRS|SPI|N,  0,       0,          0,          0,          0,          0    }, // 15 N_PHA   4
  { SPD,        RS|WM,      RRS|WA|N,   0,          0,          0,          0,          0    }, // 16 N_PLA   5
  { FAA,        RRC|WB,     RS|WM,      RP|WRS|SPI, RB|WP|N,    0,          0,          0    }, // 17 N_JSR   7
  { SPD,        RS|WM,      RRS|WP|N,   0,          0,          0,          0,          0    }, // 18 N_RTS   5
  { SPD,        RS|WM,      RRS|WP|N,   0,          0,          0,          0,          0    }, // 19 N_RC    5
  { SPD,        RS|WM,      RRS|WP|N,   0,          0,          0,          0,          0    }, // 20 N_RZ    5
  { SPD,        RS|WM,      RRS|WP|N,   0,          0,          0,          0,          0    }, // 21 N_RNC   5
  { SPD,        RS|WM,      RRS|WP|N,   0,          0,          0,          0,          0    }, // 22 N_RNZ   5
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 1d
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 1e
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 1f

  { FAA,        RRC|WB,     RL|WA|FL|N, 0,          0,          0,          0,          0    }, // 20 N_ADI   5
  { FAA,        RRC|WM,     RRD|WB,     RL|WA|FL|N, 0,          0,          0,          0    }, // 21 N_ADM   6
  { FAA,        RRC|WB,     RL|WA|FL|BI|LCI|N,  0,  0,          0,          0,          0    }, // 22 N_SBI   5
  { FAA,        RRC|WM,     RRD|WB,     RL|WA|FL|BI|LCI|N,  0,  0,          0,          0    }, // 23 N_SBM   6
  { FAA,        RRC|WB,     RL|WA|FL|N, 0,          0,          0,          0,          0    }, // 24 N_ACI   5
  { FAA,        RRC|WM,     RRD|WB,     RL|WA|FL|N, 0,          0,          0,          0    }, // 25 N_ACM   6
  { FAA,        RRC|WB,     RL|WA|FL|BI|N,  0,      0,          0,          0,          0    }, // 26 N_SCI   5
  { FAA,        RRC|WM,     RRD|WB,     RL|WA|FL|BI|N,  0,      0,          0,          0    }, // 27 N_SCM   6
  { FAA,        RRC|WB,     FL|BI|LCI|N,  0,        0,          0,          0,          0    }, // 28 N_CPI   5
  { FAA,        RRC|WM,     RRD|WB,     FL|BI|LCI|N,0,          0,          0,          0    }, // 29 N_CPM   6
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 2a
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 2b
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 2c
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 2d
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 2e
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 2f

  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 30
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 31
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 32
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 33
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 34
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 35
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 36
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 37
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 38
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 39
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 3a
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 3b
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 3c
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 3d
  { 0,          0,          0,          0,          0,          0,          0,          0    }, // 3e
  { 0,          0,          0,          0,          0,          0,          0,          0    }  // 3f
};

// A buffer to hold the microinstruction words (steps) for an instruction that have been
// built by buildInstruction.
microcode_t code[NUM_STEPS];

// A buffer to hold an 8-bit slice of the microinstruction words for an instruction to
// burn into a section of ROM.  This buffer allows the use of the block write capability
// of the 28C256 chips, which results in a speed up of the ROM burn times.  The buffer
// allows one write operation (per ROM chip) for each instruction rather than one write
// for each microinstruction step.
uint8_t burnBuffer[NUM_STEPS];


// Convert the individual address components into a 15-bit address for the ROM.
uint16_t makeAddress(uint16_t rom, uint16_t flags=0, uint16_t instr=0, uint16_t step=0) {
    uint16_t addr =  (rom << 12) | (flags << 10) | (instr << 4) | step;
//    char b[50];
//    sprintf(b, "a=x%04x  r=%d f=%d i=%u s=%u", addr, rom, flags, instr, step);
//    Serial.println(b);
    return addr;
}


#define F1    RP | WM | PI  // Instruction fetch step 1: instruction address from PC to MAR
#define F2    RRC | WI      // Instruction fetch step 2: instruction from RAM to IR

void buildInstruction(unsigned opcode, unsigned flags) {
    // Initialize the code buffer from a template.
    code[0] = F1;            // Fetch instruction from memory
    code[1] = F2;            // Opcode into IR
    memcpy_P(code + 2, template0[opcode], NUM_TEMPLATE_STEPS*2);
    code[10] = code[11] = code[12] = code[13] = code[14] = code[15] = 0;

    bool skipJmp = false;
    bool skipRts = false;
    switch (opcode) {

        // For the ADD and SUB with Carry instructions, set the ALU carry-in flag if the
        // carry flag is set.
        case N_ACI:
        case N_SCI:
            if (flags & FL_C) {
                code[4] |= LCI;
            }
            break;

        case N_ACM:
        case N_SCM:
            if (flags & FL_C) {
                code[5] |= LCI;
            }
            break;

        case N_JC:    skipJmp = ((flags & FL_C) == 0);      break;
        case N_JZ:    skipJmp = ((flags & FL_Z) == 0);      break;
        case N_JNZ:   skipJmp = ((flags & FL_C) != 0);      break;
        case N_JNC:   skipJmp = ((flags & FL_Z) != 0);      break;

        case N_RC:    skipRts = ((flags & FL_C) == 0);      break;
        case N_RZ:    skipRts = ((flags & FL_Z) == 0);      break;
        case N_RNZ:   skipRts = ((flags & FL_C) != 0);      break;
        case N_RNC:   skipRts = ((flags & FL_Z) != 0);      break;
    }

    // Conditional jump instructions are coded by default as JMP instructions.
    // If the condition is not met, overwrite the microcode step that loads
    // the jump address into the PC with code that just advances PC to the
    // next instruction.
    if (skipJmp) {
        code[2] = PI | N;   // PI to move the PC past the address argument
        code[3] = 0;
    }

    // Conditional returns are the same - all are coded as RTS instructions but
    // this code will overwrite the RTS if the flag condition is not a match.
    else if (skipRts) {
        code[2] = N;   // No argument to skip, so just end the instruction
        code[3] = code[4] = code[5] = 0;
    }
}


void burnCodeBuffer(uint16_t flags, uint8_t opcode) {
    // Slice the a 16-bit instruction control words of an instruction into individual
    // 8-bit chunks and burn them into the appropriate position in the ROM.
    for (uint16_t rom = 0; (rom < NUM_ROMS); rom++) {
        uint16_t shift = rom * 8;  // Shift 8 bits for each ROM position
        for (int step = 0; step < NUM_STEPS; step++) {
            burnBuffer[step] = uint8_t(code[step] >> shift);
        }
        if (!writeData(burnBuffer, sizeof(burnBuffer), makeAddress(rom, flags, opcode))) {
            fail();
        }
    }
}

const char hex[] = "0123456789abcdef";

void printByte(byte b) {
    char line[4];

    line[0] = hex[b >> 4];
    line[1] = hex[b & 0x0f];
    line[2] = ' ';
    line[3] = '\0';

    Serial.print(line);
}

void printWord(word w) {
    char line[6];

    line[0] = hex[(w >> 12) & 0x0f];
    line[1] = hex[(w >>  8) & 0x0f];
    line[2] = hex[(w >>  4) & 0x0f];
    line[3] = hex[(w)       & 0x0f];
    line[4] = ' ';
    line[5] = '\0';

    Serial.print(line);
}

void printHex24(uint32_t u32) {
    char line[8];

    line[0] = hex[(u32 >> 20) & 0x0f];
    line[1] = hex[(u32 >> 16) & 0x0f];
    line[2] = hex[(u32 >> 12) & 0x0f];
    line[3] = hex[(u32 >>  8) & 0x0f];
    line[4] = hex[(u32 >>  4) & 0x0f];
    line[5] = hex[(u32)       & 0x0f];
    line[6] = ' ';
    line[7] = '\0';

    Serial.print(line);
}


void burnMicrocodeRoms() {
    for (unsigned flags = 0; (flags < NUM_FLAG_COMBOS); flags++) {
        for (unsigned opcode = 0; (opcode < NUM_INSTRUCTIONS); opcode++) {
            if ((opcode & 0x0f) == 0) {
                Serial.print(F("Burning "));
                Serial.print((flags & 2) ? 'C' : 'c');
                Serial.print((flags & 1) ? 'Z' : 'z');
                Serial.print(':');
                Serial.println(opcode, HEX);
            }
            buildInstruction(opcode, flags);

            printByte(opcode);
            Serial.print(": ");
            for (uint16_t rom = 0; (rom < NUM_ROMS); rom++) {
                printWord(makeAddress(1 - rom, flags, opcode));
            }
            Serial.print("- ");
            for (int step = 0; step < 11; step++) {
                printWord(code[step]);
            }
            Serial.println("");
            burnCodeBuffer(flags, opcode);
        }
    }
}



const uint32_t mSize = 32 * 1024L;      // Size of the device, in bytes
const unsigned int mBlockSize = 64;     // Block size for page writes, zero if N/A
const unsigned int mMaxWriteTime = 10;  // Max time (in ms) to wait for write cycle to complete

// Write the special six-byte code to turn off Software Data Protection.
void disableSoftwareWriteProtect() {
    disableOutput();
    disableWrite();
    enableChip();
    setDataBusMode(OUTPUT);

    setByte(0xaa, 0x5555);
    setByte(0x55, 0x2aaa);
    setByte(0x80, 0x5555);
    setByte(0xaa, 0x5555);
    setByte(0x55, 0x2aaa);
    setByte(0x20, 0x5555);

    setDataBusMode(INPUT);
    disableChip();
}

// Read a byte from a given address
byte readByte(uint32_t address) {
    byte data = 0;
    setAddress(address);
    setDataBusMode(INPUT);
    disableOutput();
    disableWrite();
    enableChip();
    enableOutput();
    data = readDataBus();
    disableOutput();
    disableChip();
    return data;
}


// Burn a byte to the chip and verify that it was written.
bool burnByte(byte value, uint32_t address) {
    bool status = false;

    disableOutput();
    disableWrite();

    setAddress(address);
    setDataBusMode(OUTPUT);
    writeDataBus(value);

    enableChip();
    delayMicroseconds(3);
    enableWrite();
    delayMicroseconds(3);
    disableWrite();

    status = waitForWriteCycleEnd(value);

    disableChip();

    if (!status) {
        Serial.print(F("burn address="));
        Serial.println(address, HEX);
    }
    return status;
}


bool burnBlock(byte data[], uint32_t len, uint32_t address) {
    bool status = false;
    char cb[50];
//    sprintf(cb, "burn %02x at %04x", uint16_t(len), uint16_t(address));
//    Serial.println(cb);
//    Serial.println(address, HEX);
    if (len == 0)  return true;

    disableOutput();
    disableWrite();
    enableChip();

    // Write all of the bytes in the block out to the chip.  The chip will
    // program them all at once as long as they are written fast enough.
    setDataBusMode(OUTPUT);
    for (uint32_t ix = 0; (ix < len); ix++) {
        setAddress(address + ix);
        writeDataBus(data[ix]);

        delayMicroseconds(1);
        enableWrite();
        delayMicroseconds(1);
        disableWrite();
    }

    status = waitForWriteCycleEnd(data[len - 1]);
    disableChip();

    if (!status) {
        Serial.print(F("burnBlock fail at 0x"));
        Serial.println(address, HEX);
        for (uint32_t ix = 0; (ix < len); ix++) {
            uint8_t b = readByte(address+ix);
            if (b != data[ix]) {
                sprintf(cb, "addr=%04x, data=%0x2, readback=%02x", unsigned(address+ix), unsigned(data[ix]), unsigned(b));
                Serial.println(cb);
                break;
            }
        }
    }

    return status;
}


bool waitForWriteCycleEnd(byte lastValue) {
    // Verify programming complete by reading the last value back until it matches the
    // value written twice in a row.  The D7 bit will read the inverse of last written
    // data and the D6 bit will toggle on each read while in programming mode.
    //
    // Note that the max readcount is set to the device's maxReadTime (in uSecs)
    // divided by two because there are two 1 uSec delays in the loop.  In reality,
    // the loop could run for longer because this does not account for the time needed
    // to run all of the loop code.  In actual practice, the loop will terminate much
    // earlier because it will detect the end of the write well before the max time.
    setDataBusMode(INPUT);
    delayMicroseconds(1);
    for (int readCount = mMaxWriteTime * 1000 / 2; (readCount > 0); readCount--) {
        enableChip();
        enableOutput();
        delayMicroseconds(1);
        byte b1 = readDataBus();
        disableOutput();
        disableChip();
        enableChip();
        enableOutput();
        delayMicroseconds(1);
        byte b2 = readDataBus();
        disableOutput();
        disableChip();
        if ((b1 == b2) && (b1 == lastValue)) {
            return true;
        }
    }

    return false;
}


// Set an address and data value and toggle the write control.  This is used
// to write control sequences, like the software write protect.  This is not a
// complete byte write function because it does not set the chip enable or the
// mode of the data bus.
void setByte(byte value, uint32_t address)
{
    setAddress(address);
    writeDataBus(value);

    delayMicroseconds(1);
    enableWrite();
    delayMicroseconds(1);
    disableWrite();
}

// Set a 16 bit address in the two address shift registers.
void setAddress(uint32_t address) {
    static byte lastHi = 0xca;
    byte hi = (address >> 8) & 0xff;
    byte lo = address & 0xff;

    if (hi != lastHi) {
        setAddressRegister(ADDR_CLK_HI, hi);
        lastHi = hi;
    }
    setAddressRegister(ADDR_CLK_LO, lo);
}

// Shift an 8-bit value into one of the address shift registers.  Note that
// the data pins are tied together, selecting the high or low address register
// is a matter of using the correct clock pin to shift the data in.
void setAddressRegister(uint8_t clkPin, byte addr) {
    byte mask = 0;

    if (clkPin == ADDR_CLK_HI) {
        mask = 0x08;
    } else if (clkPin == ADDR_CLK_LO) {
        mask = 0x10;
    }

    // Make sure the clock is low to start.
    PORTC &= ~mask;

    // Shift 8 bits in, starting with the MSB.
    for (int ix = 0; (ix < 8); ix++) {
        // Set the data bit
        if (addr & 0x80) {
            PORTC |= 0x20;
        } else {
            PORTC &= 0xdf;
        }

        // Toggle the clock high then low
        PORTC |= mask;
        delayMicroseconds(3);
        PORTC &= ~mask;
        addr <<= 1;
    }
}

void dump(uint16_t address, byte data[]) {
    char buffer[80];
    sprintf(buffer, "%04x %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x",
    address,
    data[0],data[1],data[2],data[3],
    data[4],data[5],data[6],data[7],
    data[8],data[9],data[10],data[11],
    data[12],data[13],data[14],data[15]);
    Serial.println(buffer);

}
// Write a block of data to the device.  The device supports block writes,
// so the data will be broken into chunks and written using the block mode.
bool writeData(byte data[], uint32_t len, uint32_t address)
{
    /*
    dump(address, data);
    dump(address+0x10, data+0x10);
    dump(address+0x20, data+0x20);
    dump(address+0x30, data+0x30);
    dump(address+0x40, data+0x40);
    dump(address+0x50, data+0x50);
    dump(address+0x60, data+0x60);
    dump(address+0x70, data+0x70);
    */
    bool status = true;

    if (mBlockSize == 0) {
        // Device does not support block writes.
        for (uint32_t ix = 0; (ix < len); ix++) {
            if (burnByte(data[ix], address + ix) == false) {
                status = false;
                break;
            }
        }
    } else {
        uint32_t offset = 0;
        uint32_t chunkSize;
        if (address & (mBlockSize - 1)) {
            // Address does not start on a block boundary.  Adjust the size of
            // the first block to fit within a single block.
            chunkSize = mBlockSize - (address & (mBlockSize - 1));
            chunkSize = (chunkSize > len) ? len : chunkSize;
            if (burnBlock(data, chunkSize, address) == false) {
                return false;
            }
            offset += chunkSize;
            len -= chunkSize;
        }

        // All writes are now aligned to block boundaries, so write full blocks
        // or remaining length, whichever is smaller.
        while (len > 0) {
            chunkSize = (len > mBlockSize) ? mBlockSize : len;
            if (burnBlock(data + offset, chunkSize, address + offset) == false) {
                status = false;
                break;
            }
            offset += chunkSize;
            len -= chunkSize;
        }
    }

    return status;
}

void setDataBusMode(uint8_t mode) {
    // On the Uno and Nano, D2..D9 maps to the upper 6 bits of port D and the
    // lower 2 bits of port B.
    if (mode == OUTPUT) {
        DDRB |= 0x03;
        DDRD |= 0xfc;
    } else {
        DDRB &= 0xfc;
        DDRD &= 0x03;
    }
}

// Read a byte from the data bus.  The caller must set the bus to input_mode
// before calling this or no useful data will be returned.
byte readDataBus() {
    return (PINB << 6) | (PIND >> 2);
}

// Write a byte to the data bus.  The caller must set the bus to output_mode
// before calling this or no data will be written.
void writeDataBus(byte data) {
     PORTB = (PORTB & 0xfc) | (data >> 6);
     PORTD = (PORTD & 0x03) | (data << 2);
}
