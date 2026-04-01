#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ROW_START 2
#define N_ROWS 5
#define COL_START 0
#define N_COLS 5
#define ROW_MASK 0x7C
#define COL_MASK 0x1F
#define PCINT_ROW_MASK 0x7C

#define LCD_PORT PORTB
#define LCD_DDR  DDRB
#define LCD_RS   PB0
#define LCD_EN   PB1
#define LCD_DATA_MASK 0x3C

#define BUF_SIZE 32
#define STACK_SIZE 32

volatile char g_buf[BUF_SIZE];
volatile int g_buf_i = 0;
volatile uint8_t g_eval = 0;

const char KEYMAP[5][5]={{'s','c','t','^','/'},{'7','8','9','*','('},{'4','5','6','-',')'},{'1','2','3','+','l'},{'0','.','e','=','C'}};

typedef enum { T_NONE, T_NUM, T_OP, T_FN, T_LPAREN, T_RPAREN } token_type_t;
typedef struct { token_type_t type; char val[16]; } token_t;
typedef struct { token_t data[STACK_SIZE]; int top; } tstack_t;
typedef struct { float data[STACK_SIZE]; int top; } fstack_t;

void lcd_init(void);
void lcd_data(uint8_t d);
void lcd_cmd(uint8_t cmd);
void keypad_sleep(void);
int tokenize(const char* in, token_t* tokens, int max);
int shunt(token_t* infix, int in_len, token_t* rpn);
float eval(token_t* rpn, int rpn_len);
void ftoa(float val, char* buf, int p);

ISR(PCINT2_vect) {
    PCMSK2 = 0;
    DDRD |= ROW_MASK; PORTD |= ROW_MASK;
    DDRC &= ~COL_MASK; PORTC |= COL_MASK;
    _delay_us(10);
    char key = 0;
    for (int r=0;r<N_ROWS;r++){PORTD&=~(1<<(r+ROW_START));for(int c=0;c<N_COLS;c++){if(!(PINC&(1<<(c+COL_START)))){key=KEYMAP[r][c];goto isr_end;}}PORTD|=(1<<(r+ROW_START));}
isr_end:
    if(key){if(key=='='){g_eval=1;}else if(key=='C'){g_buf_i=0;g_buf[0]='\0';lcd_cmd(0x01);}else if(g_buf_i<BUF_SIZE-1){g_buf[g_buf_i++]=key;g_buf[g_buf_i]='\0';lcd_data(key);}}
    keypad_sleep();
    PCMSK2 |= PCINT_ROW_MASK;
}

int main(void) {
    lcd_init();
    keypad_sleep();
    PCICR |= (1 << PCIE2);
    PCMSK2 |= PCINT_ROW_MASK;
    sei();
    lcd_cmd(0x80); lcd_print("Ready");

    while (1) {
        if (g_eval) {
            cli();
            g_eval = 0;
            lcd_cmd(0xC0); lcd_print("Calc...");
            token_t infix[STACK_SIZE], rpn[STACK_SIZE];
            int n_infix = tokenize((char*)g_buf, infix, STACK_SIZE);
            int n_rpn = shunt(infix, n_infix, rpn);
            float result = eval(rpn, n_rpn);
            char res_buf[16];
            ftoa(result, res_buf, 3);
            lcd_cmd(0xC0); lcd_print("=           ");
            lcd_cmd(0xC2); lcd_print(res_buf);
            g_buf_i = 0; g_buf[0] = '\0';
            sei();
        }
    }
    return 0;
}

void lcd_pulse_en(void){LCD_PORT|=(1<<LCD_EN);_delay_us(1);LCD_PORT&=~(1<<LCD_EN);_delay_us(50);}
void lcd_write_nibble(uint8_t n){LCD_PORT=(LCD_PORT&~LCD_DATA_MASK)|((n>>2)&LCD_DATA_MASK);lcd_pulse_en();}
void lcd_write(uint8_t b,uint8_t is_data){if(is_data)LCD_PORT|=(1<<LCD_RS);else LCD_PORT&=~(1<<LCD_RS);lcd_write_nibble(b);lcd_write_nibble(b<<4);}
void lcd_cmd(uint8_t c){lcd_write(c,0);}
void lcd_data(uint8_t d){lcd_write(d,1);}
void lcd_print(const char*s){while(*s)lcd_data(*s++);}
void lcd_init(void){DDRB|=LCD_DATA_MASK|(1<<LCD_RS)|(1<<LCD_EN);_delay_ms(50);lcd_write_nibble(0x30);_delay_ms(5);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x30);_delay_us(150);lcd_write_nibble(0x20);_delay_us(150);lcd_cmd(0x28);lcd_cmd(0x0C);lcd_cmd(0x01);_delay_ms(2);lcd_cmd(0x06);}
void keypad_sleep(void){DDRD&=~ROW_MASK;PORTD|=ROW_MASK;DDRC|=COL_MASK;PORTC&=~COL_MASK;}
void fstack_init(fstack_t*s){s->top=-1;}
int fstack_push(fstack_t*s,float v){if(s->top>=STACK_SIZE-1)return-1;s->data[++s->top]=v;return 0;}
float fstack_pop(fstack_t*s){if(s->top<0)return 0.0f;return s->data[s->top--];}
void tstack_init(tstack_t*s){s->top=-1;}
int tstack_push(tstack_t*s,token_t v){if(s->top>=STACK_SIZE-1)return-1;s->data[++s->top]=v;return 0;}
token_t tstack_pop(tstack_t*s){token_t t;t.type=T_NONE;if(s->top<0)return t;return s->data[s->top--];}
token_t tstack_peek(tstack_t*s){token_t t;t.type=T_NONE;if(s->top<0)return t;return s->data[s->top];}
float deriv_exp(float x,float y){return y;}
float deriv_ln(float x,float y){return 1.0/x;}
float rk4(float x,float y,float h,float(*df)(float,float)){float k1=h*df(x,y),k2=h*df(x+0.5*h,y+0.5*k1),k3=h*df(x+0.5*h,y+0.5*k2),k4=h*df(x+h,y+k3);return y+(k1+2*k2+2*k3+k4)/6.0;}
float solve(float x0,float y0,float xt,float(*df)(float,float)){float x=x0,y=y0;const float h=0.01;int n=(int)((xt-x0)/h);for(int i=0;i<n;i++){y=rk4(x,y,h,df);x+=h;}if(xt-x>1e-6)y=rk4(x,y,xt-x,df);return y;}
int tokenize(const char*p,token_t*t,int max){int n=0;while(*p&&n<max){if(isspace(*p)){p++;continue;}if(isdigit(*p)||*p=='.'){const char*s=p;while(isdigit(*p)||*p=='.')p++;int l=p-s;t[n].type=T_NUM;strncpy(t[n].val,s,l);t[n].val[l]='\0';n++;continue;}if(strchr("+-*/^",*p)){t[n].type=T_OP;t[n].val[0]=*p;t[n].val[1]='\0';n++;p++;continue;}if(*p=='('){t[n].type=T_LPAREN;strcpy(t[n].val,"(");n++;p++;continue;}if(*p==')'){t[n].type=T_RPAREN;strcpy(t[n].val,")");n++;p++;continue;}if(isalpha(*p)){if(*p=='s'){t[n].type=T_FN;strcpy(t[n].val,"sin");n++;p+=1;continue;}if(*p=='c'){t[n].type=T_FN;strcpy(t[n].val,"cos");n++;p+=1;continue;}if(*p=='t'){t[n].type=T_FN;strcpy(t[n].val,"tan");n++;p+=1;continue;}if(*p=='l'){t[n].type=T_FN;strcpy(t[n].val,"ln");n++;p+=1;continue;}if(*p=='e'){t[n].type=T_FN;strcpy(t[n].val,"exp");n++;p+=1;continue;}}p++;}return n;}
int prec(const token_t*t){if(t->type==T_FN)return 4;if(t->type!=T_OP)return 0;switch(t->val[0]){case'^':return 3;case'*':case'/':return 2;case'+':case'-':return 1;}return 0;}
int shunt(token_t*in,int in_len,token_t*rpn){int rpn_len=0;tstack_t ops;tstack_init(&ops);for(int i=0;i<in_len;i++){token_t t=in[i];if(t.type==T_NUM){rpn[rpn_len++]=t;}else if(t.type==T_FN||t.type==T_LPAREN){tstack_push(&ops,t);}else if(t.type==T_OP){while(ops.top>-1&&prec(&tstack_peek(&ops))>=prec(&t)){rpn[rpn_len++]=tstack_pop(&ops);}tstack_push(&ops,t);}else if(t.type==T_RPAREN){while(ops.top>-1&&tstack_peek(&ops).type!=T_LPAREN){rpn[rpn_len++]=tstack_pop(&ops);}tstack_pop(&ops);if(ops.top>-1&&tstack_peek(&ops).type==T_FN){rpn[rpn_len++]=tstack_pop(&ops);}}}while(ops.top>-1){rpn[rpn_len++]=tstack_pop(&ops);}return rpn_len;}
float eval(token_t*rpn,int len){fstack_t vs;fstack_init(&vs);for(int i=0;i<len;i++){token_t t=rpn[i];if(t.type==T_NUM){fstack_push(&vs,atof(t.val));}else if(t.type==T_OP){float b=fstack_pop(&vs);float a=fstack_pop(&vs);switch(t.val[0]){case'+':fstack_push(&vs,a+b);break;case'-':fstack_push(&vs,a-b);break;case'*':fstack_push(&vs,a*b);break;case'/':fstack_push(&vs,a/b);break;case'^':fstack_push(&vs,solve(1,a,b,deriv_ln));break;}}else if(t.type==T_FN){float a=fstack_pop(&vs);if(t.val[0]=='e')fstack_push(&vs,solve(0,1,a,deriv_exp));else if(t.val[0]=='l')fstack_push(&vs,solve(1,0,a,deriv_ln));}}return fstack_pop(&vs);}
void ftoa(float val,char*b,int p){long ip=(long)val;if(val<0){*b++='-';val=-val;ip=(long)val;}itoa(ip,b,10);while(*b!='\0')b++;*b++='.';float fp=val-ip;for(int i=0;i<p;i++)fp*=10;long fpi=(long)fp;itoa(fpi,b,10);}
