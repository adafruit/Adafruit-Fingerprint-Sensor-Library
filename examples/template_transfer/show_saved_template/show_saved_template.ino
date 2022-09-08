/*******************************************************************************************
  Author: Md. Asifuzzaman Khan (HAALniner21) <https://github.com/AsifKhan991>
  Date:9/3/2022

  This example shows how to read already saved template data from the sensor 
 
  Happy coding! :)
 *******************************************************************************************/
 
#include <Adafruit_Fingerprint.h>

#include <SoftwareSerial.h>
SoftwareSerial swser(6, 7); // RX, TX

#define mySerial swser

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial, 0);

void setup()
{ Serial.begin(115200);
  finger.begin(57600);
  delay(5000);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }

  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  finger.getTemplateCount();
  Serial.print(F("stored template: ")); Serial.println(finger.templateCount);
  
  show_from_saved(5); //input the id of the template you want to see, in this example it is 5
  
}

void show_from_saved(uint16_t id) {
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #"); Serial.println(id);
  uint8_t p = finger.loadModel(id);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template "); Serial.print(id); Serial.println(" loaded");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return;
    default:
      Serial.print("Unknown error "); Serial.println(p);
      return ;
  }

  // OK success!

  Serial.print("Attempting to get #"); Serial.println(id);
  p = finger.getModel();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template "); Serial.print(id); Serial.println(" transferring:");
      break;
    default:
      Serial.print("Unknown error "); Serial.println(p);
      return;
  }
  
  uint8_t f_buf[512];
  if (finger.get_template_buffer(512, f_buf) == FINGERPRINT_OK) {
    for (int k = 0; k < 4; k++) {
      for (int l = 0; l < 128; l++) {
        Serial.print(f_buf[(k * 128) + l], HEX);
        Serial.print(",");
      }
      Serial.println("");
    }
  }
}


void loop() {}
