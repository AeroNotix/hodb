#include "Arduino.h"
#include <SoftwareSerialWithHalfDuplex.h>
#include "honda_3_pin.h"

byte COMMAND_BYTE = 0x20;
byte TYPICAL_COMMAND_LENGTH = 0x5;

CommandData Commands[3] = {
    {RPM, 0x00, 0x02},
    {ECT, 0x01, 0x01},
    {VSS, 0x02, 0x01}
};

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

CommandData findCommand(Command cmd) {
    for (unsigned int i = 0; sizeof(Commands) / sizeof(Commands[0]); i++) {
        if (Commands[i].commandType == cmd) {
            return Commands[i];
        }
    }
}

int Honda3Pin::dlcCommand(Command cmd, byte data[]) {

  unsigned long timeOut = millis() + 250;
  // refactor the length arg to be known from data[]
  memset(data, 0, 20);

  _dlcSerial.listen();
  

  CommandData cd = findCommand(cmd);
  
  // checksum FF - (cmd + num + loc + len - 0x01)
  byte crc = (0xFF - (COMMAND_BYTE + TYPICAL_COMMAND_LENGTH + cd.address + cd.responseSize - 0x01));

  _dlcSerial.write(COMMAND_BYTE);  // header/cmd read memory ??
  _dlcSerial.write(TYPICAL_COMMAND_LENGTH);  // num of bytes to send, change this when necessary
  _dlcSerial.write(cd.address);  // address
  _dlcSerial.write(cd.responseSize);  // num of bytes to read
  _dlcSerial.write(crc);  // checksum

  int i = 0;
  while (i < (TYPICAL_COMMAND_LENGTH + 3) && millis() < timeOut) {
    if (_dlcSerial.available()) {
      data[i] = _dlcSerial.read();
      i++;
    }
  }
  if (data[0] != 0x00 && data[1] != (TYPICAL_COMMAND_LENGTH + 3)) {
    return 0;
  }
  if (i < (TYPICAL_COMMAND_LENGTH + 3)) {
    return 0;
  }
  return 1;
}

unsigned int Honda3Pin::readRPM() {
    if (dlcCommand(RPM, _dlcdata) != -1) {

        if (_odb1_or_odb2 == 1) {
            return (1875000 / (_dlcdata[2] * 256 + _dlcdata[3] + 1)) * 4;
        }
        
        if (_odb1_or_odb2 == 2) {
            return (_dlcdata[2] * 256) + _dlcdata[3];
        }
    }
    return -1;
}

