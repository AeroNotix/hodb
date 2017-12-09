#ifndef honda_3_pin_h
#define honda_3_pin_h

#include <Arduino.h>
#include <SoftwareSerialWithHalfDuplex.h>

enum Command {
    RPM,
    ECT,
    VSS,
    IAT,
    TPS,
    TimingAdvance,
    IACV,
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
    int ECT();
    int VSS();
    int IAT();
    int TPS();
    int TimingAdvance();
    int IACV();
    bool ResetECUErrorCodes();

private:
    bool ecuCommand(Command cmd);
    CommandData findCommand(Command cmd);
    int basicCommand(Command cmd);
    SoftwareSerialWithHalfDuplex _ecuSerial;
    byte _ecudata[20];
    int _odb1_or_odb2;
};

#endif
