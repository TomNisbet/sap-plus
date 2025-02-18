#ifndef INCLUDE_LOADER_HW
#define INCLUDE_LOADER_HW

#include "Arduino.h"

/*****************************************************************************/
/*****************************************************************************/
/**
 * LoaderHw class
 *
 * Provides the device-specific interface to read and write registers and RAM.
 */


class LoaderHw {
  public:
    LoaderHw();
    void begin();
    void enable();
    void disable();
    bool isActive() { return mEnabled; }
    void clkPulse();
    void clkPulseWidth(unsigned width);
    void reset();
    void clearAll();
    const char * registerName(int registerNumber);
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t data);
    void transferRegister(uint8_t wReg, uint8_t rReg);
    bool writeData(const byte data[], uint16_t len, uint16_t address);
    byte readByte(uint16_t address);
    void writeControls(uint8_t data);
    bool testHardware();

  protected:
    bool mEnabled;

    bool burnByte(byte value, uint16_t address);
    bool valueTest(uint8_t writeVal, uint8_t readVal, const char * s="", uint8_t addr=0);
    bool testOutputRegister();
    bool testRegister(unsigned reg, bool isRw);
    bool testCounters(uint8_t count);
    bool testMemory(bool highMem);
    bool testAlu();
    bool testAdder(bool sub, uint8_t carry);
    bool testAdderOperation(uint8_t a, uint8_t b, bool sub, uint8_t carry);

    void selectWriteRegister(uint8_t reg);
    void selectReadRegister(uint8_t reg);
    void setDataBusMode(uint8_t mode);
    void setAddress(uint16_t address);
    byte readDataBus();
    void writeDataBus(byte data);
};

#endif  // #define INCLUDE_LOADER_HW
