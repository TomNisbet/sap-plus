// SSD1306lite
//
// Tom Nisbet 2024  https://github.com/TomNisbet/SSD1306lite
//
// Very lightweight Arduino hardware driver and text display methods for
// SSD1306-based I2C 128x64 OLED displays.
//
// This code works with 128x64 I2C OLED displays and only supports text and
// very basic bitmap drawing.  It does not support scrolling or arbitrary
// drawing functions.  It uses minimal RAM and does not require any
// support libraries.
//
// The I2C code is bit-banged and does not listen for ACK/NACK from the display.
// This takes liberties with the I2C standards, but it does work for the SSD1306
// hardware.
// 
// This code does not use the Arduino Wire library and requires no buffer space.
//
// The low-level I2C code is based on SSD1306xLED from the ATtiny85 tinusaur project
// by Neven Boyanov https://bitbucket.org/tinusaur/ssd1306xled 
// which was itself inspired by IIC_wtihout_ACK http://www.14blog.com/archives/1358.

#include "ssd1306lite.h"
#include "font6x8.h"
#include "font8x16.h"

// The slave address of an SSD1306 is seven bits and should be either 0x3c or 0x3d.
// The bit following the seven address bits is the read/write bit and it is always
// set to zero to indicate that the microcontroller is writing to the display.
// 
// The address and R/W bit are combined below so that the sendByte code can send the
// adddress and R/W bit as a single byte.
//
// Change the SSD1306_ADDR to match the i2C slave address of the display.
// Some displays may already be marked Addr=78 rather than Addr=3C, but the
// code below should always be 0x78 or 0x79 two cover the two possible
// addresses used by the controller.
#define SSD1306_ADDR    0x78    // Slave address of the display (0x3c << 1) | 0
//#define SSD1306_ADDR    0x79    // Slave address of the display (0x3d << 1) | 0

// Communication pin definitions.  
// The default communication pins for an Arduino Uno or Nano are A5 for SCL and A4
// for SDA.  To use different pins on these Arduinos or to use a different Arduino
// type, lookup the mapping of the Arduino pins to hardware ports and change the
// PORT, DDR, and PIN definitions below to match.
#define SCL_PORT        PORTC
#define SCL_DDR         DDRC
#define SCL_PIN         PC5     // Arduino A5 - connect to SCL on SSD1306 display
#define SDA_PORT        PORTC
#define SDA_DDR         DDRC
#define SDA_PIN         PC4     // Arduino A4 - connect to SDA on SSD1306 display


// Functions to set the SCL and SDA bits as output and to set the bits high and low.
// All hardware changes can be handled in the definitions above. so there should be
// no need to edit this code.
inline void SCL_MODE_OUTPUT() { SCL_DDR |= (1 << SCL_PIN); }
inline void SDA_MODE_OUTPUT() { SDA_DDR |= (1 << SDA_PIN); }
inline void SCL_high() { SCL_PORT |=  (1 << SCL_PIN); }
inline void SCL_low()  { SCL_PORT &= ~(1 << SCL_PIN); }
inline void SDA_high() { SDA_PORT |=  (1 << SDA_PIN); }
inline void SDA_low()  { SDA_PORT &= ~(1 << SDA_PIN); }


// SSD1306 Display Controller commands
enum {
    CMD_SET_COLUMN_LO =         0x00,   // commands 00..0f set low nibble of column start
    CMD_SET_COLUMN_HI =         0x10,   // commands 10..1f set high nibble of start address
    CMD_ADDRESS_MODE =          0x20,   // one byte argument 0=horiz, 1=vert, 2=page (default)
    CMD_SET_START_LINE =        0x40,   // commands 40..7f set start line from 0..63
    CMD_SET_CONTRAST =          0x81,   // one byte argument sets contrast level 0..255
    CMD_CHARGE_PUMP =           0x8d,   // one byte argument 0x10=disable, 0x14=enable
    CMD_HORIZONTAL_NORMAL =     0xa0,   // column 0 to SEG0  (write from 0 to 127)
    CMD_HORIZONTAL_REMAP =      0xa1,   // column 127 to SEG0  (write from 127 to 0)
    CMD_RAM_ENABLE =            0xa4,   // display follows RAM content
    CMD_RAM_DISABLE =           0xa5,   // display all ON
    CMD_INVERT_OFF =            0xa6,   // normal pixel values (RAM bit set = pixel ON) (default)
    CMD_INVERT_ON =             0xa7,   // inverted pixel values (RAM bit set = pixel OFF)
    CMD_MULTIPLEX_RATIO =       0xa8,   // one byte argument 0..63 to set values up to 64MUX
    CMD_DISPLAY_OFF =           0xae,   // turn display off
    CMD_DISPLAY_ON =            0xaf,   // turn display on
    CMD_SET_ROW =               0xb0,   // commands b0..b7 set page start address (row) in page mode
    CMD_VERTICAL_NORMAL =       0xc0,   // row 0 to row 7
    CMD_VERTICAL_REMAP =        0xc8,   // row 7 to row 0
    CMD_DISPLAY_OFFSET =        0xD3,   // one byte argument 0..63
    CMD_DIVIDE_AND_FREQ =       0xD5,   // display clock divide ratio and oscillator frequency
    CMD_PRE_CHARGE_PERIOD =     0xD9,   // set pre-charge period
    CMD_COM_PIN_CONFIG =        0xDA,   // COM pins hardware configuration     
    CMD_VCOMH_LEVEL =           0xDB    // Set VCOMH deselect level
};


// If display is upside down, use VERTICAL_NORMAL and HORIONTAL_NORMAL instead of _REMAP
static const uint8_t initCommands[] PROGMEM = {  
    CMD_DISPLAY_OFF,            // display off while doing initial setup
    CMD_MULTIPLEX_RATIO, 63,    // mux ratio 64MUX (default)
    CMD_DISPLAY_OFFSET, 0,      // display offset zero (default)
    CMD_SET_START_LINE,         // start line address zero (default)
    CMD_HORIZONTAL_REMAP,       // COM output scan direction (horizontal 127..0)
    CMD_VERTICAL_REMAP,         // segment Re-map (vertical 63..0)
    CMD_ADDRESS_MODE, 2,        // memory addressing mode to Page Addressing (default)
    CMD_SET_CONTRAST, 127,      // contrast set to middle range (default)
    CMD_INVERT_OFF,             // (default)
    CMD_RAM_ENABLE,             // (default)
    CMD_DIVIDE_AND_FREQ, 0xF0,  // display clock divide ratio and oscillator frequency
    CMD_PRE_CHARGE_PERIOD, 0x22,// set pre-charge period (default)
    CMD_COM_PIN_CONFIG, 0x12,   // COM pins hardware configuration (default)      
    CMD_VCOMH_LEVEL, 0x20,      // deselect level 0.77 x Vcc (default)
    CMD_CHARGE_PUMP, 0x14,      // enable charge pump (0x10=disable, 0x14=enable)
    CMD_DISPLAY_ON              // turn display on at completion of configuration
};


SSD1306Display::SSD1306Display(void) {
    fInvertData = false;
}


void SSD1306Display::initialize(void) {
    SCL_MODE_OUTPUT();
    SDA_MODE_OUTPUT();
    SCL_high();         // SCL and SDA are both high when line is idle
    SDA_high();

    
    // Send all of the commands in the init table at startup
    ssd1306CmdBegin();
    for (uint8_t ix = 0; (ix < sizeof(initCommands)); ix++) {
        i2cSendByte(pgm_read_byte(&initCommands[ix]));
    }
    ssd1306CmdEnd();
}


// setPosition
//
// Note that the row argument is 0..8, specifying a display line of 8 vertical pixels.
// The column argument is 0..127, specifying a horizontal pixel.  This can be a bit
// confusing when drawing characters because the row is the size of an entire character
// but the column is just one pixel.  So to draw a 6x8 character on row 2 at the 5th
// character position, the r,c value would be {2, 6*5} rather than {2, 5}.
void SSD1306Display::setPosition(uint8_t row, uint8_t column) {
    if ((row >= NUM_ROWS) || (column >= NUM_COLUMNS))  return;
    
    ssd1306CmdBegin();
    i2cSendByte(CMD_SET_ROW | row);
    i2cSendByte(CMD_SET_COLUMN_HI | ((column >> 4) & 0x0f));
    i2cSendByte(CMD_SET_COLUMN_LO | (column & 0x0f));
    ssd1306CmdEnd();
}


void SSD1306Display::invertData(bool b) { fInvertData = b; }

// Text drawing methods using either the 6x8 font or the double-height 8x16 font.
// When using 2x text, the specified row is the upper of the two rows, so text2x on
// row 3 is drawn on rows 3 and 4.
//
// Both text methods can be used together, for example one line of 2x text on rows 0..1 and
// six lines of normal text on rows 2..7.
//
// When usjng 2x text, the text does not need to start on an even line, so four lines of large
// text could be placed on rows 0, 2, 4, 6 or three lines of large text could be placed on
// rows 0, 3, 6 with rows 2 and 5 empty for spacing.
//
// Any text that would extend past the end of a screen row is clipped.

// text
//
// Draw text using the 6x8 font.  Maximun text on screen is 8 line of 21 characters.
void SSD1306Display::text(uint8_t row, uint8_t column, const char * str) {
    if (row > NUM_ROWS - 1)  return;

    setPosition(row, column);
    ssd1306DataBegin();
    const char * s = str;
    for (uint8_t col = column; *s && (col <= NUM_COLUMNS - 6); s++, col += 6) {
        uint8_t c = (*s > '{') ? 0 : *s - 32;
        for (uint8_t ix = 0; ix < 6; ix++) {
            ssd1306DataPutByte(pgm_read_byte(&font6x8[c * 6 + ix]));
        }
    }
    ssd1306DataEnd();
}

// text2x
//
// Draw text using the 8x16 font.  Maximum text on screen is 4 lines of 16 characters.
void SSD1306Display::text2x(uint8_t row, uint8_t column, const char * str) {
    if (row > NUM_ROWS - 2)  return;

    setPosition(row, column);
    ssd1306DataBegin();
    const char * s = str;
    for (uint8_t col = column; *s && (col <= NUM_COLUMNS - 8); s++, col += 8) {
        uint8_t c = *s > '}' ? 0 : *s - 32;
        for (int ix = 0; ix < 8; ix++) {
            ssd1306DataPutByte(pgm_read_byte(&font8x16[c * 16 + ix]));
        }
    }
    ssd1306DataEnd();
    
    setPosition(row + 1, column);
    ssd1306DataBegin();
    s = str;
    for (uint8_t col = column; *s && (col <= NUM_COLUMNS - 8); s++, col += 8) {
        uint8_t c = *s > '}' ? 0 : *s - 32;
        for (int ix = 0; ix < 8; ix++) {
            ssd1306DataPutByte(pgm_read_byte(&font8x16[c * 16 + 8 + ix]));
        }
    }
    ssd1306DataEnd();
}

// fillScreen
//
// Fill the entire screen with a single byte value.  The fillByte argument specifies
// 8 bits that are drawn with bit zero on the top display line and bit seven on the
// seventh display line.  A call to fillScreen(0x01) would draw 8 horizontal lines on
// the screen on display lines 0, 8, 16, 24, 32, 48, and 56.  Using zero would clear 
// the screen and 0xff would turn on all pixels.
void SSD1306Display::fillScreen(uint8_t fillByte) {
    for (uint8_t row = 0; row < NUM_ROWS; row++) {
        setPosition(row, 0);
        ssd1306DataBegin();
        for (uint8_t col = 0; col < NUM_COLUMNS; col++) {
            ssd1306DataPutByte(fillByte);
        }
        ssd1306DataEnd();
    }
}


// fillAreaWithByte
//
// Fill a portion of the scrren with a single byte value.  Similar to fillScreen, but allows
// onlt a subset of the screen to be filled.  Using fillAreaWithByte(0, 0, 8, 128, b) is the
// same as fillScreen(b).
//
// Note that the rows and columns arguments specify the size of the filled area, NOT the end
// coordinates of the area.
void SSD1306Display::fillAreaWithByte(uint8_t startRow, uint8_t startColumn, uint8_t rows, uint8_t columns, uint8_t b) {

    for (uint8_t row = startRow; ((row < (startRow + rows)) && (row < NUM_ROWS)); row++) {
        setPosition(row, startColumn);
        ssd1306DataBegin();
        for (uint8_t col = startColumn; ((col < (startColumn + columns)) && (col < NUM_COLUMNS)); col++) {
            ssd1306DataPutByte(b);
        }
        ssd1306DataEnd();
    }
}


// fillAreaWithBytes
//
// Fill a portion of the screen using a mult-byte pattern.  Similar to fillAreaWithByte, but
// multiple bytes are specified and these bytes are written sequentially.  The pattern repeats
// after all of the byes in the pattern are used.  When the end of a line is reached, the
// pattern starts over from its first byte on the next line.
//
// This method can be used to draw vertical lines, using a pattern like { 0xff, 0x00, 0x00, 0x00 }.
// This would draw lines in column 0, 4, 8, 12, adn so on.
// An 8-byte pattern with a single shifted bit, like 0x80, 0x40, 0x20... would draw diagonal lines.
// THis methon could also be used to draw characters that are not in the included fonts or small
// patterns.  For larger patterns, it would be better to use the drawImage method that uses images
// stored in PROGMEM.
void SSD1306Display::fillAreaWithBytes(uint8_t startRow, uint8_t startColumn, uint8_t rows, uint8_t columns, const uint8_t pattern[], uint8_t patternSize) {
    for (uint8_t row = startRow; ((row < (startRow + rows)) && (row < NUM_ROWS)); row++) {
        unsigned ix = 0;
        setPosition(row, startColumn);
        ssd1306DataBegin();
        for (uint8_t col = startColumn; ((col < (startColumn + columns)) && (col < NUM_COLUMNS)); col++) {
            ssd1306DataPutByte(pattern[ix++]);
            if (ix >= patternSize)  ix = 0;
        }
        ssd1306DataEnd();
    }
}


// drawImage
//
// Copy a bitmapped image to the screen.  The image must be stored in PROGMEM.  The imabge is stored as
// a byte array where each byte specifies a row of 8 vertical pixels on the screen.  The LSB of each byte
// is the top pixel on the screen.  
//
// The imageRows and imageColumns specify the size of the image in rows and columns, so a 128x8 pixel image
// would have a size of 128x8.
//
// If the image is too large to fit on the screen, or if the starting row or column would cause it to exceed
// the screen boundaries, the image is clipped to the edges of the screen. 
void SSD1306Display::drawImage(uint8_t startRow, uint8_t startColumn, uint8_t imageRows, uint8_t imageColumns, const uint8_t image[]) {

    for (uint8_t row = startRow; ((row < (startRow + imageRows)) && (row < NUM_ROWS)); row++) {
        // Re-compute index to the start of next line of the image data for each display line.
        // This is needed if clipping.
        unsigned ix = (row - startRow) * imageColumns;  
        setPosition(row, startColumn);
        ssd1306DataBegin();
        for (uint8_t col = startColumn; ((col < (startColumn + imageColumns)) && (col < NUM_COLUMNS)); col++) {
            ssd1306DataPutByte(pgm_read_byte(&image[ix++]));
        }
        ssd1306DataEnd();
    }
}

// Set display contrast to level from 0..255
void SSD1306Display::setContrast(uint8_t level) {
    ssd1306CmdBegin();
    i2cSendByte(CMD_SET_CONTRAST);
    i2cSendByte(level);
    ssd1306CmdEnd();

}

// invertScreen
//
// Set the display to inverted or normal mode.  When true (inverted), display
// pixels are lit when the associated display RAM data bit is OFF.
// Note that this command does not change the contents of Display RAM, just
// how the hardware displays them.
// This is different from the invertData command, which writes inverted data
// on subsequent writes to the RAM.
void SSD1306Display::invertScreen(bool b) {
    ssd1306SendCommand(b ? CMD_INVERT_ON : CMD_INVERT_OFF);
}

// sleep
//
// Set display sleep mode.  When true, the display is blank and in low-power mode.
// When false the display shows the data that has been written to it.  Sleeping
// the display does not erase to current data.
void SSD1306Display::sleep(bool b) {
    ssd1306SendCommand(b ? CMD_DISPLAY_OFF : CMD_DISPLAY_ON);
}


////////////////////////////////////////////////////////////////////////////////
//
// Private methods to manage the I2C communication and 
// format the low-level SSD1306 commands and data
//

// Two bytes are sent at the start of every communication to the display.
// The first byte is the display's address (i2C slave address) with R?W bit and
// the second is a control byte.  The control byte contains the Data/Command (D/C)
// bit at position 0x40.  All other bits of the control byte should be zero.
// D/C = 0 indicates that the following bytes are commands for the display.
// D/C = 1 indicates that the following bytes should be written to display RAM.
enum {
    SSD1306_CTL_COMMAND = 0x00,     // D/C bit = 0
    SSD1306_CTL_DATA    = 0x40      // D/C bit = 1
};


// ssd1306DataBegin
//
// Begin transmitting data to the SSD1306.  This starts I2C communication and sends
// the display's I2C Address followed by a control byte indicating that data follows.
// Any bytes sent with ssd1306DataPutByte after this call will be interpreted by the
// display as data for the Display RAM until ssdDataEnd is called. 
void SSD1306Display::ssd1306DataBegin(void) {
    i2cSendBegin();
    i2cSendByte(SSD1306_ADDR);          // address and R/W bit
    i2cSendByte(SSD1306_CTL_DATA);      // D/C bit = data
}


// ssd1306DataEnd
//
// There is nothing to be sent to indicate the end of a command, this 
// method simply calls i2cSendEnd to stop transmission.  
// This method is provided so that commands can be wrapped in 
// DataBegin/DataEnd instead of the more confusing DataBegin/i2cSendEnd.
void SSD1306Display::ssd1306DataEnd(void) {
    i2cSendEnd();
}


// ssd1306DataPutByte
//
// Sends a single byte of data to the controller to be stored in Display RAM.
// If fDataInverted is true, the byte is inverted, meaning that all ones are
// changed to zeroes and zeroes to ones.
void SSD1306Display::ssd1306DataPutByte(uint8_t b) {
    i2cSendByte(fInvertData ? ~b : b);
}


// ssd1306CmdBegin
//
// Begin transmitting a command to the SSD1306.  This starts I2C communication and sends
// the display's I2C Address followed by a control byte indicating commands follow.
// Any bytes sent with i2cSendByte after this call will be interpreted by the display as
// commands until ssdCmdEnd is called. 
void SSD1306Display::ssd1306CmdBegin(void) {
    i2cSendBegin();
    i2cSendByte(SSD1306_ADDR);          // address and R/W bit
    i2cSendByte(SSD1306_CTL_COMMAND);   // D/C bit = command
}


// ssd1306CmdEnd
//
// There is nothing to be sent to indicate the end of a command, this 
// method simply calls i2cSendEnd to stop transmission.  
// This method is provided so that commands can be wrapped in 
// CmdBegin/CmdEnd instead of the more confusing CmdBegin/i2cSendEnd.
void SSD1306Display::ssd1306CmdEnd(void) {
    i2cSendEnd();
}


// ssd1306SendCommand
//
// Standalone method to send a single command byte to the display controller.
// This can be used to send single byte commands, like CMD_INVERT_ON, with a
// single call.
//
// Multi-byte commands, like CMD_SET_CONTRAST, can be sent with multiple calls
// to this method, but it is more efficient to call CmdBegin, followed by multiple
//  i2CSendByte calls, followed by CmdEnd.  The later method does not send the
// display address and control byte for each command byte.
void SSD1306Display::ssd1306SendCommand(uint8_t b) {
    ssd1306CmdBegin();
    i2cSendByte(b);
    ssd1306CmdEnd();
}


////////////////////////////////////////////////////////////////////////////////
// i2C code
//
// This code bit-bangs the i2C protocol to communicate with the SSD1306 display.
// It is not strictly standards compliant and may not work with other devices.
// In particular, the controller does not listen for an ACK/NACK from the display
// and just blindly sends data.

// i2cSendBegin
//
// Signal the start of data transmission.  
// Start is indicated by pulling SDA low while SCL is high.
// Once a transmission starts, SCL is held low and SDA is free to change with
// no effect while SCL is low.  SCL is only brought high to clock in data bits.
void SSD1306Display::i2cSendBegin(void) {
    SCL_high();     // These two lines should have no effect because SCL and SDA
    SDA_high();     //   are both high when the line is idle
    SDA_low();
    SCL_low();
}

// i2cSendEnd
//
// Signal the end of data transmission.
// End is indicated by bringing SDA high while SCL is high.
// When not in a data transmission, SCL and SDA are high.
void SSD1306Display::i2cSendEnd(void) {
    SCL_low();      // should have no effect because SCL already low during data transmission
    SDA_low();
    SCL_high();
    SDA_high();
}


// i2cSendByte
//
// Transmit a single byte of data.
// A data bit is clocked on the rising edge of SCL.  The data is sent MSB first.
void SSD1306Display::i2cSendByte(uint8_t b) {
    for (uint8_t mask = 0x80; mask; mask >>= 1) {
        if (b & mask) {
            SDA_high();
        } else {
            SDA_low();
        }
        SCL_high();
        SCL_low();
    }
    SDA_high();
    SCL_high();
    SCL_low();
}
