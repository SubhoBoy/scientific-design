const int seg_a = 2;
const int seg_b = 3;
const int seg_c = 4;
const int seg_d = 5;
const int seg_e = 6;
const int seg_f = 7;
const int seg_g = 8;

const int segs[] = {seg_a, seg_b, seg_c, seg_d, seg_e, seg_f, seg_g};

const bool eight[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW};
const bool three[] = {LOW, LOW, LOW, LOW, HIGH, HIGH, LOW};

void show_digit(const bool segments[]) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segs[i], segments[i]);
  }
}

void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(segs[i], OUTPUT);
  }

  show_digit(eight);
  delay(2000);
}

void loop() {
  show_digit(three);
}
