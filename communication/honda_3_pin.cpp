#include "Arduino.h"
#include <SoftwareSerialWithHalfDuplex.h>
#include "honda_3_pin.h"


Honda3Pin::Honda3Pin(uint8_t k_line_pin, uint8_t odb1_or_odb2) :
    _dlcSerial(k_line_pin, k_line_pin, false, false),
    _dlcdata{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
{
    _odb1_or_odb2 = odb1_or_odb2;
}

void Honda3Pin::Init() {
    _dlcSerial.begin(9600);
    int initSequence[] = { 0x68, 0x6a, 0xf5, 0xaf, 0xbf, 0xb3, 0xb2, 0xc1, 0xdb, 0xb3, 0xe9 };
    for (unsigned int i = 0; i < sizeof(initSequence) / sizeof(initSequence[0]); i++) {
        _dlcSerial.write(initSequence[i]);
    }
    delay(300);
}

int Honda3Pin::dlcCommand(byte cmd, byte num, byte loc, byte len, byte data[]) {
  // checksum FF - (cmd + num + loc + len - 0x01)
  byte crc = (0xFF - (cmd + num + loc + len - 0x01));

  unsigned long timeOut = millis() + 250;
  memset(data, 0, len);

  _dlcSerial.listen();

  _dlcSerial.write(cmd);  // header/cmd read memory ??
  _dlcSerial.write(num);  // num of bytes to send
  _dlcSerial.write(loc);  // address
  _dlcSerial.write(len);  // num of bytes to read
  _dlcSerial.write(crc);  // checksum

  int i = 0;
  while (i < (len + 3) && millis() < timeOut) {
    if (_dlcSerial.available()) {
      data[i] = _dlcSerial.read();
      i++;
    }
  }
  if (data[0] != 0x00 && data[1] != (len + 3)) {
    return 0;
  }
  if (i < (len + 3)) {
    return 0;
  }
  return 1;
}

unsigned int Honda3Pin::readRPM() {
    if (dlcCommand(0x20, 0x05, 0x00, 0x02, _dlcdata) != -1) {

        if (_odb1_or_odb2 == 1) {
            return (1875000 / (_dlcdata[2] * 256 + _dlcdata[3] + 1)) * 4;
        }
        
        if (_odb1_or_odb2 == 2) {
            return (_dlcdata[2] * 256) + _dlcdata[3];
        }
    }
    return -1;
}

