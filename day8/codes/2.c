#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

typedef float (*deriv_f)(float x, float y);

float rk4(float x, float y, float h, deriv_f df) {
    float k1=h*df(x,y), k2=h*df(x+0.5*h,y+0.5*k1), k3=h*df(x+0.5*h,y+0.5*k2), k4=h*df(x+h,y+k3);
    return y + (k1 + 2*k2 + 2*k3 + k4) / 6.0;
}

float solve(float x0, float y0, float x_target, deriv_f df) {
    float x=x0, y=y0; const float h=0.01; int n=(int)((x_target-x0)/h);
    for (int i=0;i<n;i++){ y=rk4(x,y,h,df); x+=h; }
    if(x_target-x > 1e-6) y=rk4(x,y,x_target-x,df);
    return y;
}

float deriv_exp(float x, float y) { return y; }

void ftoa(float val, char* buf, int precision) {
    long ipart = (long)val;
    if (val < 0) { *buf++ = '-'; val = -val; ipart = (long)val; }
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
    lcd_print("Calc e^2...");
    
    float result = solve(0.0, 1.0, 2.0, deriv_exp);
    char buffer[16];
    ftoa(result, buffer, 3);

    lcd_cmd(0x01);
    lcd_print("e^2 = ");
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
