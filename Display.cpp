#include <Arduino.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include "Trace.h"
#include "Display.h"

Display g_theDisplay;

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

//SSD1306AsciiWire oled;

/**
 *
 */
void Display::setup()
{
#if RST_PIN >= 0
    begin(&SH1106_128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
    begin(&SH1106_128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
    clear();
}

