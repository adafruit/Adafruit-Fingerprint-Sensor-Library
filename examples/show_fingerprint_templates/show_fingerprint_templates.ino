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

#define TEMPLATE_SIZE 512

// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
    while (!Serial)
        ;
    Serial.begin(9600);
    Serial.println("finger template test");

    // set the data rate for the sensor serial port
    finger.begin(57600);

    if (finger.verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    } else {
        Serial.println("Did not find fingerprint sensor :(");
        while (1)
            ;
    }

    // get the templates for fingers 1 through 10
    for (int finger = 1; finger < 10; finger++)
        uploadFingerpintTemplate(finger);
}

void loop() {}

uint8_t uploadFingerpintTemplate(uint16_t id) {
    uint8_t p = finger.loadModel(id);
    switch (p) {
        case FINGERPRINT_OK:
            Serial.print("Template ");
            Serial.print(id);
            Serial.println(" loaded");
            break;
        case FINGERPRINT_DBRANGEFAIL:
            Serial.print("Template ");
            Serial.print(id);
            Serial.println(" empty");
            return p;
        default:
            Serial.print("Error ");
            printHex(p, 2);
            return p;
    }

    // OK success!
    p = finger.getModel();
    switch (p) {
        case FINGERPRINT_OK:
            Serial.print("template ");
            Serial.print(id);
            Serial.println(" transferring");
            break;
        default:
            Serial.print("Error ");
            printHex(p, 2);
            return p;
    }

    uint8_t templateBuffer[TEMPLATE_SIZE];  // 512 byte buffer for template
    memset(templateBuffer, 0xff, 256);      // zero out template buffer

    unsigned int headerSize = 9;  // 2byte (Baotou), 4byte (module addr), 1byte (packet id), 2byte (packet length)
    byte headerBuffer[headerSize];

    unsigned int index = 0;
    unsigned long starttime = millis();
    byte headerType = FINGERPRINT_DATAPACKET;

    while (headerType == FINGERPRINT_DATAPACKET) {
        // read packet header
        unsigned int headerIndex = 0;
        while (headerIndex < headerSize) {
            if (((millis() - starttime) > DEFAULTTIMEOUT)) {
                return FINGERPRINT_TIMEOUT;
            }
            if (mySerial.available()) {
                headerBuffer[headerIndex] = mySerial.read();
                headerIndex++;
            }
        }

        //  check packet header (baotou)
        if (((unsigned int)headerBuffer[0] << 8) + headerBuffer[1] != FINGERPRINT_STARTCODE) {
            return FINGERPRINT_BADPACKET;
        }

        // packet type (data or end packet)
        headerType = headerBuffer[6];

        // check packet type
        if (headerBuffer[6] == FINGERPRINT_DATAPACKET || headerBuffer[6] == FINGERPRINT_ENDDATAPACKET) {
            // read packet payload
            unsigned int remaining = (((unsigned int)headerBuffer[7] << 8) + headerBuffer[8]);
            while (remaining > 0) {
                if (((millis() - starttime) > DEFAULTTIMEOUT)) {
                    return FINGERPRINT_TIMEOUT;
                }
                if (mySerial.available()) {
                    if (remaining > 2) {
                        templateBuffer[index] = mySerial.read();
                        index++;
                    } else {
                        // TODO check last 2 bytes (checksum)
                        mySerial.read();
                    }
                    remaining--;
                }
            }
        } else {
            Serial.print("read error (bad packet type)=");
            Serial.println(headerBuffer[6]);
            return FINGERPRINT_BADPACKET;
        }
    }

    Serial.print("template ");
    Serial.print(id);
    Serial.print(" content (");
    Serial.print(index);
    Serial.println(" bytes)");

    // dump entire templateBuffer.  This prints out 32 lines of 16 bytes
    for (int count = 0; count < 32; count++) {
        for (int i = 0; i < 16; i++) {
            printHex(templateBuffer[count * 16 + i], 2);
            Serial.print(" ");
        }
        Serial.println();
    }
}

void printHex(int num, int precision) {
    char tmp[16];
    char format[128];

    sprintf(format, "0x%%.%dX", precision);
    sprintf(tmp, format, num);
    Serial.print(tmp);
}
