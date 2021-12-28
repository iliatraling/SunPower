/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo_x;  // create servo object to control a servo
Servo myservo_y;


int valx = 0, valy = 0;    // variable to read the value from the analog pin
boolean flag_return_x;
boolean flag_return_y;

void setup() {
  myservo_x.attach(5);  // attaches the servo on pin 9 to the servo object
  myservo_y.attach(6);
  myservo_x.write(90); // scale it to use it with the servo (value between 0 and 180)
  myservo_y.write(0);
  delay(4000);
}

void loop() {
  Serial.begin(9600);
  // reads the value of the potentiometer (value between 0 and 1023)
 
  if(valx == 180 || valx == 0) flag_return_x = !flag_return_x;
  if(flag_return_x) {
  valx++;
  } else {
    valx--;
  }
  if(valy == 90 || valy == 0) flag_return_y = !flag_return_y;
  if(flag_return_y) {
  valy++;
  } else {
    valy--;
  }
  myservo_x.write(valx); // scale it to use it with the servo (value between 0 and 180)
  myservo_y.write(valy);
  Serial.println(valx);
  delay(100);                           // waits for the servo to get there
}
