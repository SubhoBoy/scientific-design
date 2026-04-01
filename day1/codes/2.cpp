const int led_pin = 8;
const int btn_pin = 9;

void setup() {
  pinMode(led_pin, OUTPUT);
  pinMode(btn_pin, INPUT_PULLUP);
}

void loop() {
  int state = digitalRead(btn_pin);

  if (state == LOW) {
    digitalWrite(led_pin, HIGH);
  } else {
    digitalWrite(led_pin, LOW);
  }
}
