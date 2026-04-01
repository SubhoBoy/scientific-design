#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define ROW_START 2
#define N_ROWS 5
#define COL_START 0
#define N_COLS 5
#define LED PB5

#define ROW_MASK (((1 << N_ROWS) - 1) << ROW_START)
#define COL_MASK (((1 << N_COLS) - 1) << COL_START)
#define PCINT_ROW_MASK ((1 << PCINT22)|(1 << PCINT21)|(1 << PCINT20)|(1 << PCINT19)|(1 << PCINT18))

volatile int g_key = -1;

void keypad_sleep() {
    DDRD &= ~ROW_MASK;
    PORTD |= ROW_MASK;
    DDRC |= COL_MASK;
    PORTC &= ~COL_MASK;
}

ISR(PCINT2_vect) {
    PCMSK2 = 0;

    DDRD |= ROW_MASK;
    PORTD |= ROW_MASK;
    DDRC &= ~COL_MASK;
    PORTC |= COL_MASK;
    _delay_us(10);

    for (int r = 0; r < N_ROWS; r++) {
        PORTD &= ~(1 << (r + ROW_START));
        for (int c = 0; c < N_COLS; c++) {
            if (!(PINC & (1 << (c + COL_START)))) {
                g_key = (r * N_COLS) + c + 1;
                goto isr_found;
            }
        }
        PORTD |= (1 << (r + ROW_START));
    }

isr_found:
    keypad_sleep();
    PCMSK2 |= PCINT_ROW_MASK;
}

void blink(int count) {
    for (int i = 0; i < count; i++) {
        PORTB |= (1 << LED);
        _delay_ms(100);
        PORTB &= ~(1 << LED);
        _delay_ms(100);
    }
}

int main(void) {
    DDRB |= (1 << LED);
    keypad_sleep();

    PCICR |= (1 << PCIE2);
    PCMSK2 |= PCINT_ROW_MASK;
    sei();

    while (1) {
        if (g_key != -1) {
            cli();
            int key = g_key;
            g_key = -1;
            sei();
            blink(key);
        }
    }
    return 0;
}
