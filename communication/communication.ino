#include <SoftwareSerialWithHalfDuplex.h>

SoftwareSerialWithHalfDuplex dlcSerial(12, 12, false, false);

void setup() {
  Serial.begin(9600);
  while (!Serial) { };
  dlcSerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void dlcInit() {
  int initSequence[] = { 0x68, 0x6a, 0xf5, 0xaf, 0xbf, 0xb3, 0xb2, 0xc1, 0xdb, 0xb3, 0xe9 };
  for (int i = 0; i < sizeof(initSequence) / sizeof(initSequence[0]); i++) {
    dlcSerial.write(initSequence[i]);
  }
  delay(300);
}

int dlcCommand(byte cmd, byte num, byte loc, byte len, byte data[]) {
  // checksum FF - (cmd + num + loc + len - 0x01)
  byte crc = (0xFF - (cmd + num + loc + len - 0x01));

  unsigned long timeOut = millis() + 250;
  memset(data, 0, sizeof(data));

  dlcSerial.listen();

  dlcSerial.write(cmd);  // header/cmd read memory ??
  dlcSerial.write(num);  // num of bytes to send
  dlcSerial.write(loc);  // address
  dlcSerial.write(len);  // num of bytes to read
  dlcSerial.write(crc);  // checksum

  int i = 0;
  while (i < (len + 3) && millis() < timeOut) {
    if (dlcSerial.available()) {
      data[i] = dlcSerial.read();
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

void serialPrintRPM(int rpm) {
  Serial.print("RPM: ");
  Serial.print(rpm);
  Serial.println("");
}

void loop() {

  byte dlcdata[20] = {0};
  int rpm = 0;

  if (dlcCommand(0x20, 0x05, 0x00, 0x02, dlcdata)) {
    // apparently the format is different between ODB1 and ODB2. For
    // now, print both.
    Serial.print("ODB2 ");
    serialPrintRPM(dlcdata[2] * 256 + dlcdata[3]);

    Serial.print("ODB2 test ");
    serialPrintRPM(dlcdata[2] * 256 + dlcdata[3]);

    Serial.print("ODB2 test2 ");
    serialPrintRPM(dlcdata[0] * 256 + dlcdata[2]);

    Serial.print("ODB1 ");
    serialPrintRPM((1875000 / (dlcdata[2] * 256 + dlcdata[3] + 1)) * 4);

    Serial.print("Raw data: ");
    for (int i = 0; i < 20; i++) {
      Serial.print(i);
      Serial.print(" ");
    }

  }
  // Just a sanity-check to know if we are still running:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
}
