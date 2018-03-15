ESP32 project with lua interpreter and bindings for sensor libraries 
=========

This project has libraries for SunFounder sensors; this libraries have lua bindings and could be used from lua.

List of libs
------
   * bmp180
      - .init() - initialize i2c 
      - .get() - return values; ex. temperature, pressure = bmp180.get()
   * buzzer 
      - .play() - play sound in passive buzzer
   * rgb
      - .set(int,int,int) - saves values and turns on rgb
      - .on() - turns on rgb with saved values
      - .off() - turns off the rgb	