/* OSO : Code for testing the blue robotic sensors : Fast Temp

This code will just send temparature value every seconds
The 5v power will be set to on
The 12v power will be set to off
Led will be lighting
Buzzer will be cut
*/

// librairies
#include <Wire.h>
#include "TSYS01.h"

// declar the sensors
TSYS01 sensor;

// declare the pins
// const int buzzer = 26;
const int led = 9;     //25
const int start5v = 4;
//const int start12v = 27;


// setup is running just one at startup
void setup() {
  // define pin use for command sensor's power, led and buzzer
//  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);         
  pinMode(start5v, OUTPUT);
//  pinMode(start12v, OUTPUT);
  // configure the pins
//  digitalWrite(buzzer, LOW);        // shutdown buzzer
  digitalWrite(led, HIGH);          // power on led
  digitalWrite(start5v, HIGH);      // power on 5V system
//  digitalWrite(start12v, LOW);      // power off 12v system

  // start serial console for communication between PC and ESP32
  Serial.begin(115200);
  Serial.println("Starting");

  // initialize I2C protocole, and initialize sensors
  Wire.begin();
  sensor.init();
}


// this loop will run continuously
void loop() {
  // read the sensors 
  sensor.read();
  // Print the value in serial 
  Serial.print("Temperature: ");
  Serial.print(sensor.temperature()); 
  Serial.println(" deg C");
  //wait 1s and start again
  delay(1000);
}
