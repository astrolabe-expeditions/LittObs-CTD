/*
 * Module T/S seul pour ajout sur chaine de capteur (typiquement littObs)
 * Fonctionnement : 
 * Mesure T/S/P + date/heure par serie avec mise en veille entre chaque serie. A chaque serie (batch ) de  mesure, la chaine de mesure est enregistrer sur la carte SD.
 *
 * 
 */

// ---------------------   PARAMETRES MODIFIABLE DU PROGRAM    -----------------------------------
// Version et numero de serie
String fichier_config = "/config.txt";   // nom du fichier de configuration
char versoft[] = "1.0";                 // version du code
// --------------------     FIN DES PARAMETRES MODIFIABLES     -----------------------------------


//déclaration des librairies
#include <Wire.h>                       // enable I2C
#include <DS3231.h>                     // Pour horloge RTC
#include "SPI.h"                        // pour connection ecran bus SPI
#include <SD.h>                         // pour carte SD
#include "MS5837.h"                     // pour capteur de pression blue robotics BAR30


// déclaration pour gestion des Led et interrupteur
const int greenled = 25;
const int yellowled = 26;
const int magnet=13;
int etatmagnet =0;

//déclaration pour capteur de pression
MS5837 sensor_bar30;
float  wat_pressure, wat_temp, wat_depth, alt;

// definition pour les carte Atlas
#define ecAddress 100
byte ecCode = 0;                    // Used to hold the I2C response code.
byte ecInChar = 0;                  // Used as a 1 byte buffer to store in bound bytes from the EC Circuit.
char ecData[48];                    // We make a 48 byte character array to hold incoming data from the EC circuit.
int ecDelay = 600;                  // Used to change the delay needed depending on the command sent to the EZO Class EC Circuit. 600 par defaut. It is the correct amount of time for the circuit to complete its instruction.
char *ec;                           // Char pointer used in string parsing.
char *tds;                          // Char pointer used in string parsing.
char *sal;                          // Char pointer used in string parsing.
char *sg;                           // Char pointer used in string parsing.

//RTC definition
DS3231 Clock;
bool Century = false;
bool h12;
bool PM;
String second, minute, hour, date, month, year;
String datenum, timenum, datetime;                   // pour format de date en 1 seule écriture

// déclaration pour la gestion de fichier sur la carte SD et fichier config
String datachain = "";                   // chaine de donnée texte de mesure
const int cspin_SD=5;
String filename_temp, str_index, filetrans;
int ind;                      // index vérification de fichier
//fichier config
String id_logger, user, number_measures, delay_batch, gps_timeOut, fluo_timeOut, fluo_delay_chauffe, mode_screen, bat_capacity ,clef_test;
File confFile;




// definition pour la fonction deepsleep de l'ESP32
#define uS_TO_S_FACTOR 1000000      // Conversion factor for micro seconds to seconds
RTC_DATA_ATTR int bootCount = 0;    // utile pour enregistrer un compteur dans la memoire rtc de l'ULP pour un compteur permettant un suivi entre chaque veille
//RTC_DATA_ATTR int filenumber = 0;      // nom de fichier pour le garder entre l'initialisation (1er boot) et les mesures (tout les autres boots)
int TIME_TO_SLEEP = 15;          // Durée d'endormissement entre 2 cycles complets de mesures (in seconds) par défault 600 (mais rédéfinie par le fichier config)

const int control_pin = 4;



String filename = "/datalog.txt";
int nbrMes = 5;
int led_mode = 1;
int debug_mode = 1;




void setup() {
  // ------------------           SETUP (run loop, and what we need for each loop)    ------------------------------------------
  // ------------------        HERE IS NEEDED THE PARAMETER FOR THE ENTIER PROGRAMM   ------------------------------------------
 
      // Initialize les pin pour les LED 
      pinMode(control_pin, OUTPUT);
      digitalWrite(control_pin, HIGH);
      if (led_mode == 1) {
        pinMode(greenled, OUTPUT);
        pinMode(yellowled, OUTPUT);
        pinMode(magnet, INPUT);
        digitalWrite(greenled, HIGH);
        digitalWrite(yellowled, LOW);
      }
    
      delay(500);                              // delay pour eviter les ecriture trop rapide sur la carte SD en cas de faux contact à l'allumage
    
      // initialise les liasons
      if (debug_mode==1) Serial.begin(115200);                        // communication avec le PC
      Wire.begin();                                // for I2C communication
    
      // test de présence de capteurs ou composant
      test_bar();
      test_sd();
  
  
  if(bootCount == 0){
  // --------------        HERE IS ONLY THE INTRODUCION           -----------------------------------------------------------
  // -------------- run just once at the startup, and ignore at each loop ---------------------------------------------------
  
      //Create and Write the 1st ligne of the CSV file
      String datachain = "";
      datachain += "Date & heure"; datachain += ";"; 
      for(int n=1; n<=nbrMes; n++){
        datachain += "Profondeur(m)"; datachain += ";"; 
        datachain += "Temperature mer (C)"; datachain += ";"; 
        datachain += "Conductivity(ms/cm)"; datachain += ";"; 
        //datachain += "Salinity"; datachain += ";";
      }
    
      // save the datachain on the SD card
      File dataFile = SD.open(filename, FILE_APPEND);       // FILE_APPEND pour esp32, FILE_WRITE pour arduino
      if (dataFile) {                                        // if the file is available, write to it:
        dataFile.println(datachain);
        dataFile.close();
        if (debug_mode==1) Serial.println("Fichier créer avec succes");
        if (debug_mode==1) Serial.print("Filename : "); Serial.println(filename);
      }
      else {                                                 // if the file isn't open, pop up an error:
        if (debug_mode==1) Serial.println("error opening file");
        for (int i=0; i<=5; i++){
            errormessage_sd();
        }
      }

      // Sleeping of sensors and other
      all_sleep();

      bootCount = bootCount+1;   // changement du numéro de compteur pour passer directement dans programm loop apres le reveil
  
  }
  else{
  // ---------------        HERE IS THE MAIN PROGRAMM LOOP         ----------------------------------------------------------
  // ---------------         Programm run at each loop             ----------------------------------------------------------
 
      // on se reveille et démarre un "lot" de mesure
      if (debug_mode==1) Serial.print("------ START Measures ---------"); 
    
      // read Sensors once
      lecture_rtc();
    
      // Creating datachain 
      String datachain = "";
      datachain += datetime; datachain += ";";
      
        //read sensors several times and add values to datachain
        for(int n=1; n<=nbrMes; n++){
          mesure_pressure();
            datachain += wat_depth; datachain += ";"; 
            datachain += wat_temp; datachain += ";";
          mesureEC(); 
            datachain += ecData; datachain += ";"; 
            //datachain += Salinity; //datachain += ";";
        }
        
      if (debug_mode==1) Serial.print("DATACHAIN COMPLETE:");
      Serial.println(datachain);
    
      // enregistrement de la datachain sur la carte SD
      File dataFile = SD.open(filename, FILE_APPEND);
      if (dataFile) {                                        // if the file is available, write to it:
        dataFile.println(datachain);
        dataFile.close();
        if (debug_mode==1) Serial.println("ok : Datachain saved on SD card !");
      }
      else {                                                 // if the file isn't open, pop up an error:
        if (debug_mode==1) Serial.println("error opening file");
        for (int i=0; i<=5; i++){
            errormessage_sd();
        }
      }

      // Sleeping of sensors and other
      all_sleep();
        
  } // End of the loop programm
  
  
  // endormissement esp32
  Serial.println("Going to sleep");
  deep_sleep();
//  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//  esp_deep_sleep_start();

}


void loop() {
 //never use because of the sleeping of the princess esp32
}




/////////////////////////////////////////////////////////////////
//              FONCTIONs de gestions des sensors
/////////////////////////////////////////////////////////////////

void mesure_pressure() {                              // fonction mesure de la pression et température sur le capteur blue robotics

  // Update pressure and temperature readings
  sensor_bar30.read();

  wat_pressure = sensor_bar30.pressure();
  wat_temp = sensor_bar30.temperature();
  wat_depth = sensor_bar30.depth();
  alt = sensor_bar30.altitude();

  if (debug_mode==1) {
    Serial.println("--- Pressure Sensor :");
    Serial.print("Pressure: ");
    Serial.print(wat_pressure);
    Serial.println(" mbar");
  
    Serial.print("Temperature: ");
    Serial.print(wat_temp);
    Serial.println(" deg C");
  
    Serial.print("Depth: ");
    Serial.print(wat_depth);
    Serial.println(" m");
  
    Serial.print("Altitude: ");
    Serial.print(alt);
    Serial.println(" m above mean sea level");
  }
}

// Test/initialization pressure sensor
void test_bar(){
      if(!sensor_bar30.init()) {                                            // Returns true if initialization was successful
        if (debug_mode==1) {
          Serial.println("Init failed!");                                     // We can't continue with the rest of the program unless we can initialize the sensor
          Serial.println("Are SDA/SCL connected correctly?");
          Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
          Serial.println("\n\n\n");
        }
        while(1) {
          if (led_mode==1) errormessage_bar();
        }
    }  
}


void mesureEC() {
  if (debug_mode==1) Serial.println("--- EC Sensor :");
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
      if (debug_mode==1) Serial.println("EC Success");    // Means the command was successful.
      break;                           // Exits the switch case.
    case 2:                            // Decimal 2.
      if (debug_mode==1) Serial.println("EC Failed");     // Means the command has failed.
      break;                           // Exits the switch case.
    case 254:                          // Decimal 254.
      if (debug_mode==1) Serial.println("EC Pending");    // Means the command has not yet been finished calculating.
      break;                           // Exits the switch case.
    case 255:                          // Decimal 255.
      if (debug_mode==1) Serial.println("EC No Data");    // Means there is no further data to send.
      break;                           // Exits the switch case.
  }
  ec = strtok(ecData, ",");
  tds = strtok(NULL, ",");
  sal = strtok(NULL, ",");
  sg = strtok(NULL, ",");    // Let's pars the string at each comma.

  if (debug_mode==1) Serial.print("EC value : "); Serial.println(ecData);

}


void veille_ezos(){                          //mise en veille du capteur aprés utilisation (la fonction "r" permettra de reveiller et lire le capteur).
       //veille EC
        delay(100);
        Wire.beginTransmission(ecAddress);    // Call the circuit by its ID number.  
        Wire.write("Sleep");                  // Transmit the command that was sent through the serial port.
        Wire.endTransmission();               // End the I2C data transmission. 
}


void all_sleep(){
      veille_ezos();
      delay(500);
      digitalWrite(greenled, LOW);
      digitalWrite(yellowled, LOW);
      digitalWrite(control_pin, LOW); 
}



/////////////////////////////////////////////////////////////////
//                          FONCTION d'affichage 
/////////////////////////////////////////////////////////////////

void errormessage_bar(){
  for (int i=0; i<=10; i++){
      digitalWrite(greenled, LOW);
      delay(50);
      digitalWrite(greenled, HIGH);
      delay(50);
  }
}

void errormessage_sd(){
      digitalWrite(greenled, HIGH); delay(400);
      digitalWrite(greenled, LOW); delay(50);
      digitalWrite(greenled, HIGH); delay(50);
      digitalWrite(greenled, LOW); delay(50);
      digitalWrite(greenled, HIGH); delay(50);
      digitalWrite(greenled, LOW); delay(50);
      digitalWrite(greenled, HIGH); delay(50);
      digitalWrite(greenled, LOW); delay (300);
}


/////////////////////////////////////////////////////////////////
//                 FONCTIONS pour le datalogging
/////////////////////////////////////////////////////////////////


void lecture_rtc() {                          // fonction lecture de l'horloge RTC et ecriture dans les variables utilent
  
  int sec = Clock.getSecond();
  if (sec < 10) {
    second = String(0) + String(sec);
  }
  else {
    second = sec;
  }
  int minu = Clock.getMinute();
  if (minu < 10) {
    minute = String(0) + String(minu);
  }
  else {
    minute = minu;
  }
  int heure = Clock.getHour(h12, PM);
  if (heure < 10) {
    hour = String(0) + String(heure);
  }
  else {
    hour = heure;
  }
  int jour = Clock.getDate();
  if (jour < 10) {
    date = String(0) + String(jour);
  }
  else {
    date = jour;
  }
  int mois = Clock.getMonth(Century);
  if (mois < 10) {
    month = String(0) + String(mois);
  }
  else {
    month = mois;
  }
  year = Clock.getYear();
  datenum = ""; datenum += year; datenum += month; datenum += date;
  timenum = ""; timenum += hour; timenum += minute; timenum += second;
  datetime = ""; datetime += date; datetime += "/"; datetime += month; datetime += "/20"; datetime += year; datetime += " "; datetime += hour; datetime += ":"; datetime += minute; datetime += ":"; datetime += second;

  if (debug_mode==1) {
    Serial.println("--- RTC values :");
    Serial.print("Date :"); Serial.println(datenum);
    Serial.print("Time :"); Serial.println(timenum);
    Serial.print("DateTime"); Serial.println(datetime);
  }


}


// to initialize and test SD card
void test_sd(){
  if (debug_mode==1) Serial.print("-- Initializing SD card... :");   
  if (!SD.begin(cspin_SD)) {                      // // see if the card is present and can be initialized, ajouter ici chipSelect ou 5 pour la pin 5 par default
    if (debug_mode==1) Serial.println("Card failed, or not present");
    // don't do anything more:
    if (led_mode==1){
      while(1) errormessage_sd();
    }
  }
  if (debug_mode==1) Serial.println("card initialized.");    
}


void deep_sleep(){
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}


void hibernation(){
  // Must setup the way the ESP32 will wake up first
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_deep_sleep_start();
}



//// lecture of the config file
//void lecture_config(){
//  confFile = SD.open(fichier_config, FILE_READ);
//
//  char phrase[200];
//  byte index = 0;
//  char x=0;
//  String reste = "";
//  int k=0;
//  
//  if (confFile) {
//    
//    while (confFile.available()) {
//      x = confFile.read();
//      if (x!=10){
//        if (x!=13) {
//          phrase[index] = x;
//          index++;
//        } else {
//          if (index != 0) {
//            reste = phrase;
//            reste = reste.substring(0,index);
//            index=0;
//            // suppression des commentaires
//            k = reste.indexOf(";");
//            if (k!=0) {
//              if (k!=-1) {
//                reste = reste.substring(0,k);
//              }
//              // supprime les espaces au début et à la fin
//              reste.trim();
//              //
//              // extrait les valeurs pour les placer dans les variables du programme
//              //
//              if (reste.indexOf('=') >0) {
//                // signe égal trouvé
//                String clef = reste.substring(0,reste.indexOf('='));
//                String valeur = reste.substring(reste.indexOf('=')+1);
//                if (clef == "id_logger") id_logger = valeur;
//                if (clef == "delay_batch") delay_batch = valeur;
//                if (clef == "number_measures") number_measures = valeur;
//                //if (clef == "debug_mode") debug_mode = valeur;                        
//                if (clef == "clef_test") clef_test = valeur;
//              }
//            }
//          }          
//        }
//      }
//     }
//    // close the file:
//    confFile.close();
//  } else {
//    // if the file didn't open, print an error:
//    if (debug_mode==1) Serial.println("error opening "+fichier_config);
//    while(1) errormessage_sd();
//  }
//   
//}
