#include<p18f4520.h>
#pragma config OSC=HS
#pragma config LVP=OFF
#pragma config WDT=OFF

#define ldata PORTB
#define rs PORTDbits.RD0
#define rw PORTDbits.RD1
#define en PORTDbits.RD2
#define mic PORTAbits.RA0

int count;
int delay1 = 0;

void interrupt timer_isr(void)
{
	INTCONbits.GIE=0;
	TMR0H = 0XFA;                         // Reloading the timer values after overflow
	TMR0L = 0X23;     
    PORTAbits.RA2=~PORTAbits.RA2;
 	INTCONbits.TMR0IF = 0;	              //Resetting the timer overflow interrupt flag
}

void low_priority interrupt ISR(void)
{
    if(PIE1bits.TMR1IE && PIR1bits.TMR1IF)
    {
        count = 0;
        PORTAbits.RA1=~PORTAbits.RA1;
        PIR1bits.TMR1IF = 0;
    }
}

void delayms(unsigned int time)
{
    int i, j;
    for(i=0;i<time;i++)
        for(j=0;j<165;j++);
}

void delay(unsigned int time)
{
    int i;
    for(i=0;i<time;i++)
    {
        count=1;
        TMR1H=0xF4;
        TMR1L=0x47;
        T1CONbits.TMR1ON = 1;
        while(count>0);
        T1CONbits.TMR1ON = 0;
    }
    delay1=1;
}

void lcdcmd(unsigned char val)
{
	ldata=val;
	rs=0;
	rw=0;
	en=1;
	delayms(10);
	en=0;
}

void lcddata(unsigned char val)
{
	ldata=val;
	rs=1;
	rw=0;
	en=1;
	delayms(10);
	en=0;
}

void lcd_init()
{
	lcdcmd(0x38);    
	lcdcmd(0x0E);	 
	lcdcmd(0x06);	
	lcdcmd(0x01);	 
	lcdcmd(0x80);	 
}

void adc_init()
{
    
    ADCON0=0x01;
    ADCON1=0x0C;
    ADCON2=0x95;
        
}

unsigned int adc_conv()
{
	unsigned int adc_result;
	ADCON0bits.ADON=1;
	ADCON0bits.GO=1;
	while(ADCON0bits.GO==1);
	adc_result=ADRESL;
	adc_result|=((unsigned int)ADRESH)<<8;
    lcddata(adc_result);
	return(adc_result);
}

void uart_init_trans()
{
    TXSTA=0x24;
    BAUDCON=0x18;
    SPBRG=155;
}

void uart_transmit(unsigned int data)
{
    unsigned char datal=((data)&(0xFF));
    unsigned char datah=data>>8;
    TXREG=datah;
    RCSTAbits.SPEN=1;
    while(PIR1bits.TXIF==0);
    RCSTAbits.SPEN=0;
    TXREG=datal;
    RCSTAbits.SPEN=1;
    while(PIR1bits.TXIF==0);
    RCSTAbits.SPEN=0;
}

void timer()
{
    T0CON = 0x0A;				
  	TMR0H = 0xFE;               // Reset Timer0 to 0xFE99 for 125 microsecond delay
  	TMR0L = 0x99;   
	INTCONbits.GIE = 1;			// Global interrupt enabled
	INTCONbits.TMR0IE = 1;		// TMR0 interrupt enabled
    T0CONbits.TMR0ON = 1;
}

unsigned char uart_init_receive()
{
    RCSTA=0x10;
    BAUDCON=0x18;
    SPBRG=155;
    INTCONbits.GIEL=1;
    PIE1bits.RCIE=1;
    RCSTAbits.SPEN=1;
    while(!PIR1bits.RCIF);
    return RCREG;
}

lcd_string(char* a)
{
    int i=0;
    while(*(a+i)!='\0')
    {
        lcddata(*(a+i));
        i++;
    }
}

void sampler(unsigned int sampling_time)
{
    unsigned int data;
    delay(sampling_time);
    while(!delay1)
    {
        timer();
        if(INTCONbits.TMR0IF==0)
        {
            data=adc_conv();
            uart_transmit(data);
        }
    }
}

void main()
{
    unsigned int data, result;
    unsigned char a[]="WELCOME";
    unsigned char b[]="SPEAK PASSWORD";
    unsigned char c[]="1.CHANGE PASS";
    unsigned char d[]="2.FEED NEW VOICE";
    unsigned char e[]="3.OPEN DOOR";
    unsigned char f[]="CHANGED PASS";
    unsigned char g[]="STORED NEW VOICE";
    unsigned char h[]="DOOR NOW OPEN";
    TRISC=0x80;
    TRISB=0x00;
    TRISD=0x00;
    TRISA=0x01;
    PORTAbits.RA1=1;
    PORTAbits.RA2=1;
    adc_init();
    uart_init_trans();
    lcd_init();
    lcd_string(a);
    lcdcmd(0xC0);
    lcd_string(b);
    while(!(adc_conv()>200));
    sampler(1000);
    delayms(3000);
    uart_transmit('A');
    result=uart_init_receive();
    if(result=='A')
    {
        lcd_string(c);
        lcdcmd(0xC0);
        lcd_string(d);
        delayms(1000);
        lcdcmd(0x80);
        lcd_string(d);
        lcdcmd(0xC0);
        lcd_string(e);
    }
    if(PORTCbits.RC0==1)
    {
        uart_init_trans();
        uart_transmit('A');
        sampler(1000);
        delayms(3000);
        uart_transmit('A');
        result=uart_init_receive();
        if(result=='A')
            lcd_string(f);
    }
    else if(PORTCbits.RC1==1)
    {
        uart_init_trans();
        uart_transmit('B');
        sampler(1000);
        delayms(3000);
        uart_transmit('A');
        result=uart_init_receive();
        if(result=='A')
            lcd_string(g);
    } 
    else if(PORTCbits.RC2==1)
    {
        PORTCbits.RC3=1;
        lcd_string(h);
    }
    else;
}
