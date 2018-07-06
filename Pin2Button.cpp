#define NODEBUG 1
#include <Arduino.h>
#include <digitalWriteFast.h>
#include "Trace.h"
#include "Pin2Button.h"

#define m_bPin 2

/*static void isrOnKeyChange()
{
  g_theButton.m_bPresses++;
}*/

Pin2Button::Pin2Button()
{
  pinMode(m_bPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(m_bPin), isrOnKeyChange, CHANGE);
}

bool Pin2Button::getAndDispatchKey(const unsigned long ulNow)
{
  // get out if we are still bouncing!
  if(ulNow < m_ulBounceSubsided)
    return false;
  
  bool bRes = false;
  // read the momentary state from hardware
  bool bPressed = (digitalReadFast(m_bPin) == 0);
  if(bPressed == m_bOldPressed) 
  {
    if(!bPressed)
    {
      if(!isUserLongInactive(ulNow))
        return false;
      bRes = onUserInActivity(ulNow);
      onUserActivity(ulNow);
      return bRes;      
    }
    // Button is PRESSED!
    // fire auto repeat logic here
    if((m_ulToFireAutoRepeat == 0) || (ulNow < m_ulToFireAutoRepeat))
    {
      ;
    }
    else
    {
      m_ulToFireAutoRepeat = ulNow + s_iAutoRepeatDelay;
      DEBUG_PRINTLN("onKeyAutoRepeat");
      bRes = onKeyAutoRepeat();
    }      
    // fire long key logic here
    if((m_ulToFireLongKey == 0) || (ulNow < m_ulToFireLongKey))
      return bRes;
    m_ulToFireLongKey = 0;
    DEBUG_PRINTLN("onLongKeyDown");
    return onLongKeyDown() || bRes;
  }
  // bPressed != m_bOldPressed
  if(m_ulBounceSubsided == 0) 
  {
    m_ulBounceSubsided = ulNow + s_iDebounceDelay;
    return false;
  }
  if(bPressed)
  {
    // button was just pressed!
    m_ulToFireLongKey = ulNow + s_iLongKeyDelay;
    m_ulToFireAutoRepeat = ulNow + s_iAutoRepeatDelay;
    m_ulBounceSubsided = 0;
    DEBUG_PRINTLN("onKeyDown");
    bRes = onKeyDown();
    m_ulToClick = ulNow + (unsigned long int)s_iClick;
  }
  else
  {
    // button was released!
    DEBUG_PRINT("onKeyUp ulNow="); DEBUG_PRINTDEC(ulNow); DEBUG_PRINT(" m_ulToClick="); DEBUG_PRINTDEC(m_ulToClick); DEBUG_PRINTLN("");
    bool bLongKey = (m_ulToFireLongKey == 0);
    m_ulToFireAutoRepeat = m_ulToFireLongKey = m_ulBounceSubsided = 0;
    bRes = onKeyUp(bLongKey);
    if(ulNow < m_ulToClick)
    {
      DEBUG_PRINTLN("onClick");
      onClick();
      m_ulToClick = 0;
    }   
    if(ulNow < m_ulToDoubleClick)
    {
      onDoubleClick();
      //m_ulToDoubleClick = 0;
    }
    else
    {
      m_ulToDoubleClick = ulNow + s_iDoubleClick;
    }
  }
  onUserActivity(ulNow);
  m_bOldPressed = bPressed;
  return bRes;
}

