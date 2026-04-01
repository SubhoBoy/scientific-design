#include <avr/io.h>

int main(void) {
    DDRB |= (1 << PB0);
    DDRB &= ~(1 << PB1);
    PORTB |= (1 << PB1);

    while (1) {
        if (!(PINB & (1 << PB1))) {
            PORTB |= (1 << PB0);
        } else {
            PORTB &= ~(1 << PB0);
        }
    }
    return 0;
}
