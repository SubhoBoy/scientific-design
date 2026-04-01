#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>

float q_rsqrt(float number) {
    int32_t i;
    float x2, y;
    const float threehalfs = 1.5F;
    x2 = number * 0.5F;
    y = number;
    i = *(int32_t *)&y;
    i = 0x5f3759df - (i >> 1); // What the fuck?
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));
    return y;
}

void ftoa(float val, char* buf, int precision) {
    long ipart = (long)val;
    itoa(ipart, buf, 10);
    while (*buf != '\0') buf++;
    *buf++ = '.';
    float fpart = val - ipart;
    for (int i=0;i<precision;i++) fpart *= 10;
    long fpart_i = (long)fpart;
    itoa(fpart_i, buf, 10);
}

void lcd_init(void);
void lcd_print(const char *str);
void lcd_cmd(uint8_t cmd);

int main(void) {
    lcd_init();
    lcd_print("q_rsqrt(2.0)");
    
    float result = q_rsqrt(2.0);
    char buffer[16];
    ftoa(result, buffer, 4);

    _delay_ms(1500);
    lcd_cmd(0x01);
    lcd_print("1/sqrt(2)=");
    lcd_print(buffer);

    while(1) {}
    return 0;
}

#define LCD_PORT PORTB
#define LCD_DDR  DDRB
#define LCD_RS   PB0
#define LCD_EN   PB1
#define LCD_DATA_MASK 0x3C
void lcd_pulse_en(void){LCD_PORT|=(1<<LCD_EN);_delay_us(1);LCD_PORT&=~(1<<LCD_EN);_delay_us(50);}
void lcd_write_nibble(uint8_t n){LCD_PORT=(LCD_PORT&~LCD_DATA_MASK)|((n>>2)&LCD_DATA_MASK);lcd_pulse_en();}
void lcd_write(uint8_t b,uint8_t is_data){if(is_data)LCD_PORT|=(1<<LCD_RS);else LCD_PORT&=~(1<<LCD_RS);lcd_write_nibble(b);lcd_write_nibble(b<<4);}
void lcd_cmd(uint8_t c){lcd_write(c,0);}
void lcd_data(uint8_t d){lcd_write(d,1);}
void lcd_print(const char*s){while(*s)lcd_data(*s++);}
void lcd_init(void){DDRB|=LCD_DATA_MASK|(1<<LCD_RS)|(1<<LCD_EN);_delay_ms(50);lcd_write_nibble(0x30);_delay_ms(5);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x20);_delay_us(150);lcd_cmd(0x28);lcd_cmd(0x0C);lcd_cmd(0x01);_delay_ms(2);lcd_cmd(0x06);}
