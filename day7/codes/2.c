#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

const float conv_factors[] PROGMEM = {
    2.54, 0.3048, 1.60934, 0.453592, 3.78541
};

// Forward declarations for LCD functions
void lcd_init(void);
void lcd_print(const char *str);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);

int main(void) {
    lcd_init();
    lcd_print("PROGMEM Test");
    _delay_ms(1500);
    lcd_cmd(0x01);

    float ft_to_m = pgm_read_float(&conv_factors[1]);

    char buffer[16];
    char val_buf[8];
    
    dtostrf(ft_to_m, 4, 4, val_buf); // val, width, precision, buffer
    snprintf(buffer, 16, "1ft=%sm", val_buf);
    
    lcd_print(buffer);

    while(1) {}
    return 0;
}

// --- LCD Driver Implementation ---
#define LCD_PORT PORTB
#define LCD_DDR  DDRB
#define LCD_RS   PB0
#define LCD_EN   PB1
#define LCD_D4   PB2
#define LCD_DATA_MASK 0x3C

void lcd_pulse_en(void){LCD_PORT|=(1<<LCD_EN);_delay_us(1);LCD_PORT&=~(1<<LCD_EN);_delay_us(50);}
void lcd_write_nibble(uint8_t n){LCD_PORT=(LCD_PORT&~LCD_DATA_MASK)|((n>>2)&LCD_DATA_MASK);lcd_pulse_en();}
void lcd_write(uint8_t b,uint8_t is_data){if(is_data)LCD_PORT|=(1<<LCD_RS);else LCD_PORT&=~(1<<LCD_RS);lcd_write_nibble(b);lcd_write_nibble(b<<4);}
void lcd_cmd(uint8_t c){lcd_write(c,0);}
void lcd_data(uint8_t d){lcd_write(d,1);}
void lcd_print(const char*s){while(*s)lcd_data(*s++);}
void lcd_init(void){DDRB|=LCD_DATA_MASK|(1<<LCD_RS)|(1<<LCD_EN);_delay_ms(50);lcd_write_nibble(0x30);_delay_ms(5);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x20);_delay_us(150);lcd_cmd(0x28);lcd_cmd(0x0C);lcd_cmd(0x01);_delay_ms(2);lcd_cmd(0x06);}
