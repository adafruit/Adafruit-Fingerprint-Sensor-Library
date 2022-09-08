/*******************************************************************************************
  Author: Md. Asifuzzaman Khan (HAALniner21) <https://github.com/AsifKhan991>
  Date:9/3/2022

  This example shows how to write template data to the sensor 
  and enroll aginst an ID. 
  
  The teplate data is stored initially in the microcontroller 
  saved within the microcontroller (can be fetched from SD card or cloud)
  
  To work with this example first get the template data from "get_direct_template.ino" example. Then store it in 
  the 'fingerTemplate' array buffer in this code. Then the template data will 
  be written to the sensor and be enrolled against an ID. 

  Happy coding! :)
 *******************************************************************************************/
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial swser(6, 7); // RX, TX

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&swser, 0);

void setup() {
  Serial.begin(115200);
  finger.begin(57600); //set your sensor's baudrate
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  write_template_data_to_sensor();
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void write_template_data_to_sensor() {
  int template_buf_size=512; //usually hobby grade sensors have 512 byte template data, watch datasheet to know the info
  
  uint8_t fingerTemplate[512]; //this is where you need to store your template data 
  /*
  you can manually save the data got from "get_template.ino" example like this

  uint8_t fingerTemplate[512]={0x03,0x0E,....your template data.....};
  
  */
  memset(fingerTemplate, 0xff, 512); //comment this line if you've manually put data to the line above
  
  Serial.println("Ready to write template to sensor...");
  Serial.println("Enter the id to enroll against, i.e id (1 to 127)");
  int id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
    return;
  }
  Serial.print("Writing template against ID #"); Serial.println(id);

  if (finger.write_template_to_sensor(template_buf_size,fingerTemplate)) { //telling the sensor to download the template data to it's char buffer from upper computer (this microcontroller's "fingerTemplate" buffer)
    Serial.println("now writing to sensor...");
  } else {
    Serial.println("writing to sensor failed");
    return;
  }

  Serial.print("ID "); Serial.println(id);
  if (finger.storeModel(id) == FINGERPRINT_OK) { //saving the template against the ID you entered or manually set
    Serial.print("Successfully stored against ID#");Serial.println(id);
  } else {
    Serial.println("Storing error");
    return ;
  }
}

void loop() {}
