/* OSO : Code for testing EC sensors (Atlas Scientific)

This code will just send temparature value every 3 seconds
The 5v power will be set to on
The 12v power will be set to off
Led will be lighting
Buzzer will be cut
*/

// librairies
#include <Wire.h>

// definition pour la carte Atlas EC
#define ecAddress 100
byte ecCode = 0;                    // Used to hold the I2C response code.
byte ecInChar = 0;                  // Used as a 1 byte buffer to store in bound bytes from the EC Circuit.
char ecData[48];                    // We make a 48 byte character array to hold incoming data from the EC circuit.
int ecDelay = 600;                  // Used to change the delay needed depending on the command sent to the EZO Class EC Circuit. 600 par defaut. It is the correct amount of time for the circuit to complete its instruction.
char *ec;                           // Char pointer used in string parsing.
char *tds;                          // Char pointer used in string parsing.
char *sal;                          // Char pointer used in string parsing.
char *sg;                           // Char pointer used in string parsing.

// déclaration initiale des variables de la fonction cal_sal (Aminot A., Kérouel R. (2004), Hydrologie des écosystèmes marins. Paramètres et analyses. Cf pages 74-78)
int Rp = 1;
float aa[] = {0.0080, -0.1692, 25.3851, 14.0941, -7.0261, 2.7081};
float bb[] = {0.0005, -0.0056, -0.0066, -0.0375, 0.0636, -0.0144};
float cc[] = {0.6766097, 0.0200564, 0.0001104259, -6.9698E-07, 1.0031E-09};
float k = 0.0162;
float rt, R_t, Salinity, modA, modB;


// declare the pins
const int led = 25;
const int start5v = 4;



// setup is running just one at startup
void setup() {
  // define pin use for command sensor's power, led and buzzer
  pinMode(led, OUTPUT);         
  pinMode(start5v, OUTPUT);
  // configure the pins
  digitalWrite(led, HIGH);          // power on led
  digitalWrite(start5v, HIGH);      // power on 5V system

  // start serial console for communication between PC and ESP32
  Serial.begin(115200);
  Serial.println("Starting");

  // initialize I2C protocole, and initialize sensors
  Wire.begin();
}


// this loop will run continuously
void loop() {
  // read the sensors 
  mesureEC();

  //wait 1s and start again
  delay(3000);
}




void mesureEC() {
  Serial.println("--- EC Sensor :");
  Wire.beginTransmission(ecAddress);   // Call the circuit by its ID number.
  Wire.write('r');                     // r for reading sensor
  Wire.endTransmission();              // End the I2C data transmission.
  delay(ecDelay);                      // Reading time needing, 600 by default
  Wire.requestFrom(ecAddress, 48, 1);  // Call the circuit and request 48 bytes (this is more than we need)
  ecCode = Wire.read();                // The first byte is the response code, we read this separately.
  byte i = 0;                          // Counter used for EC_data array.
  while (Wire.available()) {           // Are there bytes to receive.
    ecInChar = Wire.read();            // Receive a byte.
    ecData[i] = ecInChar;              // Load this byte into our array.
    i += 1;                            // Incur the counter for the array element.
    if (ecInChar == 0) {               // If we see that we have been sent a null command.
      i = 0;                           // Reset the counter i to 0.
      Wire.endTransmission();          // End the I2C data transmission.
      break;                           // Exit the while loop.
    }
  }
  switch (ecCode) {                    // Switch case based on what the response code is.
    case 1:                            // Decimal 1.
      Serial.println("EC Success");    // Means the command was successful.
      break;                           // Exits the switch case.
    case 2:                            // Decimal 2.
      Serial.println("EC Failed");     // Means the command has failed.
      break;                           // Exits the switch case.
    case 254:                          // Decimal 254.
      Serial.println("EC Pending");    // Means the command has not yet been finished calculating.
      break;                           // Exits the switch case.
    case 255:                          // Decimal 255.
      Serial.println("EC No Data");    // Means there is no further data to send.
      break;                           // Exits the switch case.
  }
  ec = strtok(ecData, ",");
  tds = strtok(NULL, ",");
  sal = strtok(NULL, ",");
  sg = strtok(NULL, ",");    // Let's pars the string at each comma.

  Serial.print("EC value : "); Serial.println(ecData);

}

