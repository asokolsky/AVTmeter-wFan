#pragma once

/**
 * Arduino wrapper for TI LM35 sensor
 * www.ti.com/lit/ds/symlink/lm35.pdf
 */
class LM35
{
public:
  /** Min observed temperature in C */
  static unsigned short int g_tempMin;
  /** Max observed temperature in C */
  static unsigned short int g_tempMax;

  LM35(short int pin) :
    m_pin(pin)
  {

  }

  void setup();
  /**  
  * get the temperature and convert it to Celsius 
  * read analog LM35 sensor, 
  * presumes you did analogReference(INTERNAL); - more precise but smaller range
  */
  unsigned short int read();

private:
  /** sensor is connected to this pin */
  short int m_pin;
};
extern LM35 g_lm35;

/*class Potentiometer
{
public:
  Potentiometer(short int pin) :
    m_pin(pin)
  {

  }

  void setup()
  {
    pinMode(m_pin, INPUT);
  }

  unsigned int read()
  {
    return myAnalogRead(m_pin);
  }

private:*/
  /** potentiometer is connected to this analog input pin */
  /*short int m_pin;
};
extern Potentiometer g_pot;
*/
