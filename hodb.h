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
    byte requestSize;
    byte responseSize;
};


class Honda3Pin {
public:
    Honda3Pin(uint8_t k_line_pin, uint8_t odb1_or_odb2);
    void Init();
    unsigned int RPM();
private:
    bool dlcCommand(Command cmd);
    CommandData findCommand(Command cmd);
    SoftwareSerialWithHalfDuplex _dlcSerial;
    byte _dlcdata[20];
    int _odb1_or_odb2;
};

#endif
