const int bcd_pins[] = {8, 9, 10, 11};
const int digit_pins[] = {A0, A1, A2, A3, A4, A5};
const int btn_pins[] = {2, 3, 4, 5};
const int N_DIGITS = 6;

enum Mode { RUNNING, EDITING };
Mode mode = RUNNING;
int cursor = 0;

volatile int deci_s = 0;
int time_arr[N_DIGITS] = {1, 2, 3, 4, 5, 6};
unsigned long last_blink_t = 0;
bool blink_on = true;

int last_btn_state[4];
unsigned long last_debounce_t[4] = {0};
const unsigned long DEBOUNCE_MS = 50;

void time_update() {
  int hrs = time_arr[0] * 10 + time_arr[1];
  int mins = time_arr[2] * 10 + time_arr[3];
  int secs = time_arr[4] * 10 + time_arr[5];
  
  if (deci_s >= 10) {
    deci_s = 0; secs++;
    if (secs >= 60) { secs = 0; mins++; }
    if (mins >= 60) { mins = 0; hrs++; }
    if (hrs >= 24) { hrs = 0; }
  }
  
  time_arr[0] = hrs / 10; time_arr[1] = hrs % 10;
  time_arr[2] = mins / 10; time_arr[3] = mins % 10;
  time_arr[4] = secs / 10; time_arr[5] = secs % 10;
}

void show_bcd(int n) { for(int i=0; i<4; i++) digitalWrite(bcd_pins[i], (n >> i) & 1); }

void mux_update() {
  if (mode == EDITING && millis() - last_blink_t > 500) {
    blink_on = !blink_on;
    last_blink_t = millis();
  }
  for (int i = 0; i < N_DIGITS; i++) {
    if (mode == EDITING && i == cursor && !blink_on) {
      digitalWrite(digit_pins[i], HIGH);
      continue;
    }
    show_bcd(time_arr[i]);
    digitalWrite(digit_pins[i], LOW);
    delay(2);
    digitalWrite(digit_pins[i], HIGH);
  }
}

void btn_update() {
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(btn_pins[i]);
    if (reading == LOW && last_btn_state[i] == HIGH && (millis() - last_debounce_t[i] > DEBOUNCE_MS)) {
      last_debounce_t[i] = millis();
      if (i == 0) {
        mode = (mode == RUNNING) ? EDITING : RUNNING;
        if (mode == RUNNING) blink_on = true;
        else cursor = 0;
      } else if (mode == EDITING) {
        switch(i) {
          case 1: cursor = (cursor + 1) % N_DIGITS; break;
          case 2: time_arr[cursor]++; break; // Simplified, no bounds check
          case 3: time_arr[cursor]--; break; // Simplified, no bounds check
        }
      }
    }
    last_btn_state[i] = reading;
  }
}

void setup() {
  for(int i=0; i<4; i++) pinMode(bcd_pins[i], OUTPUT);
  for(int i=0; i<N_DIGITS; i++) { pinMode(digit_pins[i], OUTPUT); digitalWrite(digit_pins[i], HIGH); }
  for(int i=0; i<4; i++) { pinMode(btn_pins[i], INPUT_PULLUP); last_btn_state[i] = HIGH; }
  
  cli(); TCCR1A=0; TCCR1B=0; OCR1A=6249; TCCR1B|=(1<<WGM12)|(1<<CS12); TIMSK1|=(1<<OCIE1A); sei();
}

ISR(TIMER1_COMPA_vect) { if(mode == RUNNING) deci_s++; }

void loop() {
  if (mode == RUNNING) {
    time_update();
  }
  btn_update();
  mux_update();
}
