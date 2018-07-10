//#define NODEBUG 1
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <MCP342x.h>
#include "Trace.h"
#include "Pin2Button.h"
#include "Screen.h"
#include "Display.h"
#include "MCP3426.h"
#include "LM35.h"
#include "Fan.h"
#include "Led.h"

Screen *g_pActiveScreen = 0;
VAScreen g_theVAScreen;
WTScreen g_theWTScreen;
T1T2Screen g_theT1T2Screen;
AboutScreen g_theAboutScreen;

/** all the existing screens to switch through */
Screen *Screen::screens[] = 
{
  &g_theAboutScreen,
  &g_theVAScreen,
  &g_theWTScreen,
  &g_theT1T2Screen
};

/** 
 * flip to the next screen while calling the appropriate callbacks 
 */
void Screen::switchToNext()
{
  static byte uActiveScreen = 100; // index into Screen::screens

  DEBUG_PRINTLN("Screen::switchToNext");
  uActiveScreen++;
  if(uActiveScreen >= (sizeof(screens) / sizeof(screens[0])))
    uActiveScreen = 0;
  if(g_pActiveScreen != 0)
    g_pActiveScreen->onDeActivate();
  g_pActiveScreen = screens[uActiveScreen];
  g_pActiveScreen->onActivate();
  g_pActiveScreen->update(millis());
}

/** 
 * ping all the screens.
 * This correspond to letting M in MVC make the next step.
 */
void Screen::tickAll(unsigned long ulNow)
{
  for(byte uScreen = 0; uScreen < (sizeof(screens) / sizeof(screens[0])); uScreen++)
  {
    Screen *p = screens[uScreen];
    if(p->isTimeToTick(ulNow))
      p->tick(ulNow);
  }
}

uint8_t col0 = 0;  // First value column
uint8_t col1 = 100;  // Last value column.
uint8_t rows = 4;      // Rows per line

/**
 * Default screen event processing loop
 */
bool Screen::loop()
{
  unsigned long ulNow = millis();
  if(g_theButton.getAndDispatchKey(ulNow))
    return true;
  // let the model run if needed
  Screen::tickAll(ulNow);
  if((g_pActiveScreen != 0) && (!g_theButton.isKeyDown()) && g_pActiveScreen->isTimeToUpdate(ulNow))
  {
    // update the view if needed
    g_pActiveScreen->update(ulNow);
  }
  return false;
}

/**
 * 
 */
bool Screen::onActivate()
{
  DEBUG_PRINTLN("Screen::onActivate");
  m_ulNextUpdate = 0; // will need update asap

  g_theDisplay.setFont(OpenSans20b); // Callibri19); // ZevvPeep8x16); //OpenSans26
  // Increase space between letters.
  g_theDisplay.setLetterSpacing(2);
  // Print units
  uint8_t offset = 1;
  for (uint8_t i = 0; i < (sizeof(m_units) /sizeof(m_units[0])); i++)
  {
    g_theDisplay.setCursor(col1 + 1, i*rows + offset);
    if(m_units[i] != 0)
      g_theDisplay.print(m_units[i]);
  }
  g_theDisplay.setFont(lcdnums14x24); //lcdnums12x16);
  g_theDisplay.setLetterSpacing(3);
}
/**
 * 
 */
bool Screen::onDeActivate()
{
  g_theDisplay.clear();
}
/**
 *  Byt default the screen wants to be ticked
 *  every 100ms if it is active
 *  every 4sec if it is not active
 */
void Screen::tick(unsigned long ulNow)
{
  unsigned uUpdatePeriod = (this == g_pActiveScreen) ? 100 : 4000;
  m_ulNextTick = ulNow + uUpdatePeriod;
}
/**
 * By default the screen wants to be redrawn every 100ms
 */
void Screen::update(unsigned long ulNow)
{
  m_ulNextUpdate = ulNow + 100;
}
/**
 * Default key click handler - flip to the next screen
 */
bool Screen::onClick()
{
  switchToNext();
  return true;
}


/**
 * Volt/Amps screen implementation
 */
VAScreen::VAScreen() : Screen()
{
  m_units[0] = "V";
  m_units[1] = "A";
}

void clearValue(uint8_t row) 
{
  g_theDisplay.clear(col0, col1, row, row + rows - 1);
}

/**
 * Measure voltage and amperage here
 */
void VAScreen::tick(unsigned long ulNow)
{
  Screen::tick(ulNow);
  m_volts = g_adc.getVolts();
  m_amps = g_adc.getAmps();
}

/**
 * Display voltage and amperage measured
 */
void VAScreen::update(unsigned long ulNow)
{
  //DEBUG_PRINTLN("VAScreen::update");
  Screen::update(ulNow);

  if(m_volts != m_voltsDisplayed)
  {
    m_voltsDisplayed = m_volts;
    clearValue(0*rows);
    g_theDisplay.print(m_volts, (m_volts < 10.0) ? 3 : 2);
  }
  if(m_amps != m_ampsDisplayed)
  {
    m_ampsDisplayed = m_amps;
    clearValue(1*rows);
    g_theDisplay.print(m_amps, (m_amps < 10.0) ? 3 : 2);
  }
}

bool VAScreen::onActivate()
{
  m_ampsDisplayed = m_voltsDisplayed = 0;
  Screen::onActivate();
}

/**
 * Wats/Temperature screen implementation
 */
WTScreen::WTScreen() : Screen()
{
  m_units[0] = "W";
  m_units[1] = "C";
}
/**
 * Measure Wattage and temperature here, also run fans according to the temperature measured.
 */
void WTScreen::tick(unsigned long ulNow)
{
  Screen::tick(ulNow);

  m_watts = g_theVAScreen.m_volts * g_theVAScreen.m_amps;
  m_temp = g_lm35.read(); // (byte)(50.0 + 40.0 * sin(0.00001 * ulNow)); // random(1, 100);
  // run the fan according to the temperature measured
  onTemperature(m_temp);
}

/**
 * Given this temperature in C (internally or externally measured),
 * set fans PWMs
 */
void WTScreen::onTemperature(byte temp)
{
  DEBUG_PRINT("WTScreen::onTemperature m_temp="); DEBUG_PRINTDEC((unsigned int)m_temp); DEBUG_PRINTLN("");
  if(temp < m_tempStart) 
  {
    fansStop();
    g_led.off();
  }  
  else if(temp < m_tempFull)
  {
    unsigned pwmFan = map(temp, m_tempStart, m_tempFull, Fan::pwmMin, Fan::pwmMax);
    unsigned pwmNow = fansGetPWM();
    if(pwmNow == 0) 
    {
      if(pwmFan < Fan::pwmStart) 
        pwmFan = Fan::pwmStart;
    }
    else
    {
      // instead of just spinning at target pwm, lets get there gradually.
      pwmFan = (pwmFan + pwmNow)/2;
    }
    fansSpin(pwmFan);
    g_led.off();
  }
  else
  {
    fansSpin(Fan::pwmMax);
    g_led.on();
  }    
}

/**
 * Display wattage and temerature
 */
void WTScreen::update(unsigned long ulNow)
{
  //DEBUG_PRINT("WTScreen::update m_watts="); DEBUG_PRINTDEC((unsigned int)m_watts); DEBUG_PRINT(" m_temp="); DEBUG_PRINTDEC((unsigned int)m_temp); DEBUG_PRINTLN("");
  Screen::update(ulNow);
  if(m_watts != m_wattsDisplayed)
  {
    m_wattsDisplayed = m_watts;
    clearValue(0*rows);
    g_theDisplay.print(m_watts, 1);
  }
  if(m_temp != m_tempDisplayed)
  {
    m_tempDisplayed = m_temp;
    clearValue(1*rows);
    g_theDisplay.print((unsigned int)m_temp);
  }
}

bool WTScreen::onActivate()
{
  m_wattsDisplayed = m_tempDisplayed = 0;
  Screen::onActivate();
}

/**
 * Fan Temperature settings screen implementation
 */
T1T2Screen::T1T2Screen() : Screen()
{
  m_units[0] = "C";
  m_units[1] = "C";
}

void T1T2Screen::update(unsigned long ulNow)
{
  m_ulNextUpdate = (unsigned long)-1; //ulNow + 20; no need to update it ever!

  clearValue(0*rows);
  g_theDisplay.print(g_theWTScreen.m_tempStart);
  clearValue(1*rows);
  g_theDisplay.print(g_theWTScreen.m_tempFull);
}

/**
 * AboutScreen implementation
 */
bool AboutScreen::onActivate()
{
  DEBUG_PRINTLN("AboutScreen::onActivate");
  g_theDisplay.setFont(Callibri11_bold);
  g_theDisplay.setCursor(50, 0);
  g_theDisplay.print("About");
  g_theDisplay.setCursor(0, 2);
  g_theDisplay.setFont(Callibri11);
  g_theDisplay.print("Volt/Amp meter with\ntemperature-controlled\npwm fan.");
  m_bVersionDisplayed = false;
}

void AboutScreen::showVersion()
{
  DEBUG_PRINTLN("AboutScreen::showVersion");  
  g_theDisplay.clear(0, g_theDisplay.displayWidth() - 1, 2, g_theDisplay.displayRows() - 1);
  g_theDisplay.setCursor(30, 3);
  g_theDisplay.print("Version 0.1");
  m_bVersionDisplayed = true;
}

void AboutScreen::statusMessage(const char *msg)
{
  DEBUG_PRINT("AboutScreen::status="); DEBUG_PRNTLN(msg);
  //g_theDisplay.setFont(Callibri11);  
  g_theDisplay.clear(0, g_theDisplay.displayWidth() - 1, 6, g_theDisplay.displayRows() - 1);
  g_theDisplay.setCursor(0, 6);
  g_theDisplay.print(msg);  
}
/**
 * Default key click handler
 */
bool AboutScreen::onClick()
{
  if(m_bVersionDisplayed)
    switchToNext();
  else
    showVersion();
  return true;
}


