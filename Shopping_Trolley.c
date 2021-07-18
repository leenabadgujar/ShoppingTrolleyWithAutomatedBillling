#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

uint8_t data,rfid[12];
unsigned char i,count = 0,icount = 0;
unsigned int usart_getch();

void lcd_data(unsigned char data)
{
	PORTA = data;
	PORTB |= (1<<PB0); // RS = 1;
	PORTB &= (~(1<<PB1)); // RW = 0
	PORTB |= (1<<PB2);   // EN =1
	_delay_ms(10);     // 10MS
	PORTB &= (~(1<<PB2));  // EN =0;
}

void lcd_cmd(unsigned char command)
{
	PORTA = command;
	PORTB &= (~(1<<PB0)); // rs = 0;
	PORTB &= (~(1<<PB1));  // rw =0;
	PORTB |= (1<<PB2);   // en =1;
	_delay_ms(10);    // 10ms
	PORTB &= (~(1<<PB2)); // en =0;
}
void lcd_string(const unsigned char *str)
{
	int i=0,length;
	length = strlen(str);
	for(i=0;i<length;i++)
	{
		lcd_data(str[i]);
	}
}
void lcd_initialise()
{
	lcd_cmd(0x38);
	lcd_cmd(0x01);
	lcd_cmd(0x0E);
	lcd_cmd(0x80);
}

void getcard_id(void)	//Function to get 12 byte ID no. from rfid card
{	
	for(i=0;i<12;i++) 
	{
		rfid[i]= usart_getch();	// receive card value byte by byte
	}
	return;
}

void usart_init()
{
	UCSRB |= (1 << RXEN) | (1 << TXEN);   // Turn on the transmission and reception circuitry
	UCSRC |= (1 << URSEL) | (1<<USBS) | (1 << UCSZ0) | (1 << UCSZ1); 
															// Use 8-bit character sizes

	UBRRL = BAUD_PRESCALE; 	// Load lower 8-bits of the baud rate value..
							// into the low byte of the UBRR register
	UBRRH = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value..
								  // into the high byte of the UBRR register
}

 
unsigned int usart_getch()
{
	while ((UCSRA & (1 << RXC)) == 0); // Do nothing until data have been received..
					   // and is ready to be read from UDR
	return(UDR); // return the byte
}


int main()
{
	DDRA = 0xFF; // //PORTD AS O/P
   
    DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2); // PB0,PB1,PB2 AS O/P
	 
	DDRC &= ~(1<<DDC0);
	DDRC &= ~(1<<DDC1);
	 
	lcd_initialise();
	_delay_ms(50);
	usart_init();		// initiailztion of USART 
	lcd_string("WELCOME");
	_delay_ms(50);
	
    // ENABLE THE GLOBAL INTERRUPT ENABLE BIT
	sei();	
		
	while(1)
	{
	  getcard_id();	// Function to get RFID card no. from serial port
	  _delay_ms(10);
	  
	  if(PINC & (1<<PINC0))
	  {
		lcd_cmd(0x1);
	    lcd_string("DELETING THE");
		if(rfid[10] == '9' && rfid[11] == '1')
	  {
		lcd_cmd(0xC0);
		lcd_string("GOODDAY BISCUIT");
	  }	
	  if(rfid[10] == 'D' && rfid[11] == '5')
	  {
		lcd_cmd(0xC0);
	    lcd_string("NICE BISCUIT");
	  }
	  if(rfid[10] == 'F' && rfid[11] == '1')
	  {
		lcd_cmd(0xC0);
	    lcd_string("COCA-COLA");
	  }	
	  if(rfid[10] == 'B' && rfid[11] == 'E')
	  {
		lcd_cmd(0xC0);
	    lcd_string("CADBURY DAIRY MILK");
	  }			  		
		lcd_cmd(0x94);
		lcd_string("Total : ");
		lcd_cmd(0x9C);
		count = count - icount;
		char scount[10];
		itoa(count,scount,10);
		lcd_string(scount);  
	  }
	else{	  
	  if(rfid[10] == '9' && rfid[11] == '1')
	  {
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_string("P1 : GOODDAY BISCUIT");
		lcd_cmd(0xC0);
		lcd_string("PRICE : 20");
		lcd_cmd(0x94);
		lcd_string("EXPIRY : MAY 2021");
		lcd_cmd(0xD4);
		lcd_string("TOTAL :");
		lcd_cmd(0xDC);
		count = count + 20;
		icount = 20;
		char scount[10];
		itoa(count,scount,10);
		lcd_string(scount);		
	}		
	if(rfid[10] == 'D' && rfid[11] == '5')
	{
		lcd_cmd(0x01);
		lcd_cmd(0x80);
		lcd_string("P2 : NICE BISCUIT");
		lcd_cmd(0xC0);
		lcd_string("PRICE : 15");
		lcd_cmd(0x94);
		lcd_string("EXPIRY : MAR 2021");
		lcd_cmd(0xD4);
		lcd_string("TOTAL : ");
		lcd_cmd(0xDC);
		count = count + 15;
		icount = 15;
		char scount[10];
		itoa(count,scount,10);
		lcd_string(scount);
	}
	if(rfid[10] == 'B' && rfid[11] == 'E')
	{
		lcd_cmd(0x1);
		lcd_string("P3: CADBURY DAIRY MILK");
		lcd_cmd(0xC0);
		lcd_string("Price : 20");
		lcd_cmd(0x94);
		lcd_string("Expiry : MAY 2021");
		lcd_cmd(0xD4);
		lcd_string("Total : ");
		lcd_cmd(0xDC);
		count = count + 20;
		icount = 20;
		char scount[10];
		itoa(count,scount,10);
		lcd_string(scount);
	}
	if(rfid[10] == 'F' && rfid[11] == '1')
	{
		lcd_cmd(0x1);
		lcd_string("P4: COCA-COLA");
		lcd_cmd(0xC0);
		lcd_string("Price : 30");
		lcd_cmd(0x94);
		lcd_string("Expiry : NOV 2021");
		lcd_cmd(0xD4);
		lcd_string("Total : ");
		lcd_cmd(0xDC);
		count = count + 30;
		icount = 30;
		char scount[10];
		itoa(count,scount,10);	
		lcd_string(scount);
	}
  }
  if(PINC & (1<<PINC1))
	  {
		  lcd_cmd(0x1);
		lcd_string("PROCED FOR PAYMENT");
		lcd_cmd(0xC0);
		lcd_string("Total : ");
		lcd_cmd(0xC8);
		char scount[10];
		count = count - icount;
		itoa(count,scount,10);
		lcd_string(scount);
		_delay_ms(10000);
		lcd_cmd(0x1);
		lcd_string("THANKS FOR SHOPPING");
	  }		  
 }  
}

