const int bcd_a = 8;
const int bcd_b = 9;
const int bcd_c = 10;
const int bcd_d = 11;

const int digit_pins[] = {A0, A1, A2, A3, A4, A5};
const int N_DIGITS = 6;

const int time_arr[] = {1, 2, 3, 4, 5, 6};

void show_bcd(int num) {
  digitalWrite(bcd_a, (num >> 0) & 1);
  digitalWrite(bcd_b, (num >> 1) & 1);
  digitalWrite(bcd_c, (num >> 2) & 1);
  digitalWrite(bcd_d, (num >> 3) & 1);
}

void setup() {
  pinMode(bcd_a, OUTPUT);
  pinMode(bcd_b, OUTPUT);
  pinMode(bcd_c, OUTPUT);
  pinMode(bcd_d, OUTPUT);

  for (int i = 0; i < N_DIGITS; i++) {
    pinMode(digit_pins[i], OUTPUT);
    digitalWrite(digit_pins[i], HIGH);
  }
}

void loop() {
  for (int i = 0; i < N_DIGITS; i++) {
    show_bcd(time_arr[i]);
    digitalWrite(digit_pins[i], LOW);
    delay(2);
    digitalWrite(digit_pins[i], HIGH);
  }
}
