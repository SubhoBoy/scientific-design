#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef enum { T_NONE, T_NUM, T_OP, T_FN, T_LPAREN, T_RPAREN } token_type_t;
typedef struct { token_type_t type; char val[16]; } token_t;
typedef struct { token_t data[32]; int top; } tstack_t;

int tokenize(const char* in, token_t* tokens, int max);
void lcd_init(void);
void lcd_print(const char *str);
void lcd_cmd(uint8_t cmd);

void tstack_init(tstack_t *s) { s->top = -1; }
int tstack_push(tstack_t *s, token_t v) { if(s->top>=31)return -1; s->data[++s->top]=v; return 0; }
token_t tstack_pop(tstack_t *s) { token_t t; t.type=T_NONE; if(s->top<0)return t; return s->data[s->top--]; }
token_t tstack_peek(tstack_t *s) { token_t t; t.type=T_NONE; if(s->top<0)return t; return s->data[s->top]; }

int prec(const token_t* t) {
    if (t->type == T_FN) return 4;
    if (t->type != T_OP) return 0;
    switch (t->val[0]) {
        case '^': return 3;
        case '*': case '/': return 2;
        case '+': case '-': return 1;
        default: return 0;
    }
}

int shunt(token_t* infix, int in_len, token_t* rpn) {
    int rpn_len = 0;
    tstack_t ops;
    tstack_init(&ops);

    for (int i = 0; i < in_len; i++) {
        token_t t = infix[i];
        if (t.type == T_NUM) { rpn[rpn_len++] = t; }
        else if (t.type == T_FN || t.type == T_LPAREN) { tstack_push(&ops, t); }
        else if (t.type == T_OP) {
            while (ops.top > -1 && prec(&tstack_peek(&ops)) >= prec(&t)) {
                rpn[rpn_len++] = tstack_pop(&ops);
            }
            tstack_push(&ops, t);
        } else if (t.type == T_RPAREN) {
            while (ops.top > -1 && tstack_peek(&ops).type != T_LPAREN) {
                rpn[rpn_len++] = tstack_pop(&ops);
            }
            tstack_pop(&ops);
            if (ops.top > -1 && tstack_peek(&ops).type == T_FN) {
                rpn[rpn_len++] = tstack_pop(&ops);
            }
        }
    }
    while (ops.top > -1) { rpn[rpn_len++] = tstack_pop(&ops); }
    return rpn_len;
}

int main(void) {
    const char* expr = "3+4*2/(1-5)^2";
    token_t infix_tokens[20], rpn_tokens[20];
    char buf[32] = "";

    lcd_init();
    int n_infix = tokenize(expr, infix_tokens, 20);
    int n_rpn = shunt(infix_tokens, n_infix, rpn_tokens);

    lcd_print("RPN: ");
    for (int i = 0; i < n_rpn; i++) {
        strcat(buf, rpn_tokens[i].val);
        strcat(buf, " ");
    }
    lcd_print(buf);

    while(1) {}
    return 0;
}

#include <avr/io.h>
#include <util/delay.h>
int tokenize(const char*p,token_t*t,int max){int n=0;while(*p&&n<max){if(isspace(*p)){p++;continue;}if(isdigit(*p)||*p=='.'){const char*s=p;while(isdigit(*p)||*p=='.')p++;int l=p-s;t[n].type=T_NUM;strncpy(t[n].val,s,l);t[n].val[l]='\0';n++;continue;}if(strchr("+-*/^",*p)){t[n].type=T_OP;t[n].val[0]=*p;t[n].val[1]='\0';n++;p++;continue;}if(*p=='('){t[n].type=T_LPAREN;strcpy(t[n].val,"(");n++;p++;continue;}if(*p==')'){t[n].type=T_RPAREN;strcpy(t[n].val,")");n++;p++;continue;}if(isalpha(*p)){const char*s=p;while(isalpha(*p))p++;int l=p-s;t[n].type=T_FN;strncpy(t[n].val,s,l);t[n].val[l]='\0';n++;continue;}p++;}return n;}
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
