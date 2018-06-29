//#define NODEBUG 1
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include "Trace.h"
#include "PinButton.h"
#include "Screen.h"
#include "Display.h"
#include "LM35.h"


Screen *g_pActiveScreen = 0;
VAScreen g_theVAScreen;
WTScreen g_theWTScreen;
T1T2Screen g_theT1T2Screen;
AboutScreen g_theAboutScreen;

/** all the existing screens */
Screen *Screen::screens[] = {
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
  static byte uActiveScreen = 100;

  DEBUG_PRINTLN("Screen::switchToNext");
  uActiveScreen++;
  if(uActiveScreen >= (sizeof(screens) / sizeof(screens[0])))
    uActiveScreen = 0;
  if(g_pActiveScreen != 0)
    g_pActiveScreen->onDeActivate();
  g_pActiveScreen = screens[uActiveScreen];
  g_pActiveScreen->onActivate();
  //g_pActiveScreen->update();
}

/** 
 * ping all the screens 
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
 * by default process the button
 */
bool Screen::loop()
{
  unsigned long ulNow = millis();
  if(g_theButton.getAndDispatchKey(ulNow))
    return true;
  Screen::tickAll(ulNow);
  if((g_pActiveScreen != 0) && (!g_theButton.isKeyDown()) && g_pActiveScreen->isTimeToUpdate(ulNow))
  {
    g_pActiveScreen->update(ulNow);
  }
  return false;
}

/**
 * 
 */
bool Screen::onActivate()
{
  m_ulNextUpdate = 0; // update asap
}
/**
 * 
 */
bool Screen::onDeActivate()
{
  g_theDisplay.clear();
}
/**
 * 
 */
void Screen::tick(unsigned long ulNow)
{
  unsigned uUpdatePeriod = (this == g_pActiveScreen) ? 10 : 4000;
  m_ulNextTick = ulNow + uUpdatePeriod;
}
/**
 * 
 */
void Screen::update(unsigned long ulNow)
{
  m_ulNextUpdate = ulNow + 10;
}

/**
 * Volt/Amps screen implementation
 */
bool VAScreen::onActivate()
{
  DEBUG_PRINTLN("VAScreen::onActivate");
  m_ulNextUpdate = 0;
  
  static const char *units[] = {"V", "A"};

  g_theDisplay.setFont(OpenSans20b); // Callibri19); // ZevvPeep8x16); //OpenSans26
  //oled.setFont(lcdnums12x16);   
//  oled.setFont(Callibri15);
// oled.setFont(Arial14);
// oled.setFont(Callibri11_bold);
// oled.setFont(TimesNewRoman13);

  // Increase space between letters.
  g_theDisplay.setLetterSpacing(2);
  
  // Allow two or more pixels after value.
  //col1 = col0 + oled.strWidth("99.9") + 2;
  
  // Line height in rows.
  //rows = oled.fontRows();
  //DEBUG_PRINT("rows = "); DEBUG_PRINTDEC(rows); DEBUG_PRINTLN("");

  // Print units
  uint8_t offset = 1;
  for (uint8_t i = 0; i < 2; i++) 
  {
    g_theDisplay.setCursor(col1 + 1, i*rows + offset);
    g_theDisplay.print(units[i]);
  }
  //delay(3000);
  g_theDisplay.setFont(lcdnums14x24); //lcdnums12x16);
  g_theDisplay.setLetterSpacing(3);
}

void clearValue(uint8_t row) 
{
  g_theDisplay.clear(col0, col1, row, row + rows - 1);
}

void VAScreen::tick(unsigned long ulNow)
{
  unsigned uUpdatePeriod = (this == g_pActiveScreen) ? 10 : 4000;
  m_ulNextTick = ulNow + uUpdatePeriod;
  
  m_volts = 2.0 + sin(0.001 * millis()); // 0.01*random(1, 6000);
  m_amps = 0.01 * random(1, 6000);  
}

void VAScreen::update(unsigned long ulNow)
{
  //DEBUG_PRINTLN("VAScreen::update");
  m_ulNextUpdate = ulNow + 10;

  if(m_volts != m_voltsDisplayed)
  {
    m_voltsDisplayed = m_volts;
    clearValue(0*rows);
    g_theDisplay.print(m_volts, 3);
  }
  if(m_amps != m_ampsDisplayed)
  {
    m_ampsDisplayed = m_amps;
    clearValue(1*rows);
    g_theDisplay.print(m_amps, 3);
  }
}

/**
 * Wats/Temperature screen implementation
 */
bool WTScreen::onActivate()
{
  DEBUG_PRINTLN("WTScreen::onActivate");
  m_ulNextUpdate = 0; // mark the screen as requiring update asap.

  static const char *units[] = {"W", "C"};

  g_theDisplay.setFont(OpenSans20b);
  // Increase space between letters.
  g_theDisplay.setLetterSpacing(2);  
  // Print units
  uint8_t offset = 1;
  for (uint8_t i = 0; i < 2; i++) 
  {
    g_theDisplay.setCursor(col1 + 1, i*rows + offset);
    g_theDisplay.print(units[i]);
  }
  g_theDisplay.setFont(lcdnums14x24); //lcdnums12x16);
  g_theDisplay.setLetterSpacing(3);  
}


void WTScreen::tick(unsigned long ulNow)
{
  unsigned uUpdatePeriod = (this == g_pActiveScreen) ? 1000 : 4000;
  m_ulNextTick = ulNow + uUpdatePeriod;
  
  m_watts = g_theVAScreen.m_volts * g_theVAScreen.m_amps;
  m_temp = g_lm35.read();
  m_temp = random(1, 100);
}

void WTScreen::update(unsigned long ulNow)
{
  //DEBUG_PRINT("WTScreen::update m_watts="); DEBUG_PRINTDEC((unsigned int)m_watts); DEBUG_PRINT(" m_temp="); DEBUG_PRINTDEC((unsigned int)m_temp); DEBUG_PRINTLN("");
  m_ulNextUpdate = ulNow + 20;
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

/**
 * Fan Temperature settings screen implementation
 */
bool T1T2Screen::onActivate()
{
  DEBUG_PRINTLN("T1T2Screen::onActivate");
  m_ulNextUpdate = 0; // mark the screen as requiring update asap.

  static const char *units[] = {"C", "C"};

  g_theDisplay.setFont(OpenSans20b);
  // Increase space between letters.
  g_theDisplay.setLetterSpacing(2);  
  // Print units
  uint8_t offset = 1;
  for (uint8_t i = 0; i < 2; i++) 
  {
    g_theDisplay.setCursor(col1 + 1, i*rows + offset);
    g_theDisplay.print(units[i]);
  }
  g_theDisplay.setFont(lcdnums14x24); //lcdnums12x16);
  g_theDisplay.setLetterSpacing(3);  
}

void T1T2Screen::update(unsigned long ulNow)
{
  m_ulNextUpdate = ulNow + 20;

  clearValue(0*rows);
  g_theDisplay.print(m_watts, 1);
  clearValue(1*rows);
  g_theDisplay.print((unsigned int)m_temp);
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
  //g_theDisplay.setScroll(true);
  g_theDisplay.print("Volt/Amp meter with\ntemperature-controlled\nPWM Fan.");
  //\nVersion 0.1");  
  //g_theDisplay.setCursor(0, 6);
  //g_theDisplay.print("Version 0.1");
  //g_theDisplay.setScroll(false);
  
}

void AboutScreen::showVersion()
{
  DEBUG_PRINTLN("AboutScreen::showVersion");
  //rows = oled.fontRows();
  
  g_theDisplay.clear(0, g_theDisplay.displayWidth() - 1, 2, g_theDisplay.displayRows() - 1);
  
  //g_theDisplay.setFont(Callibri11);
  g_theDisplay.setCursor(30, 3);
  g_theDisplay.print("Version 0.1");  
}

void AboutScreen::statusMessage(const char *msg)
{
  DEBUG_PRINTLN("AboutScreen::status");
  //rows = oled.fontRows();
  
  g_theDisplay.clear(0, g_theDisplay.displayWidth() - 1, 5, g_theDisplay.displayRows() - 1);
  
  //g_theDisplay.setFont(Callibri11);
  g_theDisplay.setCursor(0, 5);
  g_theDisplay.print(msg);  
}

