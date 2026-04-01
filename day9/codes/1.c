#include <stdlib.h>
#include <stdio.h>

#define STACK_SIZE 32

typedef struct {
    float data[STACK_SIZE];
    int top;
} fstack_t;

void fstack_init(fstack_t *s) { s->top = -1; }
int fstack_push(fstack_t *s, float val) {
    if (s->top >= STACK_SIZE - 1) return -1;
    s->data[++s->top] = val;
    return 0;
}
float fstack_pop(fstack_t *s) {
    if (s->top < 0) return 0.0f;
    return s->data[s->top--];
}

void lcd_init(void);
void lcd_print(const char *str);
void lcd_cmd(uint8_t cmd);

int main(void) {
    fstack_t s;
    char buf[16];

    lcd_init();
    fstack_init(&s);

    fstack_push(&s, 3.14f);
    fstack_push(&s, 1.61f);
    
    lcd_print("Popped: ");
    float val1 = fstack_pop(&s);
    dtostrf(val1, 4, 2, buf);
    lcd_print(buf);
    
    lcd_cmd(0xC0);
    lcd_print("Popped: ");
    float val2 = fstack_pop(&s);
    dtostrf(val2, 4, 2, buf);
    lcd_print(buf);

    while(1) {}
    return 0;
}

#include <avr/io.h>
#include <util/delay.h>
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
void lcd_print(const char*str){while(*str)lcd_data(*str++);}
void lcd_init(void){DDRB|=LCD_DATA_MASK|(1<<LCD_RS)|(1<<LCD_EN);_delay_ms(50);lcd_write_nibble(0x30);_delay_ms(5);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x20);_delay_us(150);lcd_cmd(0x28);lcd_cmd(0x0C);lcd_cmd(0x01);_delay_ms(2);lcd_cmd(0x06);}
