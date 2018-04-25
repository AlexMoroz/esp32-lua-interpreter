ESP32 project with lua interpreter and bindings for sensor libraries 
=========

This project has libraries for SunFounder sensors; this libraries have lua bindings and could be used from lua.

List of libs
------
   * i2c
      - .init() - initialize i2c
   * bmp180
      - .get() - return values; ex. temperature, pressure = bmp180.get()
   * buzzer 
      - .play() - play sound in passive buzzer
   * rgb
      - .set(int,int,int) - saves values and turns on rgb
      - .on() - turns on rgb with saved values
      - .off() - turns off the rgb	
   * lcd
      - .init() - initialize lcd
      - .set(int,int) - pointer coordinates
      - .write(string) - string to print
   * gas
      - .get() - return digital state value, and analog ppm value; ex.  state, value = gas.get()
   * eddystone
      - .init() - initialize bluetooth beacon settings
      - .start(callback) - start scanning, return found devices in callback funcion as a table
      - .share() - start advertising of a new URL eddytone beacon 
      - .stop() - stop scanning 
      
      
table for eddystone devices contains:
   - type
      
Usage example
---------
```bash
lua> i2c = require "i2c"
lua> lcd = require "lcd"
lua> i2c.init()
lua> lcd.init()
lua> lcd.set(0,0)
lua> lcd.write("hello world")
```