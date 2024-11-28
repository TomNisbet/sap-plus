#include "Arduino.h"

static const char * MY_VERSION = "1.0";

// IO lines for the EEPROM device control
// Pins D2..D9 are used for the data bus.
#define WE              A0
#define CE              A1
#define OE              A2
#define ADDR_CLK_HI     A3
#define ADDR_CLK_LO     A4
#define ADDR_DATA       A5

#define RED_LED         13


void burnRom();
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
    Serial.print("\nBurning SAP-Plus Output Register ROM version ");
    Serial.println(MY_VERSION);
    disableSoftwareWriteProtect();
    delay(100);
    burnRom();
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

//      A
//     ---
//  F | G | B
//     ---
//  E |   | C
//     ---
//      D

// LED segment bits
enum {
    SEG_DP = 0x80,
    SEG_G = 0x40,
    SEG_F = 0x20,
    SEG_E = 0x10,
    SEG_D = 0x08,
    SEG_C = 0x04,
    SEG_B = 0x02,
    SEG_A = 0x01
};

// Character definitions
uint8_t segments[] = {
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,        // 0
    SEG_B|SEG_C,                                // 1
    SEG_A|SEG_B|SEG_D|SEG_E|SEG_G,              // 2
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_G,              // 3
    SEG_B|SEG_C|SEG_F|SEG_G,                    // 4
    SEG_A|SEG_C|SEG_D|SEG_F|SEG_G,              // 5
    SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,        // 6
    SEG_A|SEG_B|SEG_C,                          // 7
    SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,  // 8
    SEG_A|SEG_B|SEG_C|SEG_F|SEG_G,              // 9
    SEG_A|SEG_B|SEG_C|SEG_E|SEG_F|SEG_G,        // A
    SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,              // B
    SEG_A|SEG_D|SEG_E|SEG_F,                    // C
    SEG_B|SEG_C|SEG_D|SEG_E|SEG_G,              // D
    SEG_A|SEG_D|SEG_E|SEG_F|SEG_G,              // E
    SEG_A|SEG_E|SEG_F|SEG_G                     // F
};

uint8_t bin_segments[] = {
    SEG_E|SEG_C,
    SEG_E|SEG_B,
    SEG_F|SEG_C,
    SEG_F|SEG_B
};

enum {
    DIGIT_0 = 0x000,
    DIGIT_1 = 0x100,
    DIGIT_2 = 0x200,
    DIGIT_3 = 0x300
};

enum {
    OFFSET_SIGNED   = 0x000,
    OFFSET_UNSIGNED = 0x400,
    OFFSET_HEX      = 0x800,
    OFFSET_BINARY   = 0xc00
};

uint8_t buffer[1024];
void burnRom() {
    for (int ix = -128; (ix < 128); ix++) {
        int n = abs(ix);
        byte u8 = byte(ix);
        buffer[DIGIT_0 + u8] = segments[n % 10];
        buffer[DIGIT_1 + u8] = segments[(n / 10) % 10];
        buffer[DIGIT_2 + u8] = segments[(n / 100) % 10];
        buffer[DIGIT_3 + u8] = (ix < 0) ? SEG_G : 0;
    }
    if (!writeData(buffer, sizeof(buffer), OFFSET_SIGNED))  fail();

    for (unsigned ix = 0; (ix < 256); ix++) {
        buffer[DIGIT_0 + ix] = segments[ix % 10];
        buffer[DIGIT_1 + ix] = segments[(ix / 10) % 10];
        buffer[DIGIT_2 + ix] = segments[(ix / 100) % 10];
        buffer[DIGIT_3 + ix] = 0;
    }
    if (!writeData(buffer, sizeof(buffer), OFFSET_UNSIGNED))  fail();

    for (unsigned ix = 0; (ix < 256); ix++) {
        buffer[DIGIT_0 + ix] = segments[(ix)      & 0x0f];
        buffer[DIGIT_1 + ix] = segments[(ix >> 4) & 0x0f];
        buffer[DIGIT_2 + ix] = 0;
        buffer[DIGIT_3 + ix] = 0;
    }
    if (!writeData(buffer, sizeof(buffer), OFFSET_HEX))  fail();

    for (unsigned ix = 0; (ix < 256); ix++) {
        buffer[DIGIT_0 + ix] = bin_segments[(ix)      & 0x03];
        buffer[DIGIT_1 + ix] = bin_segments[(ix >> 2) & 0x03];
        buffer[DIGIT_2 + ix] = bin_segments[(ix >> 4) & 0x03];
        buffer[DIGIT_3 + ix] = bin_segments[(ix >> 6) & 0x03];
    }
    if (!writeData(buffer, sizeof(buffer), OFFSET_BINARY))  fail();
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
