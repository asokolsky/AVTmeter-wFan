#include <Arduino.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <MCP342x.h>
#include "Trace.h"
#include "Pin2Button.h"
#include "Fan.h"
#include "Screen.h"
#include "Display.h"
#include "MCP3426.h"
#include "LM35.h"
#include "Led.h"
#include "pcb.h"


/** the overheating (builtin) led is on pin 13 */
Led g_led(pinLed);

/** my buton */
MyPinButton g_theButton;

bool MyPinButton::onUserInActivity(unsigned long ulNow)
{
  DEBUG_PRINTLN("MyPinButton::onUserInActivity");
  return false;
}
bool MyPinButton::onKeyAutoRepeat()
{
  DEBUG_PRINTLN("MyPinButton::onKeyAutoRepeat");
  return false;
}
bool MyPinButton::onKeyDown()
{
  DEBUG_PRINTLN("MyPinButton::onKeyDown");
  return false;
}
bool MyPinButton::onLongKeyDown()
{
  DEBUG_PRINTLN("MyPinButton::onLongKeyDown");
  return false;
}
bool MyPinButton::onKeyUp(bool bLong)
{
  DEBUG_PRINTLN("MyPinButton::onKeyUp");
  return false;
}
bool MyPinButton::onClick()
{
  DEBUG_PRINTLN("MyPinButton::onClick");
  if(g_pActiveScreen != 0)
    return g_pActiveScreen->onClick();
  return false;
}
bool MyPinButton::onDoubleClick()
{
  DEBUG_PRINTLN("MyPinButton::onDoubleClick");
  return false;
}


void setup() 
{
  Serial.begin(115200);
  DEBUG_PRINTLN("Volt/Amp/Temp meter with Fan control");
  Wire.begin();
  Wire.setClock(400000L);

  g_lm35.setup();
  g_lm35.read();
  g_led.setup();
     
  g_theDisplay.setup();
  Screen::switchToNext(); // this will show a splash part of the About screen
  delay(2*1000);

  g_theAboutScreen.showVersion();
  
  typedef void (* fooptr)();
  static const fooptr foos[] = 
  {
    fansSetup1,
    fansSetup2,
    fansSetup3,
    fansSetup4
  };
  static const char *msgs[] = 
  {
    "",
    "Fan at start PWM...",
    "Fan at max PWM...",
    "Fan at min PWM..."
  };
  for(int8_t i = 0; i < (sizeof(foos) / sizeof(foos[0])); i++)
  {
    foos[i]();
    g_theAboutScreen.statusMessage(msgs[i]);
    delay(2*1000);
  }
  Screen::switchToNext();
  g_adc.setup();
}


void loop()
{
  if(g_pActiveScreen != 0)
    g_pActiveScreen->loop();
}


