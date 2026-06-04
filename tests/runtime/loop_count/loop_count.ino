// loop() を複数回まわしてから exit(0) で終了するパターン。
// millis()/delay() が動くことも確認する。

unsigned long t0;
int count = 0;

void setup() {
  Serial.begin(115200);
  t0 = millis();
  Serial.println("start");
}

void loop() {
  Serial.print("loop ");
  Serial.println(count);
  delay(5);
  if (++count >= 3) {
    Serial.print("elapsed_ms_ok=");
    Serial.println(millis() >= t0 ? 1 : 0);
    Serial.flush();
    exit(0);
  }
}
