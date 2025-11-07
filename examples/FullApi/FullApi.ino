// FullApi example: exercise all provided ArduinoCompat APIs
#include <Arduino.h>

const int LED_PIN = 13;

void setup() {
  // Serial
  Serial.begin(115200);
  Serial.println("FullApi sketch starting...");

  // pinMode
  pinMode(LED_PIN, OUTPUT);

  // Use byte type
  byte b = 0xA5;
  Serial.print("Byte value: ");
  Serial.println(b);

  // Millis/Delay
  uint32_t t0 = millis();
  Serial.print("millis at setup: ");
  Serial.println(t0);
}

void loop() {
  static bool state = false;

  // Toggle digital output
  state = !state;
  digitalWrite(LED_PIN, state ? 1 : 0);
  Serial.print("LED state: ");
  Serial.println(state ? "ON" : "OFF");

  // Read back digital
  int d = digitalRead(LED_PIN);
  Serial.print("digitalRead(" ); Serial.print(LED_PIN); Serial.print(") = "); Serial.println(d);

  // Analog read/write (stubs on native)
  int a = analogRead(0);
  Serial.print("analogRead(0) = "); Serial.println(a);
  analogWrite(0, 128);

  // Demonstrate millis and delay
  Serial.print("millis: "); Serial.println(millis());
  delay(500);

  // microsecond delay
  Serial.println("delayMicroseconds(100)");
  delayMicroseconds(100);

  // Serial write (binary)
  const char buf[] = "\nRAW\n";
  Serial.write(buf, sizeof(buf)-1);

  // Check available/read (will be no-op in native stub)
  if (Serial.available() > 0) {
    int r = Serial.read();
    Serial.print("Read byte: "); Serial.println(r);
  }
}
