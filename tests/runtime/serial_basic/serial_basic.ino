// Serial 出力の基本: print/println, String, 数値の基数指定, F() マクロ。
// native の loop() は無限ループのため、テスト用にスケッチ側で exit(0) する。

void setup() {
  Serial.begin(115200);

  String name = "world";
  int n = 42;

  Serial.print("hello ");
  Serial.println(name);

  Serial.print("dec=");
  Serial.print(n);
  Serial.print(" hex=");
  Serial.println(n, HEX);

  Serial.print("pi=");
  Serial.println(PI, 3);

  Serial.println(F("flash"));

  Serial.print("map=");
  Serial.println(map(5, 0, 10, 0, 100));

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.print("led=");
  Serial.println(digitalRead(LED_BUILTIN));

  Serial.println("done");
  Serial.flush();
  exit(0);
}

void loop() {}
