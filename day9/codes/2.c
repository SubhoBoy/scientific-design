#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum {
    T_NONE, T_NUM, T_OP, T_FN, T_LPAREN, T_RPAREN
} token_type_t;

typedef struct {
    token_type_t type;
    char val[16];
} token_t;

int tokenize(const char* in, token_t* tokens, int max) {
    int n = 0;
    const char* p = in;
    while (*p && n < max) {
        if (isspace(*p)) { p++; continue; }
        if (isdigit(*p) || *p == '.') {
            const char* s = p;
            while (isdigit(*p) || *p == '.') p++;
            int len = p - s;
            tokens[n].type = T_NUM;
            strncpy(tokens[n].val, s, len);
            tokens[n].val[len] = '\0';
            n++; continue;
        }
        if (strchr("+-*/^", *p)) {
            tokens[n].type = T_OP;
            tokens[n].val[0] = *p;
            tokens[n].val[1] = '\0';
            n++; p++; continue;
        }
        if (*p == '(') { tokens[n].type = T_LPAREN; strcpy(tokens[n].val, "("); n++; p++; continue; }
        if (*p == ')') { tokens[n].type = T_RPAREN; strcpy(tokens[n].val, ")"); n++; p++; continue; }
        if (isalpha(*p)) {
            const char* s = p;
            while (isalpha(*p)) p++;
            int len = p - s;
            tokens[n].type = T_FN;
            strncpy(tokens[n].val, s, len);
            tokens[n].val[len] = '\0';
            n++; continue;
        }
        p++;
    }
    return n;
}

void lcd_init(void);
void lcd_print(const char *str);
void lcd_cmd(uint8_t cmd);

int main(void) {
    const char* expr = "12.5+sin(3)";
    token_t tokens[10];
    lcd_init();
    int n = tokenize(expr, tokens, 10);
    for (int i = 0; i < n; i++) {
        lcd_cmd(0x01);
        lcd_print(tokens[i].val);
        _delay_ms(1000);
    }
    lcd_cmd(0x01);
    lcd_print("Done.");
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
void lcd_print(const char*s){while(*s)lcd_data(*s++);}
void lcd_init(void){DDRB|=LCD_DATA_MASK|(1<<LCD_RS)|(1<<LCD_EN);_delay_ms(50);lcd_write_nibble(0x30);_delay_ms(5);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x20);_delay_us(150);lcd_cmd(0x28);lcd_cmd(0x0C);lcd_cmd(0x01);_delay_ms(2);lcd_cmd(0x06);}
