const int bcd_a = 8;
const int bcd_b = 9;
const int bcd_c = 10;
const int bcd_d = 11;

int count = 0;

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
}

void loop() {
  show_bcd(count);
  delay(500);
  count++;
  if (count > 9) {
    count = 0;
  }
}
