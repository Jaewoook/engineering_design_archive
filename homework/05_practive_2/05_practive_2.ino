#define GND_PIN 7

int state = 0;
int i = 0;

void setup() {
  Serial.begin(115200);
  pinMode(GND_PIN, OUTPUT);
  digitalWrite(GND_PIN, state);
  Serial.print("Start\n");
  delay(1000);

  for (i = 0; i < 10; i++) {
    state = toggle_state(state);
    digitalWrite(GND_PIN, state);
    delay(200);
  }
  digitalWrite(GND_PIN, 1);
}

void loop() {

}

int toggle_state(int state) {
  return !state;
}
