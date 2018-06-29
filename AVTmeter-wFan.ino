#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include "Trace.h"
#include "PinButton.h"
#include "Fan.h"
#include "Screen.h"
#include "Display.h"
#include "LM35.h"
#include "Led.h"
#include "pcb.h"

/** LM35 temperature sensor is connected to this pin */
LM35 g_lm35(pinLM35);

unsigned short int LM35::g_tempMin;
unsigned short int LM35::g_tempMax;

/** the overheating (builtin) led is on pin 13 */
Led g_led(pinLed);

/** my buton */
MyPinButton g_theButton(pinButton);

bool MyPinButton::onUserInActivity(unsigned long ulNow)
{
  DEBUG_PRINTLN("MyPinButton::onUserInActivity");
}
bool MyPinButton::onKeyAutoRepeat()
{
  DEBUG_PRINTLN("MyPinButton::onKeyAutoRepeat");
}
bool MyPinButton::onKeyDown()
{
  DEBUG_PRINTLN("MyPinButton::onKeyDown");
}
bool MyPinButton::onLongKeyDown()
{
  DEBUG_PRINTLN("MyPinButton::onLongKeyDown");
}
bool MyPinButton::onKeyUp(bool bLong)
{
  DEBUG_PRINTLN("MyPinButton::onKeyUp");
}
bool MyPinButton::onClick()
{
  DEBUG_PRINTLN("MyPinButton::onClick");
  if(g_pActiveScreen != 0)
    g_pActiveScreen->switchToNext();

}
bool MyPinButton::onDoubleClick()
{
  DEBUG_PRINTLN("MyPinButton::onDoubleClick");
}

/**
 * analogRead wrapper with better precision
 */
unsigned int myAnalogRead(short int pin)
{
  // first sample seems to fluctuate a lot. Disregard it
  {
    unsigned int intmp = analogRead(pin);
    //Serial.println(intmp);
    delay(60);
  }
  // according to http://www.atmel.com/dyn/resources/prod_documents/doc8003.pdf
  // 11 bit virtual resolution arduino ADC is 10 bit real resolution
  // for 12 bit resolution 16 samples and >> 4 is needed
  unsigned int reading = 0; // accumulate samples here
  for(int i=0; i<=3; i++)
  {
    unsigned int intmp = analogRead(pin);
    reading = reading + intmp;
    //Serial.println(intmp);
    delay(60);
  }
  reading = reading>>2; // averaged over 4 samples
  /*
  unsigned int reading = analogRead(pin);
  Serial.print("analogRead() => ");
  Serial.println(reading);
  */
  return reading;
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
  Screen::switchToNext();
  delay(2*1000);

  fansSetup1();
  g_theAboutScreen.showVersion();
  delay(2*1000);
  
  fansSetup2();
  g_theAboutScreen.statusMessage("Fan at start PWM...");
  delay(2*1000);
  
  fansSetup3();
  g_theAboutScreen.statusMessage("Fan at max PWM...");
  delay(2*1000);

  fansSetup4();
  g_theAboutScreen.statusMessage("Fans at min PWM...");
  delay(2*1000);
  Screen::switchToNext();
}


void loop()
{
  if(g_pActiveScreen != 0)
    g_pActiveScreen->loop();
/*
  static byte uLoop = 0;
  switch(uLoop % 2)
  {
    case 0:
      loopVolts();
      break;
    case 1:
      loopAmps();
      break;
  }
  uLoop++;
  delay(200);
  */
}


