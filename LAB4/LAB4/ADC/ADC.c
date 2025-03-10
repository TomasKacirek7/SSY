/*
 * ADC.c
 *
 * Created: 25.02.2022 8:53:04
 *  Author: Ondra
 */ 
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "../makra.h"
#include "ADC.h"
void ADC_Init(uint8_t prescale,uint8_t uref) {
	ADMUX=0;
	ADCSRA=0;
	ADCSRB=0;
	ADCSRC=0;
	ADCSRA |= (prescale<<ADPS0);
	ADMUX |= (uref<<REFS0);
	//Kontrola jesli jsou napeti OK
	sbi(ADCSRA,ADEN);
	while(!(ADCSRB & 0x80));
	while(!(ADCSRB & 0x20));
}
void ADC_stop(void){
	//cbi(ADCSRA,ADEN);
	ADMUX=0;
	ADCSRA=0;
	ADCSRB=0;
	ADCSRC=0;
}
uint16_t ADC_get(uint8_t chan) {
	uint16_t tmp=0;
	if (tbi(ADCSRA,ADEN)) //kontrola jesti je ADC povolen...
	{
		//smazat MUX
		ADMUX &= ~(31 << MUX0); // copak ten zapis znamena?
		ADCSRB&= ~(1 << MUX5);
		//nastavit spravny kanal
		ADMUX |= (chan<<MUX0);
		ADCSRA |= (1<<ADSC); // spustit konverzi
		while((tbi(ADCSRA,ADSC))){} //pockat nez skonci, sice polling, ale proc ne, je to rychle a nezavysle
		//pomocna = ADCL | (ADCH << 8);
		tmp=ADC;
		ADCSRA |= (1<<ADIF); //vynulovat priznak konverze - tim ze zapiseme 1, viz datasheet
		return tmp;
	} 
	else
	{
		return 0;
	}
	
}
uint16_t ADC_readTemp(){
	uint16_t tmp=0;
	ADMUX=0;
	ADCSRA=0;
	ADCSRB=0;
	ADCSRC=0;
	ADCSRC = 10<<ADSUT0; // set start-up time
	ADCSRB = 1<<MUX5; // set MUX5 first
	ADMUX = (3<<REFS0) + (9<<MUX0); // store new ADMUX, 1.6V AREF
	// switch ADC on, set prescaler, start conversion
	ADCSRA = (1<<ADEN) + (1<<ADSC) + (4<<ADPS0);
	while((tbi(ADCSRA,ADSC))){} //pockat nez skonci
	tmp=ADC;
	//ADCSRA |= (1<<ADIF);
	return tmp;
	
}
void ADC_Start_per(uint8_t prescale,uint8_t uref,uint8_t chan,uint8_t source){
	ADMUX=0;
	ADCSRA=0;
	ADCSRB=0;
	ADCSRC=0;
	ADCSRA |= (prescale<<ADPS0);
	ADMUX |= (uref<<REFS0);
	//Kontrola jesli jsou napeti OK
	sbi(ADCSRA,ADEN);
	while(!(ADCSRB & 0x80));
	while(!(ADCSRB & 0x20));
	//nastavit kanal
	ADMUX |= (chan<<MUX0);
	ADCSRB |= (source<<ADTS0);
	ADCSRA |= (1<<ADATE);
	ADCSRA |= (1<<ADIE); //Povolit preruseni, prece to nebudeme porad kontrolovat pollingem
}