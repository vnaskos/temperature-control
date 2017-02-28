/*
 * main.c
 *
 * Created: 06-May-16 11:10:43 AM
 *  Author: Vasilis Naskos
 */ 
#define F_CPU 800000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "lcd16x2.h"
#include "i2c_master.h"

int tempTC74;
int tempLM35;
volatile int fanMode;
volatile int fanSpeed;
volatile uint8_t tot_overflow;

long absolute(long value);
void init_ADC(void);
void buttons_fan_init(void);
void readTC74Temp(void);
void controlFan(void);
void printToLCD(void);

long absolute(long value) {
	if (value < 0) return -value;
	
	return value;
}

void init_ADC()
{
	DDRA = 0xFF;        // Configure PortA as input
	ADCSRA = 0x8F;      // Enable ADC with clock pre-scalar to clk/128
	ADMUX = 0xE0;       // Select internal 2.56V as Vref, left justify
                        // data registers and select ADC0 as input channel
	ADCSRA |= 1<<ADSC;  // Start Conversion
}

//Get lm35 temperature and print to lcd
ISR(ADC_vect)
{	
	tempLM35 = ADCH;
	
	_delay_ms(50);
	
	ADCSRA |= 1<<ADSC;		// Start Conversion
}

void buttons_fan_init() {
	//3 first pins as input for the buttons
	//next 2 pins as output for the fan
	//rest pins, don't care
	DDRB = 0b11111000;
	PORTB |= 0b00000111;
	
	fanMode = 0;
	fanSpeed = 0;
}

//Read tc74 temperature
void readTC74Temp() {
	uint8_t data = 0;
	
	i2c_start(0x9A);
	i2c_write(0x00);
	i2c_rep_start(0x9B);
	data = i2c_readNak();
	i2c_stop();
	
	tempTC74 = data;
}

//Control fan speed using pwm
void controlFan() {
	int pwmSpeed;
	
	if(fanMode == 0) { //auto
		if(tempTC74 <= 10) {
			fanSpeed = 0;
		} else if(tempTC74 <= 20) {
			fanSpeed = 1;
		} else {
			fanSpeed = 2;
		}
	}
	
	if(fanSpeed == 0) {
		pwmSpeed = 0;
	} else {
		pwmSpeed = 255/(3-fanSpeed);
	}
	
	//send the speed value to PB3, PB4 pins
	OCR0 = pwmSpeed;
}

//Display all info on screen
void printToLCD() {
	char display[80];
	
	lcd_clrscr();
	
	if(absolute(tempTC74 - tempLM35) >= 5) {
		lcd_gotoxy(0, 0);
		lcd_puts("System Error");
		return;
	}
	
	lcd_gotoxy(0, 0);
	sprintf(display, "TC74: %d", tempTC74);
	lcd_puts(display);
	
	lcd_gotoxy(0, 1);
	sprintf(display, "LM35: %d", tempLM35);
	lcd_puts(display);
	
	lcd_gotoxy(14, 1);
	if(fanMode == 0) {
		lcd_puts("A");
	} else {
		lcd_puts("M");
	}
	itoa(fanSpeed, display, 10);
	lcd_puts(display);
}

void timer0_init()
{
	// set up timer with prescaler = 256
	TCCR2 |= (1 << CS22);
	
	// initialize counter
	TCNT2 = 0;
	
	// enable overflow interrupt
	TIMSK |= (1 << TOIE2);
	
	// enable global interrupts
	sei();
	
	// initialize overflow counter variable
	tot_overflow = 0;
}

// TIMER0 overflow interrupt service routine
// called whenever TCNT0 overflows
ISR(TIMER2_OVF_vect)
{
	// keep a track of number of overflows
	tot_overflow++;
	
	// check if no. of overflows = 12
	if (tot_overflow >= 9)  // NOTE: '>=' is used
	{
		// check if the timer count reaches 53
		if (TCNT2 >= 61)
		{
			//read tc74 temperature
			readTC74Temp();
			printToLCD();
			controlFan();
			
			TCNT2 = 0;            // reset counter
			tot_overflow = 0;     // reset overflow counter
		}
	}
}

int main(void) {
	lcd_init(LCD_DISP_ON_CURSOR);
	init_ADC();
	i2c_init();
	buttons_fan_init();
	timer0_init();
	
	TCCR0 |= (1<<WGM00)|(1<<WGM01);                    //select Fast PWM mode
	TCCR0 |= (1<<COM01)|(1<<CS00)|(0<<CS01)|(0<<CS02); //clear OC0 on compare match
	//Clock Prescaler is 1 (no prescaling) for ease of simulation
	
	while(1) {
		//fan mode auto - manual
		if(!(PINB & (1<<PB2))) {
			if(fanMode == 0)
				fanMode = 1;
			else
				fanMode = 0;
		}
		
		//increase speed
		if(!(PINB & (1<<PB1))) {
			if(fanSpeed < 2)
				fanSpeed++;
		}
		
		//decrease speed
		if(!(PINB & (1<<PB0))) {
			if(fanSpeed > 0)
				fanSpeed--;
		}
	}
	
	return 0;
}
