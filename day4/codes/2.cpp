const int btn_pause_pin = 2;
const int btn_select_pin = 3;
const int btn_inc_pin = 4;
const int btn_dec_pin = 5;

const int N_BTNS = 4;
const int btn_pins[N_BTNS] = {btn_pause_pin, btn_select_pin, btn_inc_pin, btn_dec_pin};

int last_btn_state[N_BTNS];
unsigned long last_debounce_t[N_BTNS];
const unsigned long DEBOUNCE_MS = 50;

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < N_BTNS; i++) {
    pinMode(btn_pins[i], INPUT_PULLUP);
    last_btn_state[i] = HIGH;
    last_debounce_t[i] = 0;
  }
  
  Serial.println("Ready");
}

void loop() {
    for (int i = 0; i < N_BTNS; i++) {
        int reading = digitalRead(btn_pins[i]);

        if (reading == LOW && last_btn_state[i] == HIGH) {
            if (millis() - last_debounce_t[i] > DEBOUNCE_MS) {
                switch (i) {
                    case 0: Serial.println("Pause"); break;
                    case 1: Serial.println("Select"); break;
                    case 2: Serial.println("Increment"); break;
                    case 3: Serial.println("Decrement"); break;
                }
                last_debounce_t[i] = millis();
            }
        }
        last_btn_state[i] = reading;
    }
}
