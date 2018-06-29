#pragma once

/**
 * Screen is an application entity that can be displayed on the OLED.
 * It is a View part of the MVC pattern.
 */
class Screen
{
public:
  Screen()
  {

  }

  static void switchToNext();
  static void tickAll(unsigned long ulNow);
  /**
   * called from loop on an active screen
   */
  virtual bool loop();
  /**
   * called from loop on every screen
   * This is where M in MVC is updated
   */
  virtual void tick(unsigned long ulNow);
  /** 
   * call backs.  derive a class and overwrite those 
   */
  virtual bool onActivate();
  virtual bool onDeActivate();
  /**
   * called from loop to update active screen
   * This is where V in MVC is updated
   */
  virtual void update(unsigned long int ulNow);

  bool isTimeToUpdate(unsigned long int ulNow)
  {
    return (ulNow > m_ulNextUpdate);
  }
  bool isTimeToTick(unsigned long int ulNow)
  {
    return (ulNow > m_ulNextTick);
  }

protected:
  /** when next view update should happen */
  unsigned long int m_ulNextUpdate = 0;
  /** when next tick should happen */
  unsigned long int m_ulNextTick = 0;

private:
  static Screen *screens[];
};
extern Screen *g_pActiveScreen;

/**
 * This screen shows Volts and Amps
 */
class VAScreen : public Screen
{
public:
  VAScreen() : Screen()
  {

  }

  bool onActivate();
  void update(unsigned long ulNow);
  void tick(unsigned long ulNow);

  double m_volts = 0;
  double m_voltsDisplayed = 0;
  double m_amps = 0;
  double m_ampsDisplayed = 0;
};
extern VAScreen g_theVAScreen;

/**
 * This screen shows Watts and Temperature
 */
class WTScreen : public Screen
{
public:  
  WTScreen() : Screen()
  {

  }
  bool onActivate();
  void update(unsigned long ulNow);
  void tick(unsigned long ulNow);

  double m_watts;
  double m_wattsDisplayed;
  /** temperature in C 0..100 */
  byte m_temp;
  byte m_tempDisplayed;
};
extern WTScreen g_theWTScreen;

/**
 * This screen shows Temp1 and Temp2 fan settings
 */
class T1T2Screen : public Screen
{
public:
  T1T2Screen() : Screen()
  {
    m_ulNextTick = (unsigned long)-1;
  }
  bool onActivate();
  void update(unsigned long ulNow);
  //void tick(unsigned long ulNow);
};
extern T1T2Screen g_theT1T2Screen;

/**
 * This screen shows About info.
 */
class AboutScreen : public Screen
{
public:
  AboutScreen() : Screen()
  {
    m_ulNextTick = (unsigned long)-1;
  }
  bool onActivate();
  //void update(unsigned long ulNow);
  //void tick(unsigned long ulNow);
  void showVersion();
  void statusMessage(const char *msg);
};
extern AboutScreen g_theAboutScreen;


