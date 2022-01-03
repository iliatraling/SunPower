# SunPower
 Solar station controller.
SUNPOWER

This device is designed to transmit a control signal to the servo drives for turning the solar battery perpendicular to the sun. This is to get maximum power from the sun. It does not need to be positioned in an open area like a photodiode device, and multiple rotary mechanisms can be controlled.
The first versions of SUNPOWER used photodiodes as rotation sensors, but I ran into a number of problems.
If the weather is clear, then everything is fine, but if even small clouds appear, the installation does not work correctly, the rattling of the servos or movement in one direction or the other. Also, if dirt accidentally gets on the photodiode, the solar station will behave incorrectly.

Operating principle
Arduino Uno receives the time from the DS1302 clock module (a very cheap module is in a hurry for me for 20 seconds per week), calculates the altitude and azimuth of the sun with an error of + -3 degrees by algorithms, checks if the sun is below 7 degrees (you can adjust sunset and sunrise) , then the arduino turns off the power to the servo through the transistor and plunges into a deep sleep for 1 hour. If the sun is higher than 7 degrees, then the Arduino will wake up every 5 minutes, calculate the position of the sun and rotate the solar panel by this angle.
