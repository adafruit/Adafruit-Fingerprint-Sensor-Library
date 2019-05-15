/*************************************************** 
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to 
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// On Leonardo/Micro or others with hardware serial, use those! #0 is green wire, #1 is white
// uncomment this line:
// #define mySerial Serial1

// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// comment these two lines if using hardware serial

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void sensorControl()
{
  if (finger.verifyPassword()) 
    {
      lcdShow("Fingerprint", 2, 0, "Sensor Found!", 1, 1, 750);
    } 
    
    else 
    {
      lcdShow("Fingerprint", 2, 0, "Sensor Not Found!", 0, 1, 750);
        
      while (1) 
      { 
        delay(1); 
      }
      
    }
}

uint8_t lcdShow (String text1, uint8_t column1, uint8_t line1, String text2, uint8_t column2, uint8_t line2, unsigned int standbyTime)
{
  lcd.clear();
  
  lcd.setCursor(column1, line1);
  lcd.print(text1);
  
  lcd.setCursor(column2, line2);
  lcd.print(text2);
  
  delay(standbyTime);
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      lcdShow("Image", 5, 0, "Taken", 5, 1, 750);
      break;
    case FINGERPRINT_NOFINGER:
      lcdShow("...", 6, 0, "...", 6, 1, 250);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcdShow("Communication", 1, 0, "Error!", 5, 1, 750);
      break;
    case FINGERPRINT_IMAGEFAIL:
      lcdShow("Imaging", 4, 0, "Error!", 5, 1, 750);
      break;
    default:
      lcdShow("Unknown", 4, 0, "Error!", 5, 1, 750);
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      lcdShow("Image", 5, 0, "Converted", 3, 1, 750);
      break;
    case FINGERPRINT_IMAGEMESS:
      lcdShow("Image", 5, 0, "Too Messy", 3, 1, 750);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcdShow("Communication", 1, 0, "Error!", 5, 1, 750);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcdShow("Fingerprint", 2, 0, "Feature NotFound", 0, 1, 750);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcdShow("Fingerprint", 2, 0, "Feature NotFound", 0, 1, 750);
      return p;
    default:
      lcdShow("Unknown", 4, 0, "Error!", 5, 1, 750);
      return p;
  }
  
  lcdShow("Remove", 5, 0, "Finger", 5, 1, 750);
  
  p = 0;
  
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  
  lcdShow("ID", 7, 0, String(id)+ " Formed", 3, 1, 750);
  
  p = -1;
  
  lcdShow("Place Same", 3, 0, "Finger Again", 2, 1, 750);
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      lcdShow("Image", 5, 0, "Taken", 5, 1, 750);
      break;
    case FINGERPRINT_NOFINGER:
      lcdShow("...", 6, 0, "...", 6, 1, 250);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcdShow("Communication", 1, 0, "Error!", 5, 1, 750);
      break;
    case FINGERPRINT_IMAGEFAIL:
      lcdShow("Imaging", 4, 0, "Error!", 5, 1, 750);
      break;
    default:
      lcdShow("Unknown", 4, 0, "Error!", 5, 1, 750);
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      lcdShow("Image", 5, 0, "Converted", 3, 1, 750);
      break;
    case FINGERPRINT_IMAGEMESS:
      lcdShow("Image", 5, 0, "Too Messy", 3, 1, 750);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcdShow("Communication", 1, 0, "Error!", 5, 1, 750);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcdShow("Fingerprint", 2, 0, "Feature NotFound", 0, 1, 750);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcdShow("Fingerprint", 2, 0, "Feature NotFound", 0, 1, 750);
      return p;
    default:
      lcdShow("Unknown", 4, 0, "Error!", 5, 1, 750);
      return p;
  }
  
  // OK converted!
  
  lcdShow("Creating Model", 1, 0, "For #" + String(id), 4, 1, 750);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcdShow("Prints", 5, 0, "Matched!", 4, 1, 750);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcdShow("Communication", 1, 0, "Error!", 5, 1, 750);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcdShow("Fingerprints", 2, 0, "Did Not Match", 1, 1, 750);
    return p;
  } else {
    lcdShow("Unknown", 4, 0, "Error!", 5, 1, 750);
    return p;
  }   
  
  lcdShow("ID", 7, 0, String(id)+ " Formed", 3, 1, 750);
  
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcdShow("Stode!", 4, 0, "", 0, 1, 750);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcdShow("Communication", 1, 0, "Error!", 5, 1, 750);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcdShow("Could Not Store", 0, 0, "In that Location", 0, 1, 750);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcdShow("Error Writing", 1, 0, "To Flash", 4, 1, 750);
    return p;
  } else {
    lcdShow("Unknown", 4, 0, "Error!", 5, 1, 750);
    return p;
  }   

void setup()  
{
  Serial.begin(9600);
  finger.begin(57600);
  while (!Serial);
  delay(100);
  sensorControl();
  lcdShow("Fingerprint", 3, 0, "Enrollmen", 3, 1, 750);

}

void loop()                     // run over and over again
{
  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  
  id = readnumber();
  if (id == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial.print("Enrolling ID #");
  Serial.println(id);
  
  while (!  getFingerprintEnroll() );
}
