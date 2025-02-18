/**
 * Command Status class
 *
 * Creates an object that holds an error message and a number of
 * optional numeric values.  Each value has a format (hex,
 * decimal) and a label.  The printStatus call formats all of
 * the data in the command status and prints it out to the
 * serial port.
 */

//#include <avr/pgmspace.h>

class CmdStatus
{
  public:
    enum PgmOperation { OP_LOADING, OP_RUNNING };

    CmdStatus();
    void clear();
    void info(const char * msg);
    void error(const char * msg);
    void setValueDec(int index, const char * label, long value);
    void setValueHex(int index, const char * label, long value);
    void program(const char * msg, PgmOperation op);
    void remaining(unsigned secs);
    void test(const char * msg);
    void pass();
    void fail(uint8_t exp, uint8_t rd, const char * msg="", uint8_t addr=0);

    bool isClear();
    void printStatus();

private:
    enum
    {
        MAX_VALUES = 3
    };
    enum StatusLevel { SL_NONE, SL_INFO, SL_ERROR };
    enum ValueType { VT_NONE, VT_DEC, VT_HEX };

    struct StatusValue
    {
        const char * label;
        ValueType valType;
        long      value;
    };

    StatusLevel level;
    const char * message;
    StatusValue values[MAX_VALUES];

    void setLongValue(int index, const char * label, long value, ValueType vt);
};

