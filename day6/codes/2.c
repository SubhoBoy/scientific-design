#include <avr/io.h>
#include <util/delay.h>

#define ROW_START 2
#define N_ROWS 5
#define COL_START 0
#define N_COLS 5
#define LED PB5

void blink(int count) {
    for (int i = 0; i < count; i++) {
        PORTB |= (1 << LED);
        _delay_ms(150);
        PORTB &= ~(1 << LED);
        _delay_ms(150);
    }
    _delay_ms(500);
}

int main(void) {
    DDRB |= (1 << LED);
    DDRD &= ~(((1 << N_ROWS) - 1) << ROW_START);
    PORTD |= (((1 << N_ROWS) - 1) << ROW_START);
    DDRC |= (((1 << N_COLS) - 1) << COL_START);
    PORTC |= (((1 << N_COLS) - 1) << COL_START);

    while (1) {
        int key = -1;

        for (int c = 0; c < N_COLS; c++) {
            PORTC &= ~(1 << (c + COL_START));

            for (int r = 0; r < N_ROWS; r++) {
                if (!(PIND & (1 << (r + ROW_START)))) {
                    key = (r * N_COLS) + c + 1;
                    _delay_ms(20);
                    while(!(PIND & (1 << (r + ROW_START))));
                    _delay_ms(20);
                    goto found;
                }
            }
            PORTC |= (1 << (c + COL_START));
        }

    found:
        if (key != -1) {
            blink(key);
        }
    }
    return 0;
}
