/*
 * CV1 - LEDKY.c
 *
 * Created: 2/17/2025 13:57:19
 * Author : Student
 */ 

#include <avr/io.h>
#include <avr/delay.h>

// Makra pro nastavení, zrušení a XOR bitu
#define setbit(port, bit)   (port |= (1 << bit))
#define clearbit(port, bit) (port &= ~(1 << bit))
#define xorbit(port, bit)   (port ^= (1 << bit))
int main(void)
{
		// binary notation
	//DDRB = 0b01000000;
	//PORTB = 0b01000000;
	// equivalent hex notation
	//DDRG = 0x02;
	//PORTG = 0x02;
	// equivalent decimal notation
	//DDRG = 2;
	//PORTG = 2;
	//while(1)
	 //{
	//PORTB = 0b01000000;
	//_delay_loop_2(0xFFFF);
	//PORTB = 0b00000000;
	//_delay_loop_2(0xFFFF);
	// Nastavení pin? 4, 5, 6 na portu B jako výstupy (DDRB = 0b01111000)
	DDRB = 0b01110000;  // Nastaví piny 4, 5, 6 jako výstupy (pro LED na bitech 4, 5, 6)

	while(1)
	{
		// ** Binární zápis pro zapnutí LED diod **

		// LED0 (bit 4) zapnutí
		PORTB = 0b00010000;  // Zapnutí LED0 na pinu 4
		_delay_loop_2(0xFFFF);  // Zpožd?ní

		// LED1 (bit 5) zapnutí
		PORTB = 0b00100000;  // Zapnutí LED1 na pinu 5
		_delay_loop_2(0xFFFF);  // Zpožd?ní

		// LED2 (bit 6) zapnutí
		PORTB = 0b01000000;  // Zapnutí LED2 na pinu 6
		_delay_loop_2(0xFFFF);  // Zpožd?ní

		// ** Dekadický zápis pro zapnutí LED diod **

		PORTB = 16;  // Zapnutí LED0 (bit 4) v dekadickém zápisu
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		PORTB = 32;  // Zapnutí LED1 (bit 5) v dekadickém zápisu
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		PORTB = 64;  // Zapnutí LED2 (bit 6) v dekadickém zápisu
		_delay_loop_2(0xFFFF);  // Zpožd?ní

		// ** Hexadecimální zápis pro zapnutí LED diod **

		PORTB = 0x10;  // Zapnutí LED0 (bit 4) v hexadecimálním zápisu
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		PORTB = 0x20;  // Zapnutí LED1 (bit 5) v hexadecimálním zápisu
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		PORTB = 0x40;  // Zapnutí LED2 (bit 6) v hexadecimálním zápisu
		_delay_loop_2(0xFFFF);  // Zpožd?ní

		// ** Použití maker pro zapnutí/vypnutí LED diod **
		
		setbit(PORTB, 4);  // Zapnutí LED0 pomocí makra setbit (bit 4)
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		setbit(PORTB, 5);  // Zapnutí LED1 pomocí makra setbit (bit 5)
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		setbit(PORTB, 6);  // Zapnutí LED2 pomocí makra setbit (bit 6)
		_delay_loop_2(0xFFFF);  // Zpožd?ní

		clearbit(PORTB, 4);  // Vypnutí LED0 pomocí makra clearbit (bit 4)
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		clearbit(PORTB, 5);  // Vypnutí LED1 pomocí makra clearbit (bit 5)
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		clearbit(PORTB, 6);  // Vypnutí LED2 pomocí makra clearbit (bit 6)
		_delay_loop_2(0xFFFF);  // Zpožd?ní

		// ** Použití XOR pro zm?nu stavu LED diod **
		xorbit(PORTB, 4);  // Zm?na stavu LED0 pomocí makra xorbit (bit 4)
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		xorbit(PORTB, 5);  // Zm?na stavu LED1 pomocí makra xorbit (bit 5)
		_delay_loop_2(0xFFFF);  // Zpožd?ní
		xorbit(PORTB, 6);  // Zm?na stavu LED2 pomocí makra xorbit (bit 6)
		_delay_loop_2(0xFFFF);  // Zpožd?ní
	}
	 


	
}

