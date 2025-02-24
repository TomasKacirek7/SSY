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

void USART_Init(uint16_t baud) {
	/* Spocteni hodnoty pro UBRR podle vzorce */
	uint16_t ubrr = F_CPU / 16 / baud - 1;
	
	/* Nastaveni baudrate */
	UBRR1H = (uint8_t)(ubrr >> 8);
	UBRR1L = (uint8_t)ubrr;
	
	/* Povoleni prijimace a vysilace */
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	
	/* Nastaveni formatu ramece: Asynchronni, bez parity, 1 stopbit, 8 datovych bitu */
	UCSR1C = (1 << UCSZ01) | (1 << UCSZ00);
}

/* Funkce pro odeslani jednoho znaku */
void USART_Transmit(uint8_t data) {
	/* Cekani na volny vysilaci buffer */
	while (!(UCSR1A & (1 << UDRE1))) {
		// Cekame, dokud nebude buffer priprav
	}
	
	/* Zapsani dat do registru pro odeslani */
	UDR1 = data;
}

/* Funkce pro prijeti jednoho znaku */
uint8_t USART_Receive(void) {
	/* Cekani na prijeti dat */
	while (!(UCSR1A & (1 << RXC1))) {
		// Cekame, dokud nebude prijata data
	}
	
	/* Ziskani a vraceni prijateho datoveho znaku */
	return UDR1;
}

/* Funkce pro odeslani retezce */
void USART_SendString(const char *text) {
	while (*text) {
		USART_Transmit(*text++);  // Odeslani kazdeho znaku a posun ukazatele
	}
}

/* Funkce pro ziskani jednoho znaku (s blokovanim) */
uint8_t UART_GetChar(void) {
	return USART_Receive();
}

/* Funkce pro blikani LED (napr. LED na pinu PD6) */
void LED_Blink(void) {
	// Nastavime PD6 jako vystup
	DDRD |= (1 << PD6);
	
	for (int i = 0; i < 3; i++) {
		PORTD |= (1 << PD6);  // LED ON
		_delay_ms(500);        // Cekani 500 ms
		PORTD &= ~(1 << PD6);  // LED OFF
		_delay_ms(500);        // Cekani 500 ms
	}
}

int main(void) {
	// Inicializace USART s baudrate 38400
	USART_Init(38400);
	
	// Nastaveni LED pin? jako vystupy
	DDRB |= (1 << PB4) | (1 << PB5) | (1 << PB6);  // LED0, LED1, LED2
	DDRE |= (1 << PE3);  // LED3
	
	// Povoleni globalnich preruseni
	sei();
	
	// Zobrazeni MENU s popisem voleb
	USART_SendString("MENU:\n\r");
	USART_SendString("0: Ukonceni programu\n\r");
	USART_SendString("1: Vypis male abecedy\n\r");
	USART_SendString("2: Vypis velke abecedy\n\r");
	USART_SendString("3: Blikani LED 3x\n\r");

	while (1) {
		uint8_t recv = UART_GetChar();  // Cekame na vstup od uzivatele
		
		// Reakce na vstupy
		switch (recv) {
			case '0':
			// Ukonceni programu
			USART_SendString("Program ukoncen.\n\r");
			return 0;
			
			case '1':
			// Vypis male abecedy
			for (char c = 'a'; c <= 'z'; c++) {
				USART_Transmit(c);
			}
			USART_Transmit('\n');
			break;
			
			case '2':
			// Vypis velke abecedy
			for (char c = 'A'; c <= 'Z'; c++) {
				USART_Transmit(c);
			}
			USART_Transmit('\n');
			break;
			
			case '3':
			// Blikani LED
			USART_SendString("Blikani LED.\n\r");
			LED_Blink();
			break;
			
			default:
			// Osetreni neznameho vstupu
			USART_SendString("Neznamy vstup, zadejte hodnotu 0-3.\n \r");
			break;
		}
		
		// Po kazdem zpracovani vypiseme MENU
		USART_SendString("MENU:\n");
		USART_SendString("0: Ukonceni programu\n\r");
		USART_SendString("1: Vypis male abecedy\n\r");
		USART_SendString("2: Vypis velke abecedy\n\r");
		USART_SendString("3: Blikani LED 3x\n\r");
	}
	
	return 0;
}
