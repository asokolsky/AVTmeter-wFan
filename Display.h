#pragma once

/**
 * Display is abstraction of OLED as presented by SSD1306Ascii library
 */
class Display : public SSD1306AsciiWire
{
public:
  Display() : SSD1306AsciiWire()
  {
  }

  void setup();

protected:
};
extern Display g_theDisplay;

