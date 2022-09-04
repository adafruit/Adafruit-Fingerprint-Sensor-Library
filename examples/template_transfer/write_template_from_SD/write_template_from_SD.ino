/*******************************************************************************************
  Author: Md. Asifuzzaman Khan (HAALniner21) <https://github.com/AsifKhan991>
  Date:9/3/2022

  This example shows how to write template data to the sensor from an external 
  SD card using the SD library. 
  First a template is created then it is stored in the SD card as a text file 
  using the "Save_template_to_SD.ino" example.
  That text file can now be written back to the sensor against an ID using this example
   
  Happy coding! :)
 *******************************************************************************************/
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial swser(6, 7); // RX, TX

#include <SPI.h>
#include <SD.h>

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&swser, 0);

#define SD_CS 5  //change according to your chip select connection of SD card

void setup() {
  Serial.begin(115200);
  finger.begin(57600); //set your sensor's baudrate
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
    if(!SD.begin(SD_CS)){ 
        Serial.println("Card Mount Failed");
        while(1);
    }
    write_template_from_SD();
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void write_template_from_SD(){
  Serial.println("Ready to write template");
  Serial.println("enter the name of text file, i.e id (1 to 127)");
  int id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print("Writing stored template against ID #");Serial.println(id);

  uint8_t fingerTemplate[512]; //this buffer is where the template data is stored after being read from SD Card
  memset(fingerTemplate, 0xff, 512);

  File myFile = SD.open("/"+String(id) + ".txt", FILE_READ);
  int i = 0;
  if (myFile) {
    while (myFile.available()) {
      fingerTemplate[i] = myFile.read(); //read templatge file from sd and save to buffer
      i = i + 1;
    }
  } else {
    Serial.println("error opening template file from SD");
  }
  myFile.close();

  if (i != 512) { //checking if 512 bytes of template data was recieved or not
    Serial.println("template data corrupted or incomplete!");
    return;
  }


  if (finger.write_template_to_sensor(fingerTemplate)) { //telling the sensor to download the template data from upper computer (this microcontroller's "fingerTemplate" buffer)
    Serial.println("now saving to library...");
  } else {
    Serial.println("writing to sensor failed");
    return;
  }

  Serial.print("ID "); Serial.println(id);
  if (finger.storeModel(id) == FINGERPRINT_OK) { //saving the template against the ID you entered or manually set
    Serial.println("Stored!");
  } else {
    Serial.println("Storing error");
    return ;
  }
}

void loop() {}
