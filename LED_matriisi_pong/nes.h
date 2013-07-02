#define DATA0 0
#define OUT 1
#define STCP 2
#define SHCP 3 //notted!
#define LATCH 4
#define DATA1 5
#define DATA2 6
#define DATA3 7

#define NESDELAY 80 //microseconds

#define NES_BUTTON_A 7
#define NES_BUTTON_B 6
#define NES_BUTTON_START 4
#define NES_BUTTON_SELECT 5
#define NES_BUTTON_UP 3
#define NES_BUTTON_DOWN 2
#define NES_BUTTON_RIGHT 0
#define NES_BUTTON_LEFT 1


volatile uint8_t nes[4];

void nessetup()
{
	DDRD=0b11110;
	PORTD=0b11100001;

	TCCR0B= (1<<CS02)|(1<<CS00);
	TIMSK0= (1<<TOIE0);
	sei();
	
	PORTD|=(1<<STCP); //HIGH
	PORTD&=~(1<<SHCP); //LOW
/*
 pinMode(DATA, INPUT);
 pinMode(OUT, OUTPUT);
 pinMode(STCP, OUTPUT);
 pinMode(SHCP, OUTPUT);
 pinMode(LATCH, OUTPUT);
 digitalWrite(DATA, HIGH);
 pinMode(13, OUTPUT; //debug
 //setup interupt*/
}

void nesread()
{
	uint8_t i;
	//reset signal for gamepad
	PORTD|=(1<<SHCP)|(1<<LATCH)|(1<<OUT); //HIGH
	_delay_us(NESDELAY);
	PORTD&=~((1<<LATCH)|(1<<SHCP)); //LOW
	//empty cycle for the debugger
	_delay_us(NESDELAY);
	PORTD|=(1<<SHCP); //HIGH
	_delay_us(NESDELAY);
	PORTD&=~(1<<SHCP); //LOW
	//insert bits into variable
	for(i=0;i<8;i++)
	{
		nes[0] =(nes[0]<<1)|(PIND & (1 << DATA0));
		nes[1] =(nes[1]<<1)|(PIND & (1 << DATA1));
		nes[2] =(nes[2]<<1)|(PIND & (1 << DATA2));
		nes[3] =(nes[3]<<1)|(PIND & (1 << DATA3));

		_delay_us(NESDELAY);
		PORTD&=~(1<<STCP); //LOW
		PORTD|=(1<<SHCP); //HIGH
		_delay_us(NESDELAY);
		PORTD|=(1<<STCP); //HIGH
		PORTD&=~(1<<SHCP); //LOW
	}
	_delay_us(NESDELAY);
	//display data on debugger
	PORTD|=(1<<SHCP); //HIGH
	PORTD&=~(1<<OUT); //LOW

	
}

ISR(TIMER0_OVF_vect)
{
	nesread();
}
/*
void nesdebug()
{
	int i;
	for(i=0;i<8;i++)
	{
		matrix[0][i]=(nes>>i)&1;
	}
	//matrix[1][0]=(nes>>0)&1;
}*/

