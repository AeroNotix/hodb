#include "honda_3_pin.h"

Honda3Pin odb_connection(12, 2);

void setup() {
  Serial.begin(9600);
  while (!Serial) { };
  odb_connection.Init();
  pinMode(LED_BUILTIN, OUTPUT);
}

void serialPrintRPM(int rpm) {
  Serial.print("RPM: ");
  Serial.print(rpm);
  Serial.println("");
}

void loop() {
  unsigned int i = odb_connection.RPM();
  serialPrintRPM(i);
  // Just a sanity-check to know if we are still running:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1500);
}
