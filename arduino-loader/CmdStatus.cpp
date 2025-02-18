#include "Arduino.h"
#include "CmdStatus.h"

#include "ssd1306lite.h"

SSD1306Display display;

CmdStatus::CmdStatus()
{
    //clear();
}

static const uint8_t diag1[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
static const uint8_t diag2[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
static const char hex[] = "0123456789abcdef";

void CmdStatus::clear()
{
    level = SL_NONE;
    message = "OK";
    for (int ix = 0; ix < MAX_VALUES; ix++)
    {
        values[ix].valType = VT_NONE;
    }

    delay(40);
    display.initialize();
    display.fillScreen(0x00);

    display.fillAreaWithBytes(0, 0, 8, 128, diag1, sizeof(diag1));
    display.fillAreaWithBytes(0, 64, 4, 64, diag2, sizeof(diag2));
    display.fillAreaWithBytes(4, 0, 4, 64, diag2, sizeof(diag2));
    display.text2x(3, 3*8, " SAP-Plus ");
}

void CmdStatus::info(const char * msg)
{
    Serial.println(msg);
/*
    level = SL_INFO;
    message = msg;
    display.fillScreen(0);
    display.text2x(0, 0, msg);
*/
}


void CmdStatus::program(const char * msg, PgmOperation op) {
    Serial.print(F("running "));
    Serial.println(msg);

    display.fillScreen(0);
    display.invertData(true);
    display.fillAreaWithByte(0, 0, 2, 128, 0);
    display.text2x(0, 9*4, (op == OP_LOADING) ? "Loading" : "Running");
    display.invertData(false);
    display.text2x(3, (16-strlen(msg))*4, msg);
}

void CmdStatus::remaining(unsigned secs) {
    char s[5];

    display.text2x(6, 28, "Time:");
    sprintf(s, "%4d", secs > 9999 ? 9999 : secs);
    display.text2x(6, 28+5*8, s);
}


void CmdStatus::test(const char * msg) {
    Serial.print(F("Testing "));
    Serial.print(msg);
    Serial.print(F(": "));

    display.fillScreen(0);
    display.invertData(true);
    display.fillAreaWithByte(0, 0, 2, 128, 0);
    display.text2x(0, 0, "Test ");
    display.text2x(0, 5*8, msg);
    display.invertData(false);
}

void CmdStatus::pass() {
    Serial.println(F("pass"));

    display.text2x(3, 0, "PASS");
}

static void displayHex(uint8_t row, uint8_t charCol, uint8_t val) {
    char buffer[3];

    buffer[0] = hex[(val >> 4) & 0x0f];
    buffer[1] = hex[(val     ) & 0x0f];
    buffer[2] = '\0';
    display.text2x(row, charCol*8, buffer);
}

void CmdStatus::fail(uint8_t exp, uint8_t rd, const char * msg, uint8_t addr) {
    Serial.print(F("failed"));


    display.text2x(3, 0, "FAIL ");
    if (msg == NULL) {
        Serial.print(F("Addr="));
        Serial.print(addr, HEX);
        display.text2x(3, 5, "Addr=");
        displayHex(3, 10, addr);
    } else {
        Serial.print(msg);
        display.text2x(3, 5, msg);
    }

    Serial.print(F(", read="));
    Serial.print(rd, HEX);
    Serial.print(F(", expected="));
    Serial.println(exp, HEX);

    display.text2x(6, 0, "exp=    read=");
    displayHex(6, 4, exp);
    displayHex(6, 13, rd);
}

void CmdStatus::error(const char * msg) {
    Serial.println(msg);
}

void CmdStatus::setValueDec(int index, const char * label, long value)
{
    setLongValue(index, label, value, VT_DEC);
}

void CmdStatus::setValueHex(int index, const char * label, long value)
{
    setLongValue(index, label, value, VT_HEX);
}

void CmdStatus::setLongValue(int index, const char * label, long value, ValueType vt)
{
    if ((index >= 0) && (index < MAX_VALUES))
    {
        values[index].label = label;
        values[index].value = value;
        values[index].valType = vt;
    }
}

bool CmdStatus::isClear()
{
    return level == SL_NONE;
}

void CmdStatus::printStatus()
{
    if (level == SL_NONE)
    {
        Serial.println("OK");
        return;
    }
    else if (level == SL_INFO)
    {
        Serial.print("INFO: ");
    }
    else {
        Serial.print("ERROR: ");
    }
    Serial.print(message);
    for (int ix = 0; ix < MAX_VALUES; ix++)
    {
        if (values[ix].valType != VT_NONE)
        {
            Serial.print("  ");
            Serial.print(values[ix].label);
            Serial.print("=");
            if (values[ix].valType == VT_DEC)
            {
                Serial.print(values[ix].value, DEC);
            }
            else
            {
                Serial.print("0x");
                Serial.print(values[ix].value, HEX);
            }
        }
    }
    Serial.println("");
}
