#include <Arduino.h>
#include "Trace.h"
#include "LM35.h"
#include "pcb.h"

/**
 * analogRead wrapper with better precision
 */
static unsigned int myAnalogRead(short int pin)
{
  const unsigned uDelay = 6; // 60;
  // first sample seems to fluctuate a lot. Disregard it
  {
    unsigned int intmp = analogRead(pin);
    //Serial.println(intmp);
    delay(uDelay);
  }
  // according to http://www.atmel.com/dyn/resources/prod_documents/doc8003.pdf
  // 11 bit virtual resolution arduino ADC is 10 bit real resolution
  // for 12 bit resolution 16 samples and >> 4 is needed
  unsigned int reading = 0; // accumulate samples here
  for(int i=0; i<4 ; i++)
  {
    unsigned int intmp = analogRead(pin);
    reading = reading + intmp;
    //Serial.println(intmp);
    delay(uDelay);
  }
  reading = reading>>2; // averaged over 4 samples
  /*
  unsigned int reading = analogRead(pin);
  Serial.print("analogRead() => ");
  Serial.println(reading);
  */
  return reading;
}


/** LM35 temperature sensor is connected to this pin */
LM35 g_lm35(pinLM35);

unsigned short int LM35::g_tempMin;
unsigned short int LM35::g_tempMax;


void LM35::setup()
{
  // LM35 is not going to provide more than 1V output and that @100C
  // switch to internal 1.1V reference
  analogReference(INTERNAL);  
  pinMode(m_pin, INPUT);
  g_tempMin = g_tempMax = read();
}

/**  
* get the temperature and convert it to Celsius 
* read analog LM35 sensor, 
* presumes you did analogReference(INTERNAL); - more precise but smaller range
*/
unsigned short int LM35::read()
{
  unsigned int reading = myAnalogRead(m_pin);
  // 110 mV is mapped into 1024 steps.  analogReference(INTERNAL) needed
  float tempC = (float)reading * 110 / 1024;
  unsigned short int temp = (unsigned short)tempC;
  if(temp < g_tempMin)
    g_tempMin = temp;
  else if(temp > g_tempMax)
    g_tempMax = temp;
  return temp;  
}


