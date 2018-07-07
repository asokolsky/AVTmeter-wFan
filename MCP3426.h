#pragma once

/**
 * MCP3426 ADC based on MCP342x
 */

class MCP3426 : public MCP342x
{
public: 
  MCP3426();
  uint8_t convertAndRead(MCP342x::Channel channel, long &value);

  double getVolts();
  double getAmps();

  void setup();
};

extern MCP3426 g_adc;

