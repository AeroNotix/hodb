#ifndef honda_3_pin_h
#define honda_3_pin_h

#include <Arduino.h>
#include <SoftwareSerialWithHalfDuplex.h>

enum Command {
    RPM,
    ECT,
    VSS
};

struct CommandData {
    Command commandType;
    byte address;
    byte responseSize;
};


class Honda3Pin {
public:
    Honda3Pin(uint8_t k_line_pin, uint8_t odb1_or_odb2);
    void Init();
    unsigned int readRPM();
private:
    int dlcCommand(Command cmd, byte data[]);
    CommandData findCommand(Command cmd);
    SoftwareSerialWithHalfDuplex _dlcSerial;
    byte _dlcdata[20];
    int _odb1_or_odb2;
};

#endif
