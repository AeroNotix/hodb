#include "Arduino.h"
#include <SoftwareSerialWithHalfDuplex.h>
#include "hodb.h"

byte COMMAND_BYTE = 0x20;
byte TYPICAL_COMMAND_LENGTH = 0x5;

CommandData Commands[] = {
    {RPM, 0x00, 0x05, 0x02},
    {ECT, 0x01, 0x04, 0x01},
    {VSS, 0x02, 0x04, 0x01},
    {IAT, 0x11, 0x04, 0x01},
    {TPS, 0x14, 0x04, 0x01},
    {TimingAdvance, 0x26, 0x04, 0x01},
    {IACV, 0x28, 0x03, 0x01}
};

Honda3Pin::Honda3Pin(uint8_t k_line_pin, uint8_t odb1_or_odb2) :
    _ecuSerial(k_line_pin, k_line_pin, false, false),
    _ecudata{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
{
    _odb1_or_odb2 = odb1_or_odb2;
}

void Honda3Pin::Init() {
    _ecuSerial.begin(9600);
    int initSequence[] = { 0x68, 0x6a, 0xf5, 0xaf, 0xbf, 0xb3, 0xb2, 0xc1, 0xdb, 0xb3, 0xe9 };
    for (int i : initSequence) {
        _ecuSerial.write(i);
    }
    delay(300);
}

CommandData Honda3Pin::findCommand(Command cmd) {
    for (CommandData c : Commands) {
        if (c.commandType == cmd) {
            return c;
        }
    }
}

byte checksum(CommandData cd) {
    return (0xFF - (COMMAND_BYTE + cd.requestSize + cd.address + cd.responseSize - 0x01));
}

bool checksum_matches(byte data[20]) {
    // this only works for RPM right now.
    return (0xFF - (data[0] + data[1] + data[2] + data[3] - 0x01)) == data[4];
}

bool Honda3Pin::ecuCommand(Command cmd) {

  unsigned long timeOut = millis() + 250;

  memset(_ecudata, 0, sizeof(_ecudata));

  _ecuSerial.listen();
  
  CommandData cd = findCommand(cmd);
  
  _ecuSerial.write(COMMAND_BYTE);
  _ecuSerial.write(cd.requestSize);
  _ecuSerial.write(cd.address);
  _ecuSerial.write(cd.responseSize);
  _ecuSerial.write(checksum(cd));

  int i = 0;
  while (i < cd.responseSize && millis() < timeOut) {
    if (_ecuSerial.available()) {
        _ecudata[i++] = _ecuSerial.read();
    }
  }
  return checksum_matches(_ecudata);
}

int Honda3Pin::basicCommand(Command cmd) {
    if (ecuCommand(cmd)) {
        return _ecudata[2];
    } else {
        return -1;
    }
}

// This gives strange responses
unsigned int Honda3Pin::RPM() {
    if (ecuCommand(Command::RPM)) {
        
        if (_odb1_or_odb2 == 1) {
            Serial.print("Raw data: ");
            for (int i = 0; i < 20; i++) {
                Serial.print(_ecudata[i]);
                Serial.print(" ");
                Serial.println("");
            } 
            return (1875000 / (_ecudata[2] * 256 + _ecudata[3] + 1)) * 4;
        }
        
        if (_odb1_or_odb2 == 2) {
            return (_ecudata[2] * 256) + _ecudata[3];
        }
    }
    return -1;
}

int Honda3Pin::ECT() {
    return basicCommand(Command::ECT);
}

int Honda3Pin::VSS() {
    return basicCommand(Command::VSS);
}

int Honda3Pin::IAT() {
    return basicCommand(Command::IAT);
}

int Honda3Pin::TPS() {
    return basicCommand(Command::TPS);
}

int Honda3Pin::TimingAdvance() {
    return basicCommand(Command::TimingAdvance);
}

int Honda3Pin::IACV() {
    return basicCommand(Command::IACV);
}

bool Honda3Pin::ResetECUErrorCodes() {
    // TODO
    return false;
}
