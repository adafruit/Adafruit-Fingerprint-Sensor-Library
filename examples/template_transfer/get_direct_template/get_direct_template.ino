/*******************************************************************************************
  Author: Md. Asifuzzaman Khan (HAALniner21) <https://github.com/AsifKhan991>
  Date:9/3/2022

  This example shows how to save template data to an 512 byte array buffer for further 
  manipulation (i.e: save in SD card or cloud). 
  
  You can load this data on the 'fingerTemplate' buffer of the "write_template_from_SD.ino" 
  example to write it back to the sensor.
 
  Happy coding! :)
 *******************************************************************************************/
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>

SoftwareSerial swser(6, 7); // RX, TX

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&swser, 0);


uint8_t f_buf[512]; //here is where the template data is sotred

void setup() {
  Serial.begin(9600);
  finger.begin(57600); //set your sensor's baudrate
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  store_template_to_buf();
}


void store_template_to_buf(){

  Serial.println("Waiting for valid finger....");
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


  Serial.println("Creating model...");

  if (finger.createModel() == FINGERPRINT_OK) {  //creating the template from the 2 charecter files and saving it to char buffer 1
    Serial.println("Prints matched!");
    Serial.println("Template created");
  } else {
    Serial.println("Template not build");
    return;
  }

  Serial.println("Attempting to get template..."); 
  if (finger.getModel() == FINGERPRINT_OK) {  //requesting sensor to transfer the template data to upper computer (this microcontroller)
    Serial.println("Transferring Template...."); 
  } else {
    Serial.println("Failed to transfer template");
    return;
  }
  
  if (finger.get_template_buffer(512, f_buf) == FINGERPRINT_OK) { //read the template data from sensor and save it to buffer f_buf
    Serial.println("Template data (comma sperated HEX):");
    for (int k = 0; k < (512/finger.packet_len); k++) { //printing out the template data in seperate rows, where row-length = packet_length
      for (int l = 0; l < finger.packet_len; l++) {
        Serial.print("0x");
        Serial.print(f_buf[(k * finger.packet_len) + l], HEX);
        Serial.print(",");
      }
      Serial.println("");
    }
  }

}

void loop() {}
