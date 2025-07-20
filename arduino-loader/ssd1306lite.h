#ifndef SSD1306LITE_H
#define SSD1306LITE_H

#include "font6x8.h"
#include "font8x16.h"

// The slave address of an SSD1306 is seven bits and should be either 0x3c or 0x3d.
// The bit following the seven address bits is the read/write bit and it is always
// set to zero to indicate that the microcontroller is writing to the display.
//
// The address and R/W bit are combined below so that the sendByte code can send the
// adddress and R/W bit as a single byte.
//
// Some displays may already be marked Addr=78 rather than Addr=3C, but the
// code below should always be 0x78 or 0x79 two cover the two possible
// addresses used by the controller.
//
// These addresses may be specified in the SSD1306Display constructor.

#define SSD1306_ADDR1    0x78    // Slave address of the display (0x3c << 1) | 0
#define SSD1306_ADDR2    0x7a    // Slave address of the display (0x3d << 1) | 0


class SSD1306Display {
    enum {
        NUM_ROWS = 8,
        NUM_COLUMNS = 128,

        MAX_TEXT = 21,      // NUM_COLUMNS / 6,
        MAX_TEXT2X = 16     // NUM_COLUMNS / 8
    };

    public:
        SSD1306Display(uint8_t addr = SSD1306_ADDR1);
        void initialize(void);

        void setPosition(uint8_t row, uint8_t column);
        void invertData(bool b);
        void clear(void) { fillScreen(0x00); }

        void text(uint8_t row, uint8_t column, const char * str);
        void text2x(uint8_t row, uint8_t column, const char * str);

        void fillScreen(uint8_t fillByte);
        void fillAreaWithByte(uint8_t startRow, uint8_t startColumn, uint8_t rows, uint8_t columns, uint8_t b);
        void fillAreaWithBytes(uint8_t startRow, uint8_t startColumn, uint8_t rows, uint8_t columns, const uint8_t pattern[], uint8_t patternSize);
        void drawImage(uint8_t startRow, uint8_t startColumn, uint8_t imageRows, uint8_t imageColumns, const uint8_t image[]);

        void setContrast(uint8_t level);
        void invertScreen(bool b);
        void sleep(bool b);

    private:
        bool fInvertData;
        uint8_t i2cAddress;

        void ssd1306DataBegin(void);
        void ssd1306DataPutByte(uint8_t b);
        void ssd1306DataEnd(void);
        void ssd1306CmdBegin(void);
        void ssd1306CmdEnd(void);
        void ssd1306SendCommand(uint8_t b);

        void i2cSendBegin(void);
        void i2cSendEnd(void);
        void i2cSendByte(uint8_t b);
};

#endif
