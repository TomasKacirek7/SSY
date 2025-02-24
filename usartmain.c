/*
 * usartmain.c
 *
 * Created: 2/24/2025 13:12:24
 *  Author: Student
 */ 
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "makra.h"  // P?ipojení makra pro ovládání LED a bitových operací

#define BUTTON_PIN    PD2  // Pin pro tla?ítko (INT0)

void USART_Init(uint16_t baud) {
	uint16_t ubrr = F_CPU / 16 / baud - 1;
	UBRR1H = (uint8_t)(ubrr >> 8);
	UBRR1L = (uint8_t)ubrr;
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	UCSR1C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_Transmit(uint8_t data) {
	while (!(UCSR1A & (1 << UDRE1))) { }
	UDR1 = data;
}

void USART_SendString(const char *text) {
	while (*text) {
		USART_Transmit(*text++);
	}
}

/* Funkce pro p?ijetí jednoho znaku */
uint8_t USART_Receive(void) {
	while (!(UCSR1A & (1 << RXC1))) { }
	return UDR1;
}

/* Funkce pro vymazání obrazovky */
void clear_screen(void) {
	USART_SendString("\033[2J");  // Escape sekvence pro vymazání obrazovky
}

/* Funkce pro nastavení ?ervené barvy textu */
void set_red(void) {
	USART_SendString("\033[31m");  // Escape sekvence pro ?ervenou barvu
}

/* Funkce pro resetování barvy textu */
void reset_color(void) {
	USART_SendString("\033[0m");  // Escape sekvence pro resetování barvy
}

void LED_Blink(void) {
	DDRB = 0b01110000;  // Nastavení pin? 4, 5, 6 jako výstupy
	setbit(PORTB, 4);
	_delay_ms(500);
	clearbit(PORTB, 4);
	_delay_ms(500);
	setbit(PORTB, 5);
	_delay_ms(500);
	clearbit(PORTB, 5);
	_delay_ms(500);
	setbit(PORTB, 6);
	_delay_ms(500);
	clearbit(PORTB, 6);
	_delay_ms(500);
}

// P?erušení pro detekci stisku tla?ítka
ISR(INT0_vect) {
	USART_Transmit('1');  // Odeslání znaku '1' p?i stisku tla?ítka
}

int main(void) {
	// Inicializace USART s baudrate 38400
	USART_Init(38400);

	// Povoleni globalnich preruseni
	sei();

	// Nastavení p?erušení pro tla?ítko (INT0)
	EIMSK |= (1 << INT0);    // Povolení p?erušení INT0
	EICRA |= (1 << ISC01);   // Nastavení pro spušt?ní p?erušení p?i poklesu signálu (tla?ítko)
	
	// Nastavení ?ervené barvy pro celý text
	set_red();

	// Zobrazení MENU s popisem voleb
	USART_SendString("MENU:\r\n");
	USART_SendString("0: Ukonceni programu\r\n");
	USART_SendString("1: Vypis male abecedy\r\n");
	USART_SendString("2: Vypis velke abecedy\r\n");
	USART_SendString("3: Blikani LED 3x\r\n");
	USART_SendString("4: Vymazat obrazovku a zobrazit MENU\r\n");

	while (1) {
		uint8_t recv = USART_Receive();  // Cekáme na p?íkaz od uživatele

		// Reakce na vstupy
		switch (recv) {
			case '0':
			// Ukon?ení programu
			USART_SendString("Program ukoncen.\r\n");
			return 0;
			
			case '1':
			// Vypis male abecedy
			for (char c = 'a'; c <= 'z'; c++) {
				USART_Transmit(c);
			}
			USART_Transmit('\r');
			USART_Transmit('\n');
			break;
			
			case '2':
			// Vypis velke abecedy
			for (char c = 'A'; c <= 'Z'; c++) {
				USART_Transmit(c);
			}
			USART_Transmit('\r');
			USART_Transmit('\n');
			break;
			
			case '3':
			// Blikání LED
			USART_SendString("Blikani LED.\r\n");
			LED_Blink();
			break;
			
			case '4':
			// Vymazání obrazovky a zobrazení menu
			clear_screen();
			USART_SendString("MENU:\r\n");
			USART_SendString("0: Ukonceni programu\r\n");
			USART_SendString("1: Vypis male abecedy\r\n");
			USART_SendString("2: Vypis velke abecedy\r\n");
			USART_SendString("3: Blikani LED 3x\r\n");
			USART_SendString("4: Vymazat obrazovku a zobrazit MENU\r\n");
			break;

			default:
			// Ošet?ení neznámého vstupu
			USART_SendString("Neznamy vstup, zadejte hodnotu 0-4.\r\n");
			break;
		}
	}

	return 0;
}
