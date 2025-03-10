/*
 * Timers.c
 *
 * Created: 24.02.2022 13:43:26
 *  Author: Ondra
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "Timers.h"
#include "../makra.h"

void Timer0_cmp_start(uint8_t compare_value){
	
	cli();          // disable global int.
	TCCR0A = 0;     // clean registers
	TCCR0B = 0; //
	TIMSK0 = 0;    //
	// Value to compare
	OCR0A = compare_value;
	// CTC mode:
	TCCR0A |= (1 << WGM01);
	// 1024 prescale:
	//TCCR0B |= (1 << CS00);
	//TCCR0B |= (1 << CS02);
	TCCR0B |= PRESCALE;
	// enable int.:
	TIMSK0 |= (1 << OCIE0A);
	//output to OC0A, toggle
	TCCR0A |= (1 << COM0A0);
	sei();          // enable glob. int.
}

void Timer0_ovf_start(){
	cli();          
	TCCR0A = 0;     
	TCCR0B = 0;
	TIMSK0 = 0;     
	// mode = 0 by default, we do need to set it
	TCCR0B |= PRESCALE;
	//output to OC0A, toggle
	TCCR0A |= (1 << COM0A0);
	// 
	TIMSK0 |= (1 << TOIE0);
	sei(); // 
}

void Timer0_fastpwm_start(uint8_t duty){
	
	cli();          
	TCCR0A = 0;     
	TCCR0B = 0;  
	TIMSK0 = 0;   //
	// set value for PWM, 8bit  
	OCR0A = (255*duty)/100;
	// fastpwm mode:
	TCCR0A |= (1 << WGM01);
	TCCR0A |= (1 << WGM00);
	
	TCCR0B |= PRESCALE;
	// interrupt:
	TIMSK0 |= (1 << TOIE0);
	//output to pin OC0A
	TCCR0A |= (1 << COM0A1);
	
	sei();          // global int ennable
}

void Timer0_Stop(){
	TCCR0B=0; //just set prescale to 0
}

void Timer1_cmp_A_start(uint16_t value_to_compare){
	
	cli();          // 
	TCCR1A = 0;     // 
	TCCR1B = 0; //
	TIMSK1 = 0;    //
	// compare reg.A
	OCR1A = value_to_compare;

	TCCR1B |=(1<<WGM12);

	TCCR1B |= PRESCALE;

	TIMSK1 |= (1 << OCIE1A);
	
	TCCR1A |= (1 << COM1A0);
	sei();          // 
}
void Timer1_cmp_B_start(uint16_t value_to_compare){
	//CMP ale normální režim... pouze povolíme interrupt na compare B
	cli();          // 
	TCCR1A = 0;     // 
	TCCR1B = 0; //
	TIMSK1 = 0;    //
	
	OCR1B = value_to_compare;
	
	TCCR1B |= PRESCALE;
	
	TIMSK1 |= (1 << OCIE1B);
	//output to OC1B, toggle
	TCCR1A |= (1 << COM1B0);
	
	sei();          // povolit globalni preruseni
}

//we can also start counting from some value, some initial value, we can to calculate it from the rest...  
void Timer1_ovf_start(uint16_t rest_value){
	cli();          // 
	TCCR1A = 0;     // 
	TCCR1B = 0;
	TIMSK1 = 0;     //
	
	// initial value is max-rest
	TCNT1=65535-rest_value;
	
	TCCR1B |= PRESCALE;
	
	TIMSK1 |= (1 << TOIE1);
	sei(); // 
	}	  

void Timer1_fastpwm_start(uint8_t duty){
	cli();          // 
	TCCR1A = 0;     //
	TCCR1B = 0;  
	TIMSK1 = 0;   //
	// nastavit hodnotu pro PWM  
	OCR1A = (255*duty)/100;
	// fastpwm mod:
	TCCR1A |= (1 << WGM11);
	TCCR1A |= (1 << WGM10);
	// 1024 preddelicka:
	TCCR1B |= PRESCALE;
	// povolit preruseni:
	TIMSK1 |= (1 << TOIE1);
	//vystup na pin OC0A
	TCCR1A |= (1 << COM1A1);
	
	sei();          // povolit globalni preruseni
}

void Timer1_Stop(){
	TCCR1B=0;
}
void Timer2_cmp_start(uint8_t compare_value){
	
	cli();          // zakazat preruseni
	TCCR2A = 0;     // vycistit kontrolni registry
	TCCR2B = 0; //
	TIMSK2 = 0;    //
	// nastavit hodnotu pro porovnani
	OCR2A = compare_value;
	// CTC mod:
	TCCR2A |= (1 << WGM11);
	// 1024 preddelicka:
	//TCCR0B |= (1 << CS10);
	//TCCR0B |= (1 << CS12);
	TCCR2B |= 5;
	// povolit preruseni:
	TIMSK2 |= (1 << OCIE2A);
	//vystup na pin OC1A, toggle
	TCCR2A |= (1 << COM2A0);
	sei();          // povolit globalni preruseni
}

void Timer2_ovf_start(){
	cli();          // zakazat preruseni
	TCCR2A = 0;     // vycistit kontrolni registry
	TCCR2B = 0;
	TIMSK2 = 0;     //
	//Nastav_timer(mils);
	// nastavit pocatecni hodnotu
	//TCNT0=255-Nastav_timer(mils);
	// 1024 preddelicka:
	TCCR2B |= 5;
	//vystup na pin OC0A, toggle
	TCCR2A |= (1 << COM2A0);
	// povolit preruseni:
	TIMSK2 |= (1 << TOIE2);
	sei(); // povolit globalni preruseni
}

void Timer2_fastpwm_start(uint8_t duty){
	cli();          // zakazat preruseni
	TCCR2A = 0;     // vycistit kontrolni registry
	TCCR2B = 0;
	TIMSK2 = 0;   //
	// nastavit hodnotu pro PWM
	OCR2A = (255*duty)/100;
	// fastpwm mod:
	TCCR2A |= (1 << WGM21);
	TCCR2A |= (1 << WGM20);
	// 1024 preddelicka:
	TCCR2B |= 5;
	// povolit preruseni:
	TIMSK2 |= (1 << TOIE2);
	//vystup na pin OC2A
	TCCR2A |= (1 << COM2A1);
	
	sei();          // povolit globalni preruseni
}

void Timer2_Stop(){
	TCCR2B=0;
}

void Timer3_cmp_start(uint16_t compare_value){
	
	cli();          // zakazat preruseni
	TCCR3A = 0;     // vycistit kontrolni registry
	TCCR3B = 0; //
	TIMSK3 = 0;    //
	// nastavit hodnotu pro porovnani
	OCR3A = compare_value;
	// CTC mod:
	TCCR3A |= (1 << WGM31);
	// 1024 preddelicka:
	//TCCR0B |= (1 << CS10);
	//TCCR0B |= (1 << CS12);
	TCCR3B |= PRESCALE;
	// povolit preruseni:
	TIMSK3 |= (1 << OCIE3A);
	//vystup na pin OC1A, toggle
	//TCCR3A |= (1 << COM3A0);
	sei();          // povolit globalni preruseni
}
void Timer3_ovf_start(uint16_t rest_value){
	cli();          // zakazat preruseni
	TCCR3A = 0;     // vycistit kontrolni registry
	TCCR3B = 0;
	TIMSK3 = 0;     //
	//Nastav_timer(mils);
	// nastavit pocatecni hodnotu
	TCNT3=65535-rest_value;
	// 1024 preddelicka:
	TCCR3B |= PRESCALE;
	//vystup na pin OC0A, toggle
	//TCCR1A |= (1 << COM1A0);
	// povolit preruseni:
	TIMSK3 |= (1 << TOIE3);
	sei(); // povolit globalni preruseni
}
void Timer3_Stop(){
	TCCR3B=0;
}