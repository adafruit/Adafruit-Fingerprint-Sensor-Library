
#if (ARDUINO >= 100)
 #include "Arduino.h"
 #include <SoftwareSerial.h>
#else
 #include "WProgram.h"
#endif


#define FINGERPRINT_OK 0x00
#define FINGERPRINT_PACKETRECIEVEERR 0x01
#define FINGERPRINT_NOFINGER 0x02
#define FINGERPRINT_IMAGEFAIL 0x03
#define FINGERPRINT_IMAGEMESS 0x06
#define FINGERPRINT_FEATUREFAIL 0x07
#define FINGERPRINT_NOMATCH 0x08
#define FINGERPRINT_NOTFOUND 0x09
#define FINGERPRINT_DBRANGEFAIL 0x0C
#define FINGERPRINT_UPLOADFEATUREFAIL 0x0D
#define FINGERPRINT_PACKETRESPONSEFAIL 0x0E
#define FINGERPRINT_UPLOADFAIL 0x0F
#define FINGERPRINT_DELETEFAIL 0x10
#define FINGERPRINT_DBCLEARFAIL 0x11
#define FINGERPRINT_PASSFAIL 0x13
#define FINGERPRINT_INVALIDIMAGE 0x15
#define FINGERPRINT_FLASHERR 0x18
#define FINGERPRINT_INVALIDREG 0x1A
#define FINGERPRINT_ADDRCODE 0x20
#define FINGERPRINT_PASSVERIFY 0x21

#define FINGERPRINT_STARTCODE 0xEF01

#define FINGERPRINT_COMMANDPACKET 0x1
#define FINGERPRINT_DATAPACKET 0x2
#define FINGERPRINT_ACKPACKET 0x7
#define FINGERPRINT_ENDDATAPACKET 0x8

#define FINGERPRINT_TIMEOUT 0xFF
#define FINGERPRINT_BADPACKET 0xFE

#define FINGERPRINT_GETIMAGE 0x01
#define FINGERPRINT_IMAGE2TZ 0x02
#define FINGERPRINT_VERIFYPASSWORD 0x13
#define FINGERPRINT_HISPEEDSEARCH 0x1B

//#define FINGERPRINT_DEBUG 

#define DEFAULTTIMEOUT 5000  // milliseconds


class Adafruit_Fingerprint {
 public:
  Adafruit_Fingerprint(SoftwareSerial *);
  void begin(uint16_t baud);

  boolean verifyPassword(void);
  uint8_t getImage(void);
  uint8_t image2Tz(void);
  uint8_t fingerFastSearch(void);
  void writePacket(uint32_t addr, uint8_t packettype, uint16_t len, uint8_t *packet);
  uint8_t getReply(uint8_t packet[], uint16_t timeout=DEFAULTTIMEOUT);

  uint16_t fingerID, confidence;

 private: 
  uint32_t thePassword;
  uint32_t theAddress;
  SoftwareSerial *mySerial;
};
