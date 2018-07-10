# Volt/Amp Meter with temperature-controlled PWM fan.

Small panel Volt/Amp meter.  Also measures (external) temerature and drives PWM fan accordingly:

- below T1, fan rests;
- when the temperature reaches T1, fan starts spinning at startPWM
- when the temperature reaches T2 the fan spins at 100%.

where T1=25C, T2=50C - see class WTScreen in Screen.h
  and pwmStart, pwmMax and pwmMin are defined in Fan.h

## User Manual

Upon power-up the meter spins up fans.  First at start-up speed then at full speed then at minimum speed.
Press and hold the button to freeze the display.
Click the the button to switch between the screens.  The following screens are available:

- Volt/Amp screen displays voltage and amperage measured. 
- Power/Temperature screen displays power as calculated from Volts and Amps measured.  Temperature displayed is that from (external) LM35 sensor.
- Temperature Start/Full screen displays temperatures for fan to start / blow full speed.  For temperatures in between the speed of the fan is calculated proportionally.
- About screen shows application name and version.

## Hardware

Arduino Pro Mini (5V 16MHz) is at the core.
Internal ADC is used to measure LM35 temperature sensor output, while using the internal 1.1V Vref.
ACS723 current sensor converts current to voltage 0-5V.
MCP3426 16bit DAC is used to measure voltage and current sensor output. Its internal Vref=2.048V
Display is anything I2C (SPI is a possibility, but not tested), the software assumes SD1306 or SH1106.
ULN2003 darlingtons drive fan(s).

[Schematics and PCB] (https://easyeda.com/asokolsky/AmpVolt-Meter-with-Fan-Control)

## Software

### Dependencies

Thanks to Bill Greiman for a wonderful https://github.com/greiman/SSD1306Ascii
It wins over U8G2 (or rather U8x8) by providing font support and small memory footprint.

FastWrite - it is unclear to me how dramatic the benefits are, yet I decided to try it.

MCP3426 support is provided by one of:
https://github.com/uChip/MCP342X
https://github.com/stevemarple/MCP342x - testing this one
https://github.com/kerrydwong/MCP342X


## TODO

This is a work in progress.  Status is incomplete.

### Done

- GUI prototyping with measurements being mostly simulated
- Fan driver

### TODO

- connect ADC, read meaningful values, calibrate voltage divider
- connect amp sensor, read meaningful values, calibrate voltage divider.

## Similar Projects

http://www.djerickson.com/arduino/ardpm.html
