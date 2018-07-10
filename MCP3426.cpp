#include <Arduino.h>
#include <Wire.h>
#include <MCP342x.h>
#include "Trace.h"
#include "pcb.h"
#include "MCP3426.h"

// 0x68 is the default address for all MCP342x devices
const uint8_t adc_address = 0x68;

MCP3426 g_adc;

MCP3426::MCP3426() : MCP342x(adc_address)
{
}

void MCP3426::setup()
{
  // Reset devices
  generalCallReset();
  delay(1); // MC342x needs 300us to settle, wait 1ms
  
  // Check device present
  Wire.requestFrom(adc_address, (uint8_t)1);
  if (!Wire.available()) 
  {
    Serial.print("No ADC @"); Serial.println(adc_address, HEX);
    //while (1) ;
  } 
  else 
  {
    Serial.print("ADC found @"); Serial.println(adc_address, HEX);
  }
  

}

uint8_t MCP3426::convertAndRead(MCP342x::Channel channel, long &value)
{
  value = 0;
  MCP342x::Config status;
  // Initiate a conversion; convertAndRead() will wait until it can be read
  uint8_t err = MCP342x::convertAndRead(
    channel, 
    MCP342x::continous, // MCP342x::oneShot, 
    MCP342x::resolution16,
    MCP342x::gain1,
    1000000, // time out value in microseconds - 1s
    value, 
    status);
  if(err != 0)
  {
    DEBUG_PRINT("ADC read error: 0x"); DEBUG_PRINTHEX(err);
  } 
  else 
  {
    DEBUG_PRINT("ADC ch"); DEBUG_PRINTDEC(channel); DEBUG_PRINT(": "); DEBUG_PRINTDEC(value); DEBUG_PRINT(" err=0x"); DEBUG_PRINTHEX(err); // DEBUG_PRINT(" status=0x"); DEBUG_PRINTHEX(status); DEBUG_PRINTLN("");

    MCP342x::Channel ch = status.getChannel();
    MCP342x::Resolution res = status.getResolution();
    MCP342x::Gain gain = status.getGain();
    bool bReady = status.isReady();
    
    DEBUG_PRINT(" ch="); DEBUG_PRINTDEC(ch); DEBUG_PRINT(" res="); DEBUG_PRINTDEC(res); DEBUG_PRINT(" gain="); DEBUG_PRINTDEC(gain); DEBUG_PRINT(" bReady="); DEBUG_PRINTDEC(bReady); DEBUG_PRINTLN("");
  }
  return err;
}

double MCP3426::getVolts()
{
  long value;
  uint8_t err = convertAndRead(MCP342x::channel1, value); // read volts from ch1
  if(err != 0)
    return 0l;
  const double dDividerFactor = 1.0; // 0.33;
  // 32767 or 0x7FFF - 2^15 - 1. The maximum signed 16 bit value.
  // 65535 or 0xFFFF - 2^16 - 1. The maximum unsigned 16 bit value.
  return ((value * 2.048)/ (32768.0 * dDividerFactor));
}

double MCP3426::getAmps()
{
  long value;
  uint8_t err = convertAndRead(MCP342x::channel2, value); // read volts from ch1
  if(err != 0)
    return 0l;
  const double dDividerFactor = 1.0; // 0.35;
  // 32767 or 0x7FFF - 2^15 - 1. The maximum signed 16 bit value.
  // 65535 or 0xFFFF - 2^16 - 1. The maximum unsigned 16 bit value.
  return ((value * 2.048)/ (32768.0 * dDividerFactor));
}


