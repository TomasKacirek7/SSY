/*
 * LAB2-a.c
 *
 * Created: 15.02.2023 12:18:20
 * Author : 241044 - prevzato z LAB2 reseni
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
/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/

// F_CPU definovano primo v projektu!!!Defined in project properties

/************************************************************************/
/* VARIABLES                                                            */
/************************************************************************/

FILE uart_str = FDEV_SETUP_STREAM(printCHAR, NULL, _FDEV_SETUP_RW); // soubor pro stdout

volatile int pruchod=0;  //
volatile uint8_t klik=0; //
volatile char recv;
volatile uint8_t state=0;
volatile uint8_t pwm_duty_cycle = 0;  // po?áte?ní st?ída (50%)


char text[]="label"; // = 'l''a''b''e''l''/0'

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void board_init();
void menu1();
void prvniUkoly();
void Timer1_cmp_start(uint16_t porovnani);
void Timer1Stop(void);

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/

void board_init(){
	cli(); //disable interrupts - defaultne je sice vyple, ale pro jistotu
	
	UART_init(38400); //baudrate 38400b/s
	
	UCSR1B |= (1 << RXCIE1); // UART receive interrupt enable
	stdout = &uart_str; // standardní výstup/std output
	
	//LED0 vystup/output
	sbi(DDRB,4);
	sbi(PORTB,4); // turn the LED0 down

	//LED1
	sbi(DDRB,5);
	sbi(PORTB,5);
	
	//Tlacitka vstup/buttons as input
	cbi(DDRE,4);
	//Tlacitka pull-up
	sbi(PORTE,4);
	
	sbi(EIMSK,4); //povolit preruseni INT5 - tlacitko button0 /INT5 enable
	sbi(EICRB,ISC41); //nastupna hrana /rising edge
	sbi(EICRB,ISC40); //nastupna hrana
	
	sei(); // enable interrupts
}

void menu1(){
	UART_SendChar(27); // escape
	UART_SendString("[2J"); // clear and home
	UART_SendChar(27); // escape
	UART_SendString("[0;32;40m"); // barva pozadi a textu
	
	printf("-------------------------------------\n\r");
	printf("MENU: \n\r");
	printf("0 - Konec\n\r");
	printf("1 - Rozsvit LED1 \n\r");
	printf("2 - Zhasni LED1 \n\r");
	printf("3 - Uprava jasu \n\r");
	printf("4 - Vypis abecedu \n\r");
	printf("5 - Spustit generovani frekvence 2 Hz\n\r");
	printf("-------------------------------------\n\r");
	
	UART_SendChar(27); // escape
	UART_SendString("[0;37;40m"); // reset
}

void menu2(){
	UART_SendChar(27); // escape
	UART_SendString("[2J"); // clear and home
	UART_SendChar(27); // escape
	UART_SendString("[0;32;40m"); // barva pozadi a textu
	
	printf("-------------------------------------\n\r");
	printf("MENU: \n\r");
	printf("+ - Zvyseni jasu\n\r");
	printf("- - Snizeni jasu \n\r");
	printf("Q - Zpet do menu1 \n\r");
	printf("-------------------------------------\n\r");
	
	UART_SendChar(27); // escape
	UART_SendString("[0;37;40m"); // reset
}

void prvniUkoly(){
	DDRB=0b01110000;
	PORTB=0b01110000;
	_delay_ms(500);
	PORTB=0;
	_delay_ms(500);
	
	PORTB=112;
	_delay_ms(500);
	PORTB=0;
	_delay_ms(500);
	
	PORTB=0x70;
	_delay_ms(500);
	PORTB=0;
	_delay_ms(500);
	
	PORTB=(7<<4);
	_delay_ms(500);
	PORTB=0;
	_delay_ms(500);
	
	DDRB=0;
	PORTB=0;
}

void Timer1_cmp_start(uint16_t porovnani){
	cli(); // disable interrupts
	TCCR1A = 0; // vycistit kontrolni registry
	TCCR1B = 0; // 
	TIMSK1 = 0; // 
	OCR1A = porovnani; // nastavime hodnotu pro porovnani
	// CTC mod:
	TCCR1B |= (1 << WGM12); 
	// 1024 prescaler:
	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS12);
	// povolime preruseni:
	TIMSK1 |= (1 << OCIE1A);
	// vystup na pin OC1A (PORTB5, LED1):
	TCCR1A |= (1 << COM1A0);
	sei(); // povolime globalni preruseni
}

void Timer1Stop(void){
	TCCR1B = 0; // zastavit timer
	TIMSK1 = 0; // zakazat preruseni
}

void Timer2_fastpwm_start (uint16_t strida) {
	cli() ; // z a k a z a t p r e r u s e n i
	TCCR2A = 0 ; // v y c i s t i t k o n t r o l n i r e g i s t r y
	TCCR2B = 0 ;
	TIMSK2 = 0 ; //
	// n a s t a v i t hodnotu pro PWM
	OCR2A = (255*strida)/100;
	// fastpwm mod:
	TCCR2A |= ( 1 << WGM21) ;
	TCCR2A |= ( 1 << WGM20) ;
	// 1024 p r e d d eli c k a :
	TCCR2B |= 5 ;
	// p o v o l i t p r e r u s e ni , pokud bude pot ? eba . . . :
	TIMSK2 |= ( 1 << TOIE2 ) ;
	// vystup na pin OC2A
	TCCR2A |= ( 1 << COM2A1) ;
	sei(); // p o v o l i t g l o b a l n i p r e r u s e n i
}

/************************************************************************/
/* MAIN                                                                 */
/************************************************************************/

int main(void){
	prvniUkoly(); // jen blikani LED
	board_init();
	_delay_ms(100);
	menu1();
	_delay_ms(100);
	
	int i = 0;
	while (1) {
		if (state == 1) {
			switch (recv) {
				case '0':
				printf("\r\nKoncim program... \r\n");
				menu1();
				break;
				
				case '1':
				LED1ON;
				printf("\r\n");
				break;
				
				case '2':
				printf("\r\n");
				LED1OFF;
				break;
				
				case '3': // P?echod do menu 2 pro ovládání jasu
				menu2();
				break;
					case '+':
					if (pwm_duty_cycle < 255) {
						pwm_duty_cycle += 10;  // Zvýšení jasu o 10%
					}
					printf("\r\nZvýšení jasu: %d\n\r", pwm_duty_cycle);
					Timer2_fastpwm_start(pwm_duty_cycle);  // Nastavíme PWM
					break;
				
					case '-':
					if (pwm_duty_cycle > 0) {
						pwm_duty_cycle -= 10;  // Snížení jasu o 10%
					}
					printf("\r\nSnížení jasu: %d\n\r", pwm_duty_cycle);
					Timer2_fastpwm_start(pwm_duty_cycle);  // Nastavíme PWM
					break;

					case 'Q':
					menu1();  // Vrátí do hlavního menu
					break;
				
				case '4':
				printf("\r\n");
				for (int i = 65; i < 90; i++) {
					UART_SendChar(i);
				}
				printf("\n\r");
				break;
				
				case '5':
				printf("\r\nSpoustim generovani 2 Hz frekvence na PORTB5...\r\n");
				// 2 Hz frequency: OCR1A value for 16 MHz clock and 1024 prescaler
				Timer1_cmp_start(7812); // OCR1A = (F_CPU / (prescaler * frequency)) - 1 = (16000000 / (1024 * 2)) - 1 = 7812
				break;
				
				default:
				printf("\r\nNeznamy prikaz..\r\n");
				break;
			}
			state = 0;
		}
		_delay_ms(100);
		i++;
	}
}

/************************************************************************/
/* INTERRUPTS                                                           */
/************************************************************************/

ISR(INT4_vect){
	// debounce logic
	cbi(EIMSK,INT4); // disable interrupt INT4
	_delay_ms(200); // wait for debounce
	klik++;
	LED0CHANGE;
	sbi(EIFR,INTF4); // clear the interrupt flag
	sbi(EIMSK,INT4); // enable interrupt INT4
}

ISR(USART1_RX_vect){
	recv = UART_GetChar(); // zbytecné
	// recv = UDR1; // nejlepší zp?sob
	UART_SendChar(recv); // echo, just for safety...
	state = 1; // state machine
}

ISR(TIMER1_COMPA_vect) {
	LED3CHANGE; // toggle LED3 for 2 Hz frequency output
}