volatile int deci_s = 0;
const int led_pin = 13;

void setup() {
  pinMode(led_pin, OUTPUT);

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 6249;
  TCCR1B |= (1 << WGM12) | (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

ISR(TIMER1_COMPA_vect) {
  deci_s++;
  digitalWrite(led_pin, !digitalRead(led_pin));
}

void loop() {
}
