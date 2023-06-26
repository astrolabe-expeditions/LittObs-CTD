/* OSO : Code for testing the blue robotic sensors : Bar 30 for pressure

This code will just send pressuree value every seconds
The 5v power will be set to on
The 12v power will be set to off
Led will be lighting
Buzzer will be cut
*/

// librairies
#include <Wire.h>
#include "MS5837.h"

// declare the sensors
MS5837 sensor;

// declare the pins
//const int buzzer = 26;
const int led = 9;        //25
const int start5v = 4;
//const int start12v = 27;



void setup() {   // setup is running just one at startup
  // define pin use for command sensor's power, led and buzzer
  pinMode(led, OUTPUT);         
  pinMode(start5v, OUTPUT);

  // configure the pins
  digitalWrite(led, HIGH);          // power on led
  digitalWrite(start5v, HIGH);      // power on 5V system


  // start serial console for communication between PC and ESP32
  Serial.begin(115200);
  Serial.println("Starting");
  
  // initialize I2C protocole
  Wire.begin();

  // Initialize pressure sensor
  // Returns true if initialization was successful
  // We can't continue with the rest of the program unless we can initialize the sensor
  while (!sensor.init()) {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
    delay(5000);
  }
  
  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(1029); // kg/m^3 (freshwater, 1029 for seawater)
}

void loop() {  // this loop will run continuously
  // Update pressure and temperature readings
  sensor.read();

  Serial.print("Pressure: "); 
  Serial.print(sensor.pressure()); 
  Serial.println(" mbar");
  
  Serial.print("Temperature: "); 
  Serial.print(sensor.temperature()); 
  Serial.println(" deg C");
  
  Serial.print("Depth: "); 
  Serial.print(sensor.depth()); 
  Serial.println(" m");
  
  Serial.print("Altitude: "); 
  Serial.print(sensor.altitude()); 
  Serial.println(" m above mean sea level");

  delay(1000);
}