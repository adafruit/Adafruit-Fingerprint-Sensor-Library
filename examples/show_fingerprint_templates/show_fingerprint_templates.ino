/*************************************************** 
  This is an example sketch for our optical Fingerprint sensor

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

int getFingerprintIDez();

// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(2, 3);


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()  
{
  Serial.begin(9600);
  Serial.println("finger template test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }

  //get the templates for fingers 1 through 10
  for (int finger = 1; finger < 10; finger++)
    uploadFingerpintTemplate(finger);

}

void loop()
{

}

uint8_t uploadFingerpintTemplate(uint16_t id)
{
 uint8_t p = finger.loadModel(id);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("template "); Serial.print(id); Serial.println(" loaded");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    default:
      Serial.print("Unknown error "); Serial.println(p);
      return p;
  }

  // OK success!

  p = finger.getModel();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("template "); Serial.print(id); Serial.println(" transferring");
      break;
   default:
      Serial.print("Unknown error "); Serial.println(p);
      return p;
  }
  
  //Template data seems to be 78 bytes long?  This prints out 5 lines of 16 bytes (so there's 2 extra FF bytes at the end)
  for (int count= 0; count < 5; count++)
  {
    for (int i = 0; i < 16; i++)
    {
      Serial.print("0x");
      Serial.print(mySerial.read(), HEX);
      Serial.print(", ");
    }
    Serial.println();
  }
}


