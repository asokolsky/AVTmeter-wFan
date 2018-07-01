# Volt/Amp Meter with temperature-controlled PWM fan.

Small panel Volt/Amp meter.  Also measures (external) temerature and drives PWM fan accordingly:

- below Temp1, fan rests;
- when the temperature reaches Temp1, fan starts spinning at startPWM
- when the temperature reaches Temp2 the fan spins at 100%.

## User Manual

Upon power-up the meter spins up fans.  First at start-up then at full then at minimum speed.
Press and hold the button to freeze the display.
Use the button click to switch between the screens.  The following screens are available:

- Volt/Amp screen displays voltage and apmperage measured. 
- Power/Temperature screen displays power as calculated from Volts and Amps measured.  Temperature displayed is that from (external) LM35 sensor.
- Temperature Start/Full screen displays temperatures for fan to start / blow full speed.  For temperatures in between the speed of the fan is calculated proportionally.
- About screen

## Hardware

Arduino Pro Mini (5V 16MHz) is at the core.
Internal ADC is used to measure LM35 temperature sensor output, while using internal 1.1V Vref.
ACS723 current sensor converts current to voltage 0-5V.
MCP3426 16bit DAC is used to measure voltage and current sensor output.
Display is anything I2C or SPI, the software assumes SD1306 or SH1106.
ULN2003 darlingtons drive fan(s).

[Schematics and PCB] (https://easyeda.com/asokolsky/AmpVolt-Meter-with-Fan-Control)

## Software

### Dependencies

Thanks to Bill Greiman for a wonderful https://github.com/greiman/SSD1306Ascii
It wins over U8G2 (or rather U8x8) by providing font support and small memory footprint.

FastWrite - it is unclear to me how dramatic the benefits are, yet I decided to try it.

MCP3426 support is provided by:
https://github.com/uChip/MCP342X
https://github.com/stevemarple/MCP342x
https://github.com/kerrydwong/MCP342X


## TODO

### Done

- GUI prototyping with measurements being mostly simulated
- Fan driver

### TODO

- connect amp sensor
- connect ADC

## Similar Projects

http://www.djerickson.com/arduino/ardpm.html

