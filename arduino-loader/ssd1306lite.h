#ifndef SSD1306LITE_H
#define SSD1306LITE_H

#include "font6x8.h"
#include "font8x16.h"


class SSD1306Display {
    enum {
        NUM_ROWS = 8,
        NUM_COLUMNS = 128,

        MAX_TEXT = 21,      // NUM_COLUMNS / 6,
        MAX_TEXT2X = 16     // NUM_COLUMNS / 8
    };

    public:
        SSD1306Display(void);
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
