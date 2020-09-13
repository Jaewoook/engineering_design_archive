#define PIN_LED 13

unsigned int count, toggle;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  Serial.println("Hello, Jaewook!");
  digitalWrite(PIN_LED, toggle);
}

void loop() {
  Serial.println(++count);
  toggle = toggle_state(toggle);
  digitalWrite(PIN_LED, toggle);
  delay(1000);
}

int toggle_state(int toggle) {
  return !toggle;
}
