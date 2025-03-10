/*
 * LAB1.c
 *
 * Created: 02.02.2020 9:01:38
 * Author : Ondra
 */ 

/************************************************************************/
/* INCLUDE                                                              */
/************************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "makra.h"
#include "uart/uart.h"
#include "Timers/Timers.h"
#include "ADC/ADC.h"
#include "I2C/I2C.h"
#include "Atmel/atmel.h"

/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/

#define STEP 10
// F_CPU definovano primo v projektu!!

/************************************************************************/
/* VARIABLES                                                            */
/************************************************************************/

FILE uart_str = FDEV_SETUP_STREAM(printCHAR, NULL, _FDEV_SETUP_RW);// soubor pro stdout

volatile int pruchod=0;  
volatile uint8_t klik=0; 
volatile char recv;
volatile uint8_t char_to_process=0;
volatile uint8_t menu_level=0;

char text[]="label";

volatile uint8_t duty=0; // PWM st?ída
volatile uint8_t auto_reg=0;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void board_init();
void menu1();
void menu3();
void handle_menu1();
void handle_menu3();

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void board_init(){
	cli(); // disable interrupts
	
	UART_init(38400); // baudrate 38400b/s
	UCSR1B |= (1 << RXCIE1);// UART receive interrupt enable
	stdout = &uart_str; // standardní výstup
	
	// LEDky
	sbi(DDRB,4);
	sbi(DDRB,5);
	sbi(DDRB,6);
	sbi(DDRE,3);
	
	// Všechny zapneme
	LED0ON;
	LED1ON;
	LED2ON;
	LED3ON;
	
	Timer2_fastpwm_start(100);
	
	// Tlacitka vstup
	cbi(DDRE,4);
	// Tlacitka pull-up
	sbi(PORTE,4);
    
	sbi(EIMSK,4); // povolit preruseni INT5 - tlacitko button0 
	sbi(EICRB,ISC41); // nastupna hrana
	sbi(EICRB,ISC40);
	
	// Inicializace I2C
	i2cInit();
	
	sei(); // enable interrupts
}

void menu1(){
	UART_SendChar(27);
	UART_SendString("[2J");// clear and home
	UART_SendChar(27);
	UART_SendString("[0;32;40m");// barva pozadi a textu
	
	printf("-------------------------------------\n\r");
	printf("MENU: \n\r");
	printf("0 - Konec\n\r");
	printf("1 - Rozsvit LED1 \n\r");
	printf("2 - Zhasni LED1 \n\r");
	printf("3 - Jas LED0 \n\r");
	printf("4 - Start/stop Blikani LED2 \n\r");
	printf("5 - Teplota procesoru \n\r");
	printf("6 - Automaticky jas \n\r");
	printf("7 - Teplota z AT30TSE758 \n\r");
	printf("8 - Zapis do EEPROM \n\r");
	printf("9 - Cteni z EEPROM \n\r");
	printf("-------------------------------------\n\r");
	
	UART_SendChar(27);
	UART_SendString("[0;37;40m");// reset
}

void handle_menu1(){
	float teplota = 0;
	uint8_t eeprom_data = 0;
	
	switch (recv) {
		case '0':
			printf("\r\nEnd... \r\n");
			menu1();
			break;
		
		case '1':
			LED1ON;
			break;
		case '2':
			LED1OFF;
			break;
		case '3':
			menu_level = 3;
			menu3();
			break;
		case '4':
			if (TCCR1B) {
				printf("\r\nKonec blikani \r\n");
				Timer1_Stop();
			} else {
				printf("\r\nStart blikani \r\n");
				Timer1_cmp_A_start(3000);
			}
			break;
		case '5':
			if (auto_reg == 0) {
				teplota = (1.13 * ADC_readTemp()) - 272.8;
				printf("\n\rTeplota je: %.2f C\n\r", teplota);
				ADC_stop();
			} else {
				teplota = (1.13 * ADC_readTemp()) - 272.8;
				printf("\n\rTeplota je: %.2f C\n\r", teplota);
				ADC_stop();
				ADC_Start_per(4,2,3,3);
			}
			break;
		case '6':
			printf("\n\rAutomaticky jas: ");
			if (auto_reg == 0) {
				printf(" zapnuto\r\n");
				auto_reg = 1;
				ADC_Start_per(4,2,3,3);
				Timer0_cmp_start(100);
			} else {
				printf(" vypnuto\r\n");
				auto_reg = 0;
				ADC_stop();
				Timer0_Stop();
			}
			break;
		case '7': // ?tení teploty z AT30TSE758
			teplota = at30_read_temp();
			if (teplota != -1.0) {
				printf("\n\rTeplota z AT30TSE758: %.2f C\n\r", teplota);
			} else {
				printf("\n\rChyba p?i ?tení teploty\n\r");
			}
			break;
		case '8': // Zápis do EEPROM
			if (at30_eeprom_write(0x0000, 0x55)) {
				printf("\n\rData uložena do EEPROM\n\r");
			} else {
				printf("\n\rChyba zápisu do EEPROM\n\r");
			}
			break;
		case '9': // ?tení z EEPROM
			if (at30_eeprom_read(0x0000, &eeprom_data)) {
				printf("\n\rEEPROM hodnota: 0x%X\n\r", eeprom_data);
			} else {
				printf("\n\rChyba ?tení z EEPROM\n\r");
			}
			break;
		default:
			printf("\r\nUnknown command.. \r\n");
	}
	char_to_process = 0;
}

/************************************************************************/
/* MAIN                                                                 */
/************************************************************************/
int main(void) {  
	board_init();
	_delay_ms(100);
	menu1();
    _delay_ms(100);
	
	int i = 0;
	while (1) {
	    if (char_to_process == 1) {
		    switch (menu_level) {
			    case 0:
				    handle_menu1();
				    break;
			    case 3:
				    handle_menu3();
				    break;
		    }
	    }
	    _delay_ms(100);
		i++;
    }
}

/************************************************************************/
/* INTERRUPTS                                                           */
/************************************************************************/

ISR(INT4_vect){
	_delay_ms(200);
	klik++;
	LED3CHANGE;
	sbi(EIFR,INT4);
}

ISR(USART1_RX_vect) {
	recv = UART_GetChar();
	UART_SendChar(recv);
	char_to_process = 1;
}
