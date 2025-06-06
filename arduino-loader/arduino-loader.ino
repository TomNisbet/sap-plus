/**
**/

#include "CmdStatus.h"
#include "LoaderHw.h"

static const char * MY_VERSION = "2.3";

// Global loader hardware interface
LoaderHw hw;
CmdStatus cmdStatus;

// Instruction opcodes.  Those marked with an asterisk use the ALU and thus must have
// specific opcodes that match the bits that are hardwired from the IR to the ALU control.
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
    N_LAX = 0x0e,  //   load A indexed by SP
    N_SAX = 0x0f,  //   store A indexed by SP
    N_JMP = 0x10,  //   jump unconditional
    N_JC =  0x11,  //   jump on Carry
    N_JZ =  0x12,  //   jump on Zero
    N_JNC = 0x13,  //   jump on no Carry
    N_JNZ = 0x14,  //   jump on no Zero
    N_PHA = 0x15,  //   push A
    N_PLA = 0x16,  //   pull A
    N_JSR = 0x17,  //   jump to subroutine
    N_RTS = 0x18,  //   return from subroutine
    N_RC =  0x19,  //   return if carry
    N_RZ =  0x1a,  //   return if zero
    N_RNC = 0x1b,  //   return if not carry
    N_RNZ = 0x1c,  //   return if not zero
    N_INS = 0x1d,  //   increment SP
    N_DCS = 0x1e,  //   decrement SP
    N_ADI = 0x20,  //   add immediate to A
    N_ADM = 0x21,  //   add memory to A
    N_SBI = 0x22,  //   subtract immediate from A
    N_SBM = 0x23,  //   subtract memory from A
    N_ACI = 0x24,  //   add immediate to A with carry
    N_ACM = 0x25,  //   add memory to A with carry
    N_SCI = 0x26,  //   subtract immediate from A with carry
    N_SCM = 0x27,  //   subtract memory from A with carry
    N_CPI = 0x28,  //   compare immediate to A
    N_CPM = 0x29,  //   compare memory to A
    N_CYN = 0x3f   //   can you not
};


static const uint8_t pgmShift[] = {
    N_LAI, 0x01,
    N_ASL,
    N_ASL,
    N_ASL,
    N_ASL,
    N_ASL,
    N_ASL,
    N_ASL,
    N_ASL,
    N_JMP, 0x00
};

static const uint8_t pgmFastCount[] = {
    // No code - special case - the loader fills memory with repeated N_INA, N_OUT
};

static const uint8_t pgmCount3[] = {
    // Count by 3
    N_NOP,
    N_LAI, 0,    // start at 0
// LOOP
    N_ACI, 3,    // ADD 3 to A
    N_OUT,       // OUT (A to display)
    N_JMP, 3     // JMP back to LOOP
};

#include "pgmSquares.h"
#include "pgmBounce.h"
#include "pgmFibonacci.h"
#include "pgmPrimes.h"
#include "pgmStackTest.h"


struct program_t {
    unsigned        seconds;
    const uint8_t * data;
    size_t          len;
    const char *    name;
};
static const program_t programs[] = {
    20,     pgmBounce,      sizeof(pgmBounce),    "Bounce",
    20,     pgmSquares,     sizeof(pgmSquares),   "Squares",
    20,     pgmFastCount,   0,                    "Fast Count",
    30,     pgmFibonacci,   sizeof(pgmFibonacci), "Fibonacci",
    10,     pgmShift,       sizeof(pgmShift),     "Shift",
    20,     pgmCount3,      sizeof(pgmCount3),    "Count by 3",
    100,    pgmPrimes,      sizeof(pgmPrimes),    "Primes",
    30,     pgmStackTest,   sizeof(pgmStackTest), "Stack Test"
};


/*****************************************************************************/
/*****************************************************************************/

/**
 * CLI parse functions
 */
static const char hex[] = "0123456789abcdef";

void setup() {
    Serial.begin(115200);
    Serial.println("Press RETURN to start the interactive monitor");
    hw.begin();
    cmdStatus.clear();

    if (!waitMillisForChar(5000)) {
        // No key pressed for several seconds at power up.  Start demo mode.
        demoMode();
    }

    cmdStatus.clear();
    // If a character is received on the serial port, then Demo Mode will exit and control
    // will fall through to the processCommand loop below.
}

void loop() {
    processCommand();
}


void runProgram(unsigned pgmIx) {
    char s[50];
    const unsigned BEAT_DELAY = 800;
    if (pgmIx < (sizeof(programs) / sizeof(*programs))) {
        const program_t * pgm = programs + pgmIx;
        const uint8_t * pgmData = pgm->data;
        size_t pgmLen = pgm->len;

        hw.enable();
        delay(BEAT_DELAY);
        hw.clearAll();
        delay(BEAT_DELAY);

        cmdStatus.program(pgm->name, CmdStatus::OP_LOADING);
        if (pgmData == pgmFastCount) {
            // Special case
            burnFastCount();
        } else {
            sprintf(s, "Burning %s program (%u) len=%u", pgm->name, pgmIx, pgmLen);
            Serial.println(s);
            for (unsigned ix = 0; (ix < pgmLen); ix++) {
                hw.writeData(pgmData + ix, 1, ix);
                delay(80);
            }
            for (unsigned ix = 0; (ix < pgmLen); ix++) {
                uint8_t b = hw.readByte(ix);
                if (b != pgmData[ix]) {
                    sprintf(s, "ERROR: mem[%02x]=%02x  pgm[%02x]=%02x", ix, b, ix, pgmData[ix]);
                    Serial.println(s);
                }
            }
        }

        delay(BEAT_DELAY);
        cmdStatus.program(pgm->name, CmdStatus::OP_RUNNING);
        hw.clearAll();
        delay(BEAT_DELAY);
        hw.disable();
    }
}


// Cycle through all of the demo programs continuously.  Step if the used presses a
// key to enter the interactive monitor.
void demoMode() {
    unsigned numPrograms = sizeof(programs) / sizeof(*programs);
    unsigned ix = 0;
    bool keyPressed = false;

    Serial.println("Starting Demo Mode");
    while (!keyPressed) {
        unsigned runSeconds = programs[ix].seconds;
        runProgram(ix);
        if (++ix >= numPrograms) {
            ix = 0;
        }

        // Run the program for runSeconds or until a key press
        while (!keyPressed && runSeconds--) {
            // Wait one second or until a char is received
            cmdStatus.remaining(runSeconds);
            keyPressed = waitMillisForChar(1000);
        }
    }
}


// wait until a character is ready on the serial port
char waitChar() {
    while (!Serial.available()) {}
    return Serial.read();
}

// wait msWaitTime or until a character is ready on the serial port.  Returns
// true if a char is ready to read.
bool waitMillisForChar(unsigned long msWaitTime) {

     unsigned long start = millis();
     while (millis() - start < msWaitTime) {
        if (Serial.available()) {
            return true;
        }
     }

    // No character available before msWaitTime elapsed
    return false;
}

// Read a line of data from the serial connection.
char * readLine(char * buffer, int len) {
    for (int ix = 0; (ix < len); ix++) {
         buffer[ix] = 0;
    }

    // read serial data until linebreak or buffer is full
    char c = ' ';
    int ix = 0;
    do {
        c = waitChar();
        if (c == '\\') {
            // Do a cYcle command without waiting for newline character.
            buffer[0] = '\\';
            ix = 2;
            break;
        } else if (c == '\b') {
            // Backspace, forget last character.
            if (ix > 0) {
                --ix;
            }
        } else {
            buffer[ix++] = c;
        }
        Serial.write(c);
    } while ((c != '\n') && (ix < len));

    buffer[ix - 1] = '\0';
    return buffer;
}


/************************************************************
* convert a single hex character [0-9a-fA-F] to its value
* @param char c single character (digit)
* @return byte value of the digit (0-15)
************************************************************/
byte hexDigit(char c) {
    if ((c >= '0') && (c <= '9')) {
        return c - '0';
    } else if ((c >= 'a') && (c <= 'f')) {
        return c - 'a' + 10;
    } else if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
    } else {
        return 0xff;
    }
}

/************************************************************
* Convert a hex string to a uint32_t value.
* Skips leading spaces and terminates on the first non-hex
* character.  Leading zeroes are not required.
*
* No error checking is performed - if no hex is found then
* defaultValue is returned.  Similarly, a hex string of more than
* 8 digits will return the value of the last 8 digits.
* @param pointer to string with the hex value of the word (modified)
* @return unsigned int represented by the digits
************************************************************/
uint32_t getHex32(char *& pData, uint32_t defaultValue=0) {
    uint32_t u32 = 0;

    while (isspace(*pData)) {
        ++pData;
    }

    if (isxdigit(*pData)) {
        while (isxdigit(*pData)) {
            u32 = (u32 << 4) | hexDigit(*pData++);
        }
    } else {
        u32 = defaultValue;
    }

    return u32;
}


void printByte(byte b) {
    char line[3];

    line[0] = hex[b >> 4];
    line[1] = hex[b & 0x0f];
    line[2] = '\0';

    Serial.print(line);
}


void printWord(word w) {
    char line[5];

    line[0] = hex[(w >> 12) & 0x0f];
    line[1] = hex[(w >>  8) & 0x0f];
    line[2] = hex[(w >>  4) & 0x0f];
    line[3] = hex[(w)       & 0x0f];
    line[4] = '\0';

    Serial.print(line);
}


/*
* Prints a 32 bit value as a hex.
*
* Note that no values over 4 digits are used in
* this application, so only 4 digits are printed.*/
void printHex32(uint32_t u32) {
    char line[6];

    line[0] = hex[(u32 >> 12) & 0x0f];
    line[1] = hex[(u32 >>  8) & 0x0f];
    line[2] = hex[(u32 >>  4) & 0x0f];
    line[3] = hex[(u32)       & 0x0f];
    line[4] = '\0';

    Serial.print(line);
}


// If the user presses a key then pause until they press another.  Return true if
// Ctrl-C is pressed.
bool checkForBreak() {
    if (Serial.available()) {
        if (Serial.read() == 0x03) {
            return true;
        }
        while (!Serial.available())
        {}
        if (Serial.read() == 0x03) {
            return true;
        }
    }

    return false;
}


/*****************************************************************************/
/*****************************************************************************/
/**
 * Command implementations
 */


/**
* Read data from the device and dump it in hex and ascii.
**/
void dumpBlock(uint32_t start, uint32_t end) {
    char line[81];
//  01234567891 234567892 234567893 234567894 234567895 234567896 234567897 23456789
//  01234: 01 23 45 67  89 ab cf ef  01 23 45 67  89 ab cd ef  1.2.3.4. 5.6.7.8.
    int count = 0;

    memset(line, ' ', sizeof(line));

    char * pHex = line;
    char * pChar = line + 59;
    for (uint32_t addr = start; (addr <= end); addr++) {
        if (count == 0) {
            //print out the address at the beginning of the line
            pHex = line;
            pChar = line + 59;
            *pHex++ = hex[(addr >> 16) & 0x0f];
            *pHex++ = hex[(addr >> 12) & 0x0f];
            *pHex++ = hex[(addr >>  8) & 0x0f];
            *pHex++ = hex[(addr >>  4) & 0x0f];
            *pHex++ = hex[(addr)       & 0x0f];
            *pHex++ = ':';
            *pHex++ = ' ';
        }

        byte data = hw.readByte(addr);
        *pHex++ = hex[data >> 4];
        *pHex++ = hex[data & 0x0f];
        *pHex++ = ' ';
        *pChar++ = ((data < 32) | (data >= 127)) ? '.' : data;

        if ((count & 3) == 3) {
            *pHex++ = ' ';
        }
        if ((count & 7) == 7) {
            *pChar++ = ' ';
        }
        if ((++count >= 16) || (addr == end)) {
            *pChar = '\0';
            Serial.println(line);
            if (checkForBreak()) {
                return;
            }
            memset(line, ' ', sizeof(line));
            count = 0;
        }
    }

    if (count) {
        Serial.println();
    }
}

/**
* Print an address and value and then prompt the user for input.  If two hex characters
* are entered then the value is stored in memory at the current address the the process
* is repeated for the next address.  The loop stops when a non-hex character is entered.
**/
void examine(uint32_t addr) {
    uint8_t val;
    uint8_t x;
    char c;
    while (1) {
        val = 0;
        printWord(addr);
        Serial.print(':');
        printByte(hw.readByte(addr));
        Serial.print(' ');
        Serial.flush();
        c = waitChar();
        if ((c == '\n') || (c == '\r')) {
            // The Arduino IDE serial monitor requires a newline so read and ignore
            // newlineif it is seen.
            Serial.println();
            continue;
        }
        x = hexDigit(c);
        if (x == 0xff) {
            break;
        }
        val = x << 4;
        x = hexDigit(waitChar());
        if (x == 0xff) {
            break;
        }
        hw.writeData(&val, 1, addr++);
        Serial.println();
    }
    Serial.println();
}

/**
 * Fill a block of RAM data with a single value.
 *
 * @param start - start address
 * @param end - end address
 * @param val - data byte to write to all addresses
 */
void fillBlock(uint32_t start, uint32_t end, byte val) {
    enum { BLOCK_SIZE = 32 };
    byte block[BLOCK_SIZE];

    for (int ix = 0; ix < BLOCK_SIZE; ix++) {
        block[ix] = val;
    }

    for (uint32_t addr = start; (addr <= end); addr += BLOCK_SIZE) {
        uint32_t writeLen = ((end - addr + 1) < BLOCK_SIZE) ? (end - addr + 1) : uint32_t(BLOCK_SIZE);
        if (!hw.writeData(block, writeLen, addr)) {
            cmdStatus.error("Write failed");
            return;
        }
    }
}

void burnFastCount() {
    Serial.println(F("Filling program memory with FastCount"));
    hw.enable();
    uint8_t buffer[2] = { N_OUT, N_INA };
    for (unsigned ix = 0; (ix < 256); ix+= sizeof(buffer)) {
        bool status = hw.writeData(buffer, sizeof(buffer), ix);
        if (!status) {
            cmdStatus.error("Write failed");
            return;
        }
    }
}

void burnProgram(unsigned pgmIx, uint32_t start) {
    char s[50];
    if (pgmIx < (sizeof(programs) / sizeof(*programs))) {
        const uint8_t * pgmData = programs[pgmIx].data;
        size_t pgmLen = programs[pgmIx].len;
        const char * pgmName = programs[pgmIx].name;

        cmdStatus.program(pgmName, CmdStatus::OP_RUNNING);
        if (pgmData == pgmFastCount) {
            // Special case
            burnFastCount();
        } else {
            sprintf(s, "Burning %s program (%u) at %04x len=%u", pgmName, pgmIx, unsigned(start), pgmLen);
            Serial.println(s);
            hw.enable();
            bool status = hw.writeData(pgmData, pgmLen, start);
            if (!status) {
                cmdStatus.error("Write failed");
                return;
            }

            for (unsigned ix = 0; (ix < pgmLen); ix++) {
                byte val = hw.readByte(start + ix);
                if (val != pgmData[ix]) {
                    //cmdStatus.fail(pgmData[ix], val, NULL, start+ix);
                    return;
                }
            }
            cmdStatus.info("Write verified");
        }
        hw.reset();
    }
}

void insertBytes(char * pCursor) {
    uint32_t val;
    uint32_t start;
    unsigned byteCtr = 0;

    enum { BLOCK_SIZE = 32 };
    byte data[BLOCK_SIZE];

    //first value returned is the starting address
    start = getHex32(pCursor, 0);
    while (((val = getHex32(pCursor, 0xffff)) != 0xffff) && (byteCtr < BLOCK_SIZE)) {
        data[byteCtr++] = byte(val);
    }

    if (byteCtr > 0) {
        if (!hw.writeData(data, byteCtr, start)) {
            cmdStatus.error("Write failed");
            return;
        }
    } else {
        cmdStatus.error("Missing address or data");
        return;
    }
    delay(100);

    for (unsigned ix = 0; ix < byteCtr ; ix++) {
        byte val = hw.readByte(start + ix);
        if (val != data[ix]) {
            cmdStatus.fail(data[ix], val, NULL, start+ix);
            return;
        }
    }
    cmdStatus.info(" successful");
}

void listPrograms() {
    unsigned n = sizeof(programs) / sizeof(*programs);
    for (unsigned ix = 0; (ix < n); ix++) {
        Serial.print(ix);
        Serial.print(" - ");
        Serial.println(programs[ix].name);
    }
}
void printRegisterNames() {
    for (int num = 0; (num < 16); num++) {
        Serial.print("  ");
        Serial.print(num, HEX);
        Serial.print(" - ");
        Serial.println(hw.registerName(num));
    }
}

enum {
    // CLI Commands
    CMD_INVALID,
    CMD_CTRL,
    CMD_DUMP,
    CMD_FILL,
    CMD_INSERT,
    CMD_GET,
    CMD_LIST,
    CMD_NAMES,
    CMD_PUT,
    CMD_QUIT,
    CMD_TEST,
    CMD_EXAMINE,
    CMD_CYCLE,
    CMD_ZAP,
    CMD_DOLLAR,
    CMD_ASSIGN,
    CMD_CLOCK
};


/*
G5 - get r5 into $ and print
P6 2f- put value 2f into r6 - can use $ for value
=2 4 - transfer r2 from r4 (dest first, like P command)
$ - print last value
$ 3c - set value of $ to 3c
Y - cYcle clock - for example, if IR has an ALU op will change values
The G,P,T commands will leave the RW selects set for additional Y cmds
Dssss eeee dump memory
Fssss eeee vv fill memory
Xssss eXamine memory will step through and can change values see gwmon
T - test memory, alu, and all registers.  R/W registers will verify a set of values. R/O registers will do the light show. Some R/O are implicitly tested.
*/
byte parseCommand(char c) {
    byte cmd = CMD_INVALID;

    // Convert the command to lowercase.
    if ((c >= 'A') && (c <= 'Z')) {
        c |= 0x20;
    }

    switch (c) {
        case 'c':  cmd = CMD_CTRL;      break;
        case 'd':  cmd = CMD_DUMP;      break;
        case 'f':  cmd = CMD_FILL;      break;
        case 'g':  cmd = CMD_GET;       break;
        case 'i':  cmd = CMD_INSERT;    break;
        case 'l':  cmd = CMD_LIST;      break;
        case 'n':  cmd = CMD_NAMES;     break;
        case 'p':  cmd = CMD_PUT;       break;
        case 'q':  cmd = CMD_QUIT;      break;
        case 't':  cmd = CMD_TEST;      break;
        case 'x':  cmd = CMD_EXAMINE;   break;
        case 'y':  cmd = CMD_CYCLE;     break;
        case 'z':  cmd = CMD_ZAP;       break;
        case '$':  cmd = CMD_DOLLAR;    break;
        case '=':  cmd = CMD_ASSIGN;    break;
        case '\\': cmd = CMD_CLOCK;     break;
        default:   cmd = CMD_INVALID;   break;
    }

    return cmd;
}

void usage() {
    Serial.print(F("SAP-Plus Loader/Debugger "));
    Serial.println(MY_VERSION);
    Serial.println(F("RAM commands:"));
    Serial.println(F("  Dss ee    - Dump bytes from RAM to terminal"));
    Serial.println(F("  Fss ee dd - Fill block of RAM with fixed value"));
    Serial.println(F("  Iss dd .. - Insert one or more values into RAM"));
    Serial.println(F("  Xss       - eXamine (and optionally modify) RAM"));

    Serial.println(F("\nRegister and hardware commands:"));
    Serial.println(F("  Ccc       - Set control bits to cc (0..3f)"));
    Serial.println(F("  Gr        - Get (read) and print register value"));
    Serial.println(F("  Pr dd     - Put (write) value to register"));
    Serial.println(F("  =r1 r2    - Assign (r1=r2) copy value from register r2 to r1"));
    Serial.println(F("  Y[cc][ww] - cYcle host hardware clock (optional pulse count and width in uSec)"));

    Serial.println(F("\nMisc commands:"));
    Serial.println(F("  L         - List built-in program numbers and names"));
    Serial.println(F("  N         - Print register numbers and names"));
    Serial.println(F("  T         - Test host hardware"));
    Serial.println(F("  Zpp ss    - Zap (burn) stored program number pp to start address ss"));
    Serial.println(F("  Q         - Quit loader mode and return control to host"));
}

void processCommand() {
    char line[60];
    char buff[50];
    uint32_t a1, a2, a3;
    byte lastVal = 0;
    uint32_t numBytes;
    uint32_t noArg = uint32_t(-1);
    int argc = 0;

    Serial.print("\n>");
    Serial.flush();
    readLine(line, sizeof(line));
    Serial.println();
    byte cmd = parseCommand(line[0]);
    char * pCursor = line+1;
    if ((a1 = getHex32(pCursor, noArg)) != noArg) { ++argc; }
    if ((a2 = getHex32(pCursor, noArg)) != noArg) { ++argc; }
    if ((a3 = getHex32(pCursor, noArg)) != noArg) { ++argc; }

    switch (cmd) {
    case CMD_CTRL:
        hw.writeControls(uint8_t(a1));
        break;

    case CMD_DUMP:
        if (argc < 2) { a2 = a1 + 255; }
        dumpBlock(a1, a2);
        break;

    case CMD_GET:
        lastVal = hw.readRegister(a1);
        Serial.println(lastVal, HEX);
        break;

    case CMD_PUT:
        // Note that this command will not work with the Output Register because it uses
        // a latch instead of a clock pulse to store its data.  This command will
        // briefly put a value on the bus (and into the output register), but immediately
        // after the clock pulse, the loader will release the bus and the default zero
        // value will show on the Output Register.  The Assign command will work correctly
        // with Output because it leaves the read register on the bus upon completion.
        hw.writeRegister(a1, a2);
        break;

    case CMD_INSERT:
        insertBytes(line + 1);
        break;

    case CMD_LIST:
        listPrograms();

        break;

    case CMD_EXAMINE:
        if (argc < 1) { a1 = 0; }
        examine(a1);
        break;

    case CMD_ASSIGN:
        hw.transferRegister(a1, a2);
        break;

    case CMD_FILL:
        if (argc < 2) { a2 = a1 + 255; }
        fillBlock(a1, a2, byte(a3));
        break;

    case CMD_QUIT:
        hw.disable();
        Serial.println(F("Loader disabled, control returned to host."));
        break;

    case CMD_TEST:
        if (hw.testHardware()) {
            Serial.println(F("Hardware test passed"));
        } else {
            Serial.println(F("Hardware test failed"));
        }
        break;

    case CMD_CYCLE:
        if (argc < 1) { a1 = 1; }
        if (argc < 2) { a2 = 500; }
        for (uint32_t ix = 0; (ix < a1); ix++) {
            hw.clkPulseWidth(a2);
        }
        break;

    case CMD_CLOCK:
        hw.clkPulse();
        break;

    case CMD_NAMES:
        printRegisterNames();
        break;

    case CMD_DOLLAR:
        sprintf(buff, "%u (0x%02x)", lastVal, lastVal);
        Serial.println(buff);
        break;

    case CMD_ZAP:
        if (argc < 2) { a2 = 0; }
        burnProgram(a1, a2);
        break;

    default:
        usage();
        break;
    }
}
