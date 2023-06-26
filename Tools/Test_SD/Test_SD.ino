/*
Permet de tester la présence de carte SD et de son module
*/

#include "SPI.h"          
#include <SD.h>  

const int cspin_SD=5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  test_sd();

}

void loop() {
  // put your main code here, to run repeatedly:

}


void test_sd(){
  Serial.print("-- Initializing SD card... :");   
  if (!SD.begin(cspin_SD)) {                      // // see if the card is present and can be initialized, ajouter ici chipSelect ou 5 pour la pin 5 par default
  Serial.println("Card failed, or not present");
  // don't do anything more:
  while(1);
  }
  Serial.println("card initialized.");    
}