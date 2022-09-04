/*******************************************************************************************
  Author: Md. Asifuzzaman Khan (HAALniner21) <https://github.com/AsifKhan991>
  Date:9/3/2022

  This example shows how to save template data to an SD card using the 
  SsFat library. (also works with the SD library)
  First a template is created then it is stored in the SD card as a text file.
  That text file can be written back to the sensor using the "write_template_from_SD.ino" example
  
  This example is tested on esp32 & arduino Mega 2560 boards. Other boards are also supposed to work.
   
  Happy coding! :)
 *******************************************************************************************/
#include <SoftwareSerial.h>

SoftwareSerial swser(6, 7); // RX, TX

#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include "SdFat.h"
SdFat SD;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&swser, 0);

#define SD_CS 5  //change according to your chip select connection of SD card

void setup() {
  Serial.begin(9600);
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

  save_template_to_SD();

}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void save_template_to_SD(){
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  int id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);
  Serial.print("Waiting for valid finger to enroll as ID:"); Serial.println(id);
  while (finger.getImage() != FINGERPRINT_OK) { // press down a finger take 1st image 
  }
  Serial.println("Image taken");


  if (finger.image2Tz(1) == FINGERPRINT_OK) { //creating the charecter file for 1st image 
    Serial.println("Image converted");
  } else {
    Serial.println("Conversion error");
    return;
  }

  Serial.println("Remove finger");
  delay(2000);
  uint8_t p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  Serial.println("Place same finger again, waiting....");
  while (finger.getImage() != FINGERPRINT_OK) { // press the same finger again to take 2nd image
  }
  Serial.println("Image taken");


  if (finger.image2Tz(2) == FINGERPRINT_OK) { //creating the charecter file for 2nd image 
    Serial.println("Image converted");
  } else {
    Serial.println("Conversion error");
    return;
  }


  Serial.print("Creating model for #");  Serial.println(id);


  if (finger.createModel() == FINGERPRINT_OK) {  //creating the template from the 2 charecter files and saving it to char buffer 1
    Serial.println("Prints matched!");
    Serial.println("Template created");
  } else {
    Serial.println("Template not build");
    return;
  }

  Serial.print("Attempting to get #"); Serial.println(id);
  if (finger.getModel() == FINGERPRINT_OK) {  //requesting sensor to transfer the template data to upper computer (this microcontroller)
    Serial.print("Template "); Serial.print(id); Serial.println(" transferring:");
  } else {
    Serial.print("Failed to transfer"); Serial.print(id);
    return;
  }
  
  uint8_t f_buf[512]; //here is where the template data is sotred
  
  if (finger.get_template_buffer(512, f_buf) == FINGERPRINT_OK) { //read the template data from sensor and save it to buffer f_buf
    Serial.println("Template data (comma sperated HEX):");
    for (int k = 0; k < (512/finger.packet_len); k++) { //printing out the template data in seperate rows, where row-length = packet_length
      for (int l = 0; l < finger.packet_len; l++) {
        Serial.print(f_buf[(k * finger.packet_len) + l], HEX);
        Serial.print(",");
      }
      Serial.println(".");
    }
  }

  File myFile = SD.open("/"+String(id)+".txt", FILE_WRITE); //naming the saved file as the id number, so creates a id.txt file
    if (myFile) {
      for(int i=0;i<512;i++){
        myFile.write(f_buf[i]); //writing the buffer to sd card
      }
     }else{
      Serial.println("error writing file to sd");
      return;
     }
     myFile.close();
     Serial.println("template saved to sd");
}

void loop() {}
