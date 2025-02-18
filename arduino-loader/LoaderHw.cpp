#include "LoaderHw.h"
#include "CmdStatus.h"

extern CmdStatus cmdStatus;

enum {
    DELAY_LEDS = 80
};

// Pin assignments are specific to Arduino Nano.  Some direct port manipulation is done
// that may not be compatible with other Arduino models.

// USB reserves pins 0..1
// Data bus is pins 2..9
enum {
    PGM  = 10,   // OUTPUT - puts the host in PROGRAM mode
    LD2  = A0,   // OUTPUT - CLK the R2 register (register selects)
    SER  = A1,   // OUTPUT - Serial data to the R0/R1/R2 registers
    RCLK = A2,   // OUTPUT - output the values of all shift registers to the pins
    LD0  = A3,   // OUTPUT - CLK the R0 and R1 registers as 16 bits (control signals)
    RST  = 12,   // OUTPUT - drives the host RST line
    CLK  = 11    // OUTPUT - drives the host CLK line
};

enum {
    PC_LD2  = 0x01,
    PC_SER  = 0x02,
    PC_RCLK = 0x04,
    PC_LD0  = 0x08,
    PC_RST  = 0x10,
    PC_CLK  = 0x20
};


enum {
    CTL_SC = 0x20, // Stack Pointer count enable
    CTL_CX = 0x10, // ALU carry in (and stack direction and memory extend)
    CTL_BI = 0x08, // B register invert
    CTL_FL = 0x04, // load flags
    CTL_N  = 0x02, // next instruction (clear ring counter)
    CTL_PI = 0x01, // program counter increment

    CTL_SP_DN = CTL_SC | CTL_CX, // CX controlS SP count direction
    CTL_SP_UP = CTL_SC,
    CTL_MX = CTL_CX,    // CX is also memory extend bit in MAR

    CTL_NONE = 0x00,
    CTL_ALL = 0x3f
};

enum {
    REG_NONE= 0x00, // No register selected
    REG_A =   0x01, // r/w
    REG_B =   0x02, // r/w
    REG_ALU = 0x03, // read-only
    REG_OUT = 0x03, // write-only
    REG_PC =  0x04, // r/w
    REG_MEM = 0x05, // r/w
    REG_SP  = 0x06, // r/w
    REG_MAR = 0x09, // write-only
    REG_IR =  0x0a,  // write-only

    REG_B_INVERTED = 0x0f  // pseudo-register for testing the inverted B value
};


static uint8_t valSelects = 0;

static const char * registerNames[] = {
    "none", "A",   "B",   "OUT", "PC",  "MEM", "SP",  "07",
    "08",   "MAR", "IR",  "0B",  "0C",  "0D",  "0E",  "B inverted"
};
static const unsigned woRegisters[] = { REG_MAR } ;//, REG_MEM, REG_IR };
static const unsigned rwRegisters[] = { REG_A, REG_B, REG_B_INVERTED, REG_SP, REG_PC };
static const unsigned clrRegisters[] = { REG_A, REG_B, REG_OUT, REG_PC, REG_SP, REG_MAR, REG_IR };
static unsigned numWoRegisters() { return sizeof(woRegisters) / sizeof(*woRegisters); }
static unsigned numRwRegisters() { return sizeof(rwRegisters) / sizeof(*rwRegisters); }
static unsigned numClrRegisters() { return sizeof(clrRegisters) / sizeof(*clrRegisters); }

#if 0
static void waitForUser(const char * s = 0) {
    if (s) {
        Serial.println(s);
    }
    while (Serial.read() == -1) {}
}
#endif

static void controlBitsOffOn(uint8_t clearBits, uint8_t setBits);
static void selectRegister();

LoaderHw::LoaderHw()
    : mEnabled(false)
{}


void LoaderHw::begin() {
    // The loader's data bus is connected directly to the host bus.  Define the
    // data bus as input initially so that it does not put out a signal that
    // could collide with host outputs on the bus.
    // Care must be taken to ensure that the bus is never left in the OUTPUT
    // mode when control is handed back to the host computer.
    setDataBusMode(INPUT);

    // External pull-up resistors are present on RST and PGM.  An external pull-down is on
    // CLK.  This allows the system to function as normal if the arduino is not present,
    // although without a way to load programs there isn't a lot that the system will do.
    digitalWrite(LD0, LOW);     // Normally LOW, pulse HIGH to clock the R0/R1 registers
    pinMode(LD0, OUTPUT);
    digitalWrite(SER, LOW);     // Serial data to the shift registers
    pinMode(SER, OUTPUT);
    digitalWrite(RCLK, LOW);    // Normally LOW, pulse HIGH to put shift register data on the pins
    pinMode(RCLK, OUTPUT);
    digitalWrite(LD2, LOW);     // Normally LOW, pulse HIGH to clock the R2 register
    pinMode(LD2, OUTPUT);
    digitalWrite(PGM, HIGH);    // PGM is active LOW
    pinMode(PGM, OUTPUT);
    digitalWrite(RST, HIGH);    // RST is active LOW
    pinMode(RST, OUTPUT);
    digitalWrite(CLK, LOW);     // Normally LOW.  Pulse high-low to generate a clock
    pinMode(CLK, OUTPUT);
    disable();
}

void LoaderHw::disable() {
  // Make sure the loader data bus and register selects are not putting out a signal and
  // then return control to the host.
  setDataBusMode(INPUT);
  digitalWrite(PGM, HIGH);
  mEnabled = false;
}

void LoaderHw::enable() {
    if (!mEnabled) {
        Serial.println("enable");
        setDataBusMode(INPUT);
        digitalWrite(PGM, LOW);
        valSelects = 0;
        selectRegister();
        controlBitsOffOn(CTL_ALL, CTL_NONE);
        mEnabled = true;
        delay(1);
    }
}

// Generate one pulse on the host clock
void LoaderHw::clkPulse() {
    delayMicroseconds(1);
    digitalWrite(CLK, HIGH);
    delayMicroseconds(1);
    digitalWrite(CLK, LOW);
    delayMicroseconds(1);
}

// Generate one exteremely long pulse on the host clock to allow probing of the rising
// and falling edge
void LoaderHw::clkPulseWidth(unsigned width) {
    digitalWrite(CLK, HIGH);
    delayMicroseconds(width);
    digitalWrite(CLK, LOW);
    delayMicroseconds(width);
}

// Generate one reset pulse to the host
void LoaderHw::reset() {
    digitalWrite(RST, LOW);
    delayMicroseconds(4);
    digitalWrite(RST, HIGH);
    delay(1);
}

void LoaderHw::clearAll() {
    // Set all clearable registers to zero
    for (unsigned ix = 0; (ix < numClrRegisters()); ix++) {
        writeRegister(clrRegisters[ix], 0);
    }

    // Clear the flags register.  With both A and B at zero, adding A to inverted B
    // gives 0xff, which will not set the zero or carry flag.  The zero flag tests
    // the bus value, not the ALU, so put the ALU value on the bus.
    writeControls(CTL_N|CTL_BI|CTL_FL);
    selectReadRegister(REG_ALU);
    clkPulse();
    writeControls(CTL_N);

    // Clock the flag bits into the IR, along with zeroes from the bus in the 6 instruction bits.
    selectReadRegister(REG_NONE);
    selectWriteRegister(REG_IR);
    clkPulse();

    writeControls(0);
    selectReadRegister(REG_NONE);
    selectWriteRegister(REG_NONE);
    reset();
}

// Return a name for a given register number
const char * LoaderHw::registerName(int registerNumber)
{
    return registerNames[registerNumber];
}

uint8_t LoaderHw::readRegister(uint8_t reg) {
    selectReadRegister(reg);
    delayMicroseconds(3);
    uint8_t data = readDataBus();
    delayMicroseconds(1);
    return data;
}

void LoaderHw::writeRegister(uint8_t reg, uint8_t data) {
    selectWriteRegister(reg);
    setDataBusMode(OUTPUT);
    writeDataBus(data);
    clkPulse();
    setDataBusMode(INPUT);
}

void LoaderHw::transferRegister(uint8_t wReg, uint8_t rReg) {
    selectReadRegister(rReg);
    selectWriteRegister(wReg);
    clkPulse();
}

// Write a block of data to the memory.
bool LoaderHw::writeData(const byte data[], uint16_t len, uint16_t address) {
    bool status = true;

    for (uint16_t ix = 0; (ix < len); ix++) {
        if (burnByte(data[ix], address + ix) == false) {
            status = false;
            break;
        }
    }
    return status;
}

// Read a byte from a given address
byte LoaderHw::readByte(uint16_t address) {
    byte data = 0;

    setAddress(address);
    setDataBusMode(INPUT);
    selectReadRegister(REG_MEM);
    selectWriteRegister(0);
    delayMicroseconds(1);
    data = readDataBus();
    delayMicroseconds(1);

    return data;
}

void LoaderHw::writeControls(uint8_t data) {
    enable();
    controlBitsOffOn(CTL_ALL, data);
}

bool LoaderHw::testHardware() {
    unsigned ix;

    cmdStatus.clear();
    enable();
    clearAll();

    cmdStatus.test("Loader");
    for (uint8_t reg = 1; reg <= 10; reg++) {
        if (reg == 8) continue;
        selectWriteRegister(reg);
        delay(DELAY_LEDS);
    }
    selectWriteRegister(REG_NONE);
    for (uint8_t reg = 1; reg <= 7; reg++) {
        selectReadRegister(reg);
        delay(DELAY_LEDS);
    }
    selectReadRegister(REG_NONE);

    for (uint8_t bit = 0x20; bit; bit >>= 1) {
        controlBitsOffOn(CTL_ALL, bit);
        delay(DELAY_LEDS);
    }
    controlBitsOffOn(CTL_ALL, CTL_NONE);
    delay(DELAY_LEDS);

    cmdStatus.pass();

    // Blink the data bus LEDs
    setDataBusMode(OUTPUT);
    for (uint8_t bit = 0x80; bit; bit >>= 1) {
        writeDataBus(bit);
        delay(DELAY_LEDS);
    }
    for (uint8_t bit = 0x1; bit; bit <<= 1) {
        writeDataBus(bit);
        delay(DELAY_LEDS);
    }
    setDataBusMode(INPUT);


    // Hold the N flag on so that the Step Counter doesn't increment
    // on all of the clock pulses during the testing.
    controlBitsOffOn(CTL_ALL, CTL_N);

    testOutputRegister();

    for (ix = 0; (ix < numWoRegisters()); ix++) {
        if (!testRegister(woRegisters[ix], false)) {
            return false;
        }
    }

    for (ix = 0; (ix < numRwRegisters()); ix++) {
        unsigned reg = rwRegisters[ix];
        if (!testRegister(reg, true)) {
            return false;
        }
    }

    bool ret = true;
    if (ret) ret = testCounters(11);
    if (ret) ret = testAlu();
    if (ret) ret = testMemory(false);
    if (ret) ret = testMemory(true);

    if (ret) {
        // Leave the registers as-is if a test failed, otherwise clear all registers.
        clearAll();
    }
    return ret;
}


////////////////////////////////////////////////
// BEGIN PRIVATE METHODS
//
////////////////////////////////////////////////

// Write a byte to the memory
bool LoaderHw::burnByte(byte value, uint16_t address) {
    enable();
    setAddress(address);
    setDataBusMode(OUTPUT);
    selectWriteRegister(REG_MEM);
    writeDataBus(value);
    clkPulse();
    selectWriteRegister(REG_NONE);
    setDataBusMode(INPUT);
    delayMicroseconds(1);

    return true;
}


bool LoaderHw::valueTest(uint8_t writeVal, uint8_t readVal, const char * s, uint8_t addr) {
    if (readVal != writeVal) {
        cmdStatus.fail(writeVal, readVal, s, addr);
        return false;
    }
    return true;
}

static const uint8_t patterns[] = {
    0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff,
    0xff, 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0xff,
    0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00,
    0xaa, 0x55, 0xaa, 0x55, 0xcc, 0x33, 0xcc, 0x33, 0x00
};

bool LoaderHw::testRegister(unsigned reg, bool isRw) {
    cmdStatus.test(registerNames[reg]);

    // Special case to test the inverted value of REG_B
    bool invert = false;
    if (reg == REG_B_INVERTED) {
        reg = REG_B;
        invert = true;
        controlBitsOffOn(CTL_NONE, CTL_BI);
    }

    for (unsigned ix = 0; (ix < sizeof(patterns)); ix++) {
        uint8_t writeVal = patterns[ix];
        uint8_t expectedVal = invert ? ~writeVal : writeVal;
        writeRegister(reg, writeVal);
        delayMicroseconds(2);
        selectWriteRegister(REG_NONE);
        delay(DELAY_LEDS);
        if (isRw) {
            uint8_t readVal = readRegister(reg);
            if (!valueTest(expectedVal, readVal))  return false;
        }
    }

    cmdStatus.pass();
    selectReadRegister(REG_NONE);
    selectWriteRegister(REG_NONE);
    controlBitsOffOn(CTL_BI, CTL_NONE);
    return true;
}


// Output register has its own write-only test that counts from 0 to 255
// instead of using the test patterns that are made to look good on rows
// of LEDs.
bool LoaderHw::testOutputRegister() {
    cmdStatus.test("Output");

    for (unsigned ix = 0; (ix < 256); ix++) {
        writeRegister(REG_OUT, ix);
        delayMicroseconds(2);
        selectWriteRegister(REG_NONE);
        delay(DELAY_LEDS>>3);
    }

    cmdStatus.pass();
    selectReadRegister(REG_NONE);
    selectWriteRegister(REG_NONE);
    return true;
}


bool LoaderHw::testCounters(uint8_t count) {
    cmdStatus.test("Counters");

    for (unsigned ix = 0; (ix < sizeof(patterns)); ix++) {
        uint8_t writeVal = patterns[ix];
        writeRegister(REG_PC, writeVal);
        delayMicroseconds(2);
        writeRegister(REG_SP, writeVal);
        delayMicroseconds(2);
        selectWriteRegister(0);
        uint8_t readVal = readRegister(REG_PC);
        readRegister(REG_NONE);
        delay(20);
        if (!valueTest(writeVal, readVal, "PC read"))  return false;
        readVal = readRegister(REG_SP);
        readRegister(REG_NONE);
        if (!valueTest(writeVal, readVal, "SP read"))  return false;
        controlBitsOffOn(CTL_SP_DN, CTL_PI|CTL_SP_UP);
        for (int jx = 0; (jx < count); jx++) {
            clkPulse();
            delay(3);
        }
        readVal = readRegister(REG_PC);
        if (!valueTest(writeVal + count, readVal, "PC inc"))  return false;
        readVal = readRegister(REG_SP);
        if (!valueTest(writeVal + count, readVal, "SP inc"))  return false;
        controlBitsOffOn(CTL_PI|CTL_SP_UP, CTL_SP_DN);
        readRegister(REG_NONE);
        for (int jx = 0; (jx <= count); jx++) {
            clkPulse();
            delay(3);
        }
        readVal = readRegister(REG_SP);
        readRegister(REG_NONE);
        if (!valueTest(writeVal - 1, readVal, "SP dec"))  return false;
        controlBitsOffOn(CTL_PI|CTL_SP_DN|CTL_SP_UP, CTL_NONE);
    }

    cmdStatus.pass();
    selectReadRegister(REG_NONE);
    selectWriteRegister(REG_NONE);
    return true;
}

bool LoaderHw::testMemory(bool highMem) {
    unsigned numPatterns = sizeof(patterns);
    unsigned addr;
    uint16_t blockAddr = 0;

    if (highMem) {
        cmdStatus.test("Hi RAM");
        blockAddr = 0x100;
    } else {
        cmdStatus.test("Lo RAM");
    }
    for (unsigned offset = 0; (offset < numPatterns); offset++) {
        for (addr = 0; (addr < 256); addr++) {
            burnByte(patterns[(offset + addr) % numPatterns], addr | blockAddr);
        }
        for (addr = 0; (addr < 256); addr++) {
            uint8_t readVal = readByte(addr | blockAddr);
            uint8_t expectedVal = patterns[(offset + addr) % numPatterns];
            if (!valueTest(expectedVal, readVal, NULL, addr)) return false;
        }
    }

    burnByte(0, 0);  // set MAR back to zero
    selectReadRegister(REG_NONE);
    selectWriteRegister(REG_NONE);
    cmdStatus.pass();
    return true;
}

bool LoaderHw::testAlu() {
    cmdStatus.test("ALU add C=0");
    if (!testAdder(false, 0))  return false;
    cmdStatus.test("ALU add C=1");
    if (!testAdder(false, 1))  return false;
    cmdStatus.test("ALU sub C=0");
    if (!testAdder(true, 0))  return false;
    cmdStatus.test("ALU sub C=1");
    if (!testAdder(true, 1))  return false;

    controlBitsOffOn(CTL_BI|CTL_CX, CTL_NONE);
    return true;
}


bool LoaderHw::testAdder(bool sub, uint8_t carry) {
    unsigned numPatterns = sizeof(patterns);

    for (unsigned aIndex = 0; (aIndex < numPatterns); aIndex++) {
        delay(DELAY_LEDS);
        for (unsigned bIndex = 0; (bIndex < numPatterns); bIndex++) {
            if (!testAdderOperation(patterns[aIndex], patterns[bIndex], sub, carry)) {
                return false;
            }
        }
    }
    cmdStatus.pass();
    return true;
}


bool LoaderHw::testAdderOperation(uint8_t a, uint8_t b, bool sub, uint8_t carry) {
    writeRegister(REG_A, a);
    writeRegister(REG_B, b);
    writeRegister(REG_NONE, 0); // don't want the next CLK to write any register
    controlBitsOffOn(CTL_NONE, CTL_FL);
    clkPulse();
    controlBitsOffOn(CTL_FL, CTL_NONE);
    if (sub) {
        controlBitsOffOn(CTL_NONE, CTL_BI);
    } else {
        controlBitsOffOn(CTL_BI, CTL_NONE);
    }
    if (carry) {
        controlBitsOffOn(CTL_NONE, CTL_CX);
    } else {
        controlBitsOffOn(CTL_CX, CTL_NONE);
    }
    uint8_t expectedVal = (sub) ? a + ~b + carry : a + b + carry;
    uint8_t readVal = readRegister(REG_ALU);
    if (readVal != expectedVal) {
        char s[12];
        sprintf(s, "add %02x+%02x", a, b);
        cmdStatus.fail(expectedVal, readVal, s);
        return false;
    }

    // Test that the Adder result can be transfered back to A without corruption
    transferRegister(REG_A, REG_ALU);
    selectWriteRegister(REG_NONE);
    readVal = readRegister(REG_A);
    if (readVal != expectedVal) {
        char s[60];
        sprintf(s, "FAILED to transfer - A=%02x B=%02x, result=%02x, expected=%02x", a, b, readVal, expectedVal);
        Serial.println(s);
        return false;
    }

    return true;
}


// Set bits in the control register.  Any bits in the clearBits mask will be turned off
// and any bits in the setBits mask will be turned on.  If the same bit is present in the
// clearBits and in the setBits, it will be set.  Any bits that don't appear in either
// mask will remain unchanged.
// Examples:
//   controlBitsOffOn(CTL_ALL, CTL_NONE)      - All bits off
//   controlBitsOffOn(CTL_NONE, ALL)          - All bits on
//   controlBitsOffOn(CTL_NONE, CTL_PI|CTL_N) - PI and N bits on, others unchanged
//   controlBitsOffOn(CTL_ALL, CTL_PI|CTL_N)  - PI and N bits on, others off
static void controlBitsOffOn(uint8_t clearBits, uint8_t setBits) {
    static uint8_t controls = 0;

    controls &= ~clearBits;
    controls |= setBits;
    for (uint8_t bit = 0x80; (bit); bit >>= 1) {
        if (controls & bit)
            PORTC |= PC_SER;
        else
            PORTC &= ~PC_SER;

        PORTC |= PC_LD0;    // Clock the bit into the shifter
        PORTC &= ~PC_LD0;
    }
    PORTC |= PC_RCLK;       // Clock the register contents to the output pins
    PORTC &= ~PC_RCLK;
}

static void selectRegister() {
    for (uint8_t bit = 0x80; (bit); bit >>= 1) {
        if (valSelects & bit)
            PORTC |= PC_SER;
        else
            PORTC &= ~PC_SER;

        PORTC |= PC_LD2;
        PORTC &= ~PC_LD2;
    }
    PORTC |= PC_RCLK;
    PORTC &= ~PC_RCLK;
}

void LoaderHw::selectWriteRegister(uint8_t reg) {
    if (reg != REG_NONE) {
        enable();
    }
    valSelects = (valSelects & 0x07) | reg << 3;
    selectRegister();
}

void LoaderHw::selectReadRegister(uint8_t reg) {
    if (reg != REG_NONE) {
        // Make sure the loader is not on the bus if a register is selected.
        setDataBusMode(INPUT);
        enable();
    }
    valSelects = (valSelects & 0xf8) | reg;
    selectRegister();
}

// Set the I/O state of the data bus.
// The first two bits of port D are used for serial, so the 8 bits data bus are
// on pins D2..D9.
void LoaderHw::setDataBusMode(uint8_t mode) {
    // On the Uno and Nano, D2..D9 maps to the upper 6 bits of port D and the
    // lower 2 bits of port B.
    if (mode == OUTPUT) {
        // Make sure that none of the host hardware registers are also writing to the bus.
        selectReadRegister(REG_NONE);
        DDRB |= 0x03;
        DDRD |= 0xfc;
    } else {
        DDRB &= 0xfc;
        DDRD &= 0x03;
//        PORTB |= 0x03;  // set pullup resistors
//        PORTD |= 0xfc;
    }
}

// Set an 8-bit address on the data bus and latch it into the MAR
void LoaderHw::setAddress(uint16_t address) {
    // NOTE: Must call selectWriteRegister before setting the bus because the call to
    // selectWriteRegister may call enable and that will reset the bus.
    if (address & 0x100) {
        controlBitsOffOn(CTL_NONE, CTL_MX);
    } else {
        controlBitsOffOn(CTL_MX, CTL_NONE);
    }
    selectWriteRegister(REG_MAR);
    setDataBusMode(OUTPUT);
    writeDataBus(address);
    clkPulse();
    selectWriteRegister(REG_NONE);
    setDataBusMode(INPUT);
}

// Read a byte from the data bus.  The caller must set the bus to input_mode
// before calling this or no useful data will be returned.
byte LoaderHw::readDataBus() {
    return (PINB << 6) | (PIND >> 2);
}

// Write a byte to the data bus.  The caller must set the bus to output_mode
// before calling this or no data will be written.
void LoaderHw::writeDataBus(byte data) {
     PORTB = (PORTB & 0xfc) | (data >> 6);
     PORTD = (PORTD & 0x03) | (data << 2);
}
