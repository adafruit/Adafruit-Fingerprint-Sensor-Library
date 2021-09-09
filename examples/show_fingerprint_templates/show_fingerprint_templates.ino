/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int getFingerprintIDez();

void setup()
{
  while (!Serial);
  Serial.begin(9600);
  Serial.println("Fingerprint template extractor");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }

  // Try to get the templates for fingers 1 through 10
  for (int finger = 1; finger < 10; finger++) {
    downloadFingerprintTemplate(finger);
  }
}

uint8_t downloadFingerprintTemplate(uint16_t id)
{
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #"); Serial.println(id);
  uint8_t p = finger.loadModel(id);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template "); Serial.print(id); Serial.println(" loaded");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    default:
      Serial.print("Unknown error "); Serial.println(p);
      return p;
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
      return p;
  }

  /*
     The data comes back as a standard packet
     2 bytes: 0xef01
     4 bytes: address
     1 byte: 0x02 (packet ID)
     2 bytes: N (data length)
     N bytes: actual data
     2 bytes:  checksum
  */


  const int buff_size = 267;
  const int header_size = 9;

  uint8_t templateBuffer[buff_size];
  memset(templateBuffer, 0xff, buff_size);  //zero out template buffer
  int index = 0;

  // attempt to read in header
  uint32_t starttime = millis();
  while ((index < header_size) && ((millis() - starttime) < 1000))
  {
    if (mySerial.available())
    {
      templateBuffer[index] = mySerial.read();
      index++;
    }
  }

  Serial.print(index); Serial.println(" bytes read");

  if ( index != header_size ) {
    Serial.println("bad header");
    return 1;
  }

  uint16_t start = templateBuffer[0] << 8 + templateBuffer[1];
  uint32_t addr = templateBuffer[2] << 24 + templateBuffer[3] << 16 + templateBuffer[4] << 8 + templateBuffer[5];
  uint8_t p_id = templateBuffer[6];
  uint16_t count = templateBuffer[7] << 8 + templateBuffer[8];

  Serial.print( "start: 0x"); Serial.println(start, HEX);
  Serial.print( "addr: 0x"); Serial.println(addr, HEX);
  Serial.print( "p_id: 0x"); Serial.println(p_id, HEX);
  Serial.print( "count: 0x"); Serial.println(count, HEX);

  if ( start != 0xef01 ) { Serial.println("Bad start"); return 1; };
  if ( p_id != 0x02 ) { Serial.println("Bad packet ID"); return 1; };
  if ( count > 256) { Serial.println("Count too high"); return 1; };

  count += 2;   // read 2 byte checksum at end of data
  
  // attempt to read in data
  starttime = millis();
  while (count && ((millis() - starttime) < 10000))
  {
    if (mySerial.available())
    {
      templateBuffer[index] = mySerial.read();
      index++;
      count--;
    }
  }

  if ( count ) { Serial.println("timeout reading data"); return 1; }
  
  //dump entire templateBuffer.  This prints out 16 lines of 16 bytes
  for (int count = 0; count < 16; count++)
  {
    for (int i = 0; i < 16; i++)
    {
      Serial.print("0x");
      Serial.print(templateBuffer[count * 16 + i+ header_size], HEX);
      Serial.print(", ");
    }
    Serial.println();
  }
}



void printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}

void loop()
{}

