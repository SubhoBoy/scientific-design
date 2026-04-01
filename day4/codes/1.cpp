const int bcd_pins[] = {8, 9, 10, 11};
const int digit_pins[] = {A0, A1, A2, A3, A4, A5};
const int N_DIGITS = 6;

volatile int deci_s = 0;
int secs = 0;
int mins = 0;
int hrs = 0;
int time_arr[N_DIGITS] = {0, 0, 0, 0, 0, 0};

void time_to_arr() {
  time_arr[5] = hrs / 10;
  time_arr[4] = hrs % 10;
  time_arr[3] = mins / 10;
  time_arr[2] = mins % 10;
  time_arr[1] = secs / 10;
  time_arr[0] = secs % 10;
}

void time_update() {
  if (deci_s >= 10) {
    deci_s = 0;
    secs++;
    if (secs >= 60) {
      secs = 0;
      mins++;
      if (mins >= 60) {
        mins = 0;
        hrs++;
        if (hrs >= 24) {
          hrs = 0;
        }
      }
    }
    time_to_arr();
  }
}

void show_bcd(int num) {
  for(int i=0; i<4; i++) {
    digitalWrite(bcd_pins[i], (num >> i) & 1);
  }
}

void mux_update() {
  for (int i = 0; i < N_DIGITS; i++) {
    show_bcd(time_arr[i]);
    digitalWrite(digit_pins[i], LOW);
    delay(2);
    digitalWrite(digit_pins[i], HIGH);
  }
}

void setup() {
  for (int i=0; i<4; i++) pinMode(bcd_pins[i], OUTPUT);
  for (int i=0; i<N_DIGITS; i++) {
    pinMode(digit_pins[i], OUTPUT);
    digitalWrite(digit_pins[i], HIGH);
  }
  
  cli();
  TCCR1A = 0; TCCR1B = 0;
  OCR1A = 6249;
  TCCR1B |= (1 << WGM12) | (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

ISR(TIMER1_COMPA_vect) {
  deci_s++;
}

void loop() {
  time_update();
  mux_update();
}
