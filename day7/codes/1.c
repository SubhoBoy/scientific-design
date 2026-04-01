#include <avr/io.h>
#include <util/delay.h>

#define LCD_PORT PORTB
#define LCD_DDR  DDRB
#define LCD_RS   PB0
#define LCD_EN   PB1
#define LCD_D4   PB2
#define LCD_D5   PB3
#define LCD_D6   PB4
#define LCD_D7   PB5
#define LCD_DATA_MASK ((1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7))
#define LCD_CTRL_MASK ((1<<LCD_RS)|(1<<LCD_EN))

void lcd_pulse_en(void) {
    LCD_PORT |= (1 << LCD_EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_EN);
    _delay_us(50);
}

void lcd_write_nibble(uint8_t nibble) {
    LCD_PORT &= ~LCD_DATA_MASK;
    if (nibble & (1<<4)) LCD_PORT |= (1 << LCD_D4);
    if (nibble & (1<<5)) LCD_PORT |= (1 << LCD_D5);
    if (nibble & (1<<6)) LCD_PORT |= (1 << LCD_D6);
    if (nibble & (1<<7)) LCD_PORT |= (1 << LCD_D7);
    lcd_pulse_en();
}

void lcd_write(uint8_t byte, uint8_t is_data) {
    if (is_data) LCD_PORT |= (1 << LCD_RS);
    else LCD_PORT &= ~(1 << LCD_RS);
    
    lcd_write_nibble(byte & 0xF0);
    lcd_write_nibble((byte << 4) & 0xF0);
}

void lcd_cmd(uint8_t cmd) {
    lcd_write(cmd, 0);
}

void lcd_data(uint8_t data) {
    lcd_write(data, 1);
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_data(*str++);
    }
}

void lcd_init(void) {
    LCD_DDR |= (LCD_DATA_MASK | LCD_CTRL_MASK);
    _delay_ms(50);

    lcd_write_nibble(0x30); _delay_ms(5);
    lcd_write_nibble(0x30); _delay_us(150);
    lcd_write_nibble(0x30); _delay_us(150);
    lcd_write_nibble(0x20); _delay_us(150);

    lcd_cmd(0x28); // 4-bit, 2-line, 5x8
    lcd_cmd(0x0C); // Display on, cursor off
    lcd_cmd(0x01); // Clear
    _delay_ms(2);
    lcd_cmd(0x06); // Increment cursor
}

int main(void) {
    lcd_init();
    lcd_print("Hello World!");
    while(1) {}
    return 0;
}
