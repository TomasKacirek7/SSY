/*
 * LAB1.c
 *
 * Created: 17.02.2025 12.50
 * Author : 241044
 */ 

/************************************************************************/
/* INCLUDE                                                              */
/************************************************************************/
#include <avr/io.h>
#include <util/delay.h>
#include "libs/libprintfuart.h"
#include <stdio.h>
/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/
#define HODNOTAX 10
#define HODNOTAY 2
#define	ODECET (HODNOTAX - HODNOTAY)
// F_CPU definovano primo v projektu!!! Debug->Properties->Toolchain->Symbols
// Konstanty pro ur?ení typu písma
#define UPPER_CASE 1
#define NORMAL_CASE 0

// Konstanty pro sm?r výpisu
#define DIRECTION_UP 1
#define DIRECTION_DOWN 0

// Maximální velikost pole pro abecedu
#define MAX_SIZE 26
/************************************************************************/
/* VARIABLES                                                            */
/************************************************************************/
char alphabet[MAX_SIZE * 2];  // Zahrnuje A-Z a a-z
//musime vytvorit soubor pro STDOUT
FILE uart_str = FDEV_SETUP_STREAM(printCHAR, NULL, _FDEV_SETUP_RW);

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void board_init();

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void board_init(){
	UART_init(38400); //nastaveni rychlosti UARTu, 38400b/s
	stdout = &uart_str; //presmerovani STDOUT
}

int main(void)
{
	board_init();
	_delay_ms(1000);
	printf("Hello word \n\r");
	_delay_ms(100);
	
	int a = HODNOTAX; // Nastavení po?áte?ní hodnoty prom?nné a
	
	#ifdef ODECET
	a = a - HODNOTAY; // Ode?ítáme, pokud je definována direktiva ODECET
	#endif
	
	printf("Hodnota promenne a = %d \n\r", a); // Vytiskneme výslednou hodnotu
	
	// Deklarace dvou promenych typu unsigned char
	unsigned char prvni = 255;
	unsigned char druhy = 255;
	
	// Deklarace treti promenne typu unsigned int pro soucet
	unsigned int soucet = (unsigned int)prvni + (unsigned int)druhy; // Pouziti sirsiho datoveho typu

	// Vystup souctu
	printf("Soucet je: %u\n\r", soucet);

	// Pokud chcete soucet zpet do unsigned char (a zajistit, ze hodnoty budou v rozsahu 0-255)
	unsigned char soucet_char = (unsigned char)soucet;
	printf("Soucet v typu unsigned char: %u\n\r", soucet_char);
	
	// Deklarace promenne s hodnotou 24
	int promenna = 24;
	
	// Posunuti vpravo o 3, odeceteni 1
	promenna = (promenna >> 3) - 1;
	
	// Vymaskovani s 0x2
	promenna = promenna & 0x2;
	
	// Vystup
	printf("Vysledek je: %d\n", promenna);  // Ocekavany vysledek je 2
	
	printf("\n\r");
	
	// Deklarace promenne H s hodnotou 200
    int H = 200;
    char str[80];  // Deklarace pole pro retezec

    // Pouziti funkce sprintf pro slouceni retezce a hodnoty promenne
    sprintf(str, "Hodnota=%d", H);  // Formatovani retezce s hodnotou promenne 'H'
    
    // Vytisknuti vysledku
    printf("Vysledek pomoci sprintf: %s\n\r", str);

    // Alternativni metoda s pouzitim string.h a stdlib.h
    char temp[80];  // Do?asný retezec pro hodnotu
    snprintf(temp, sizeof(temp), "%d", H);  // Prevedeni cisla na retezec

    // Slouceni retezce "Hodnota=" a prevedeneho cisla
    char result[80];
    strcpy(result, "Hodnota=");  // Zkopirujeme pocatecni text
    strcat(result, temp);  // Pripojime prevedenou hodnotu

    // Vytisknuti vysledku
    printf("Vysledek pomoci string.h: %s\n\r", result);
	
	printf("\n\r");
	
	// Generování abecedy A-Z a a-z
	int i;
	for (i = 0; i < 26; i++) {
		alphabet[i] = 'A' + i;  // Velká písmena A-Z
		alphabet[i + 26] = 'a' + i;  // Malá písmena a-z
	}

	// Funkce pro p?evod písmen na velká nebo malá
	for (i = 0; i < 26; i++) {
		alphabet[i] = toupper(alphabet[i]);  // P?evod na velká písmena
		alphabet[i + 26] = tolower(alphabet[i + 26]);  // P?evod na malá písmena
	}

	// Výpis písmen vzestupn?
	printf("Pismena vzestupne:\n\r");
	for (i = 0; i < MAX_SIZE * 2; i++) {
		printf("%c ", alphabet[i]);
	}

	printf("\n\r");

	// Výpis písmen sestupn?
	printf("Pismena sestupne:\n\r");
	for (i = MAX_SIZE * 2 - 1; i >= 0; i--) {
		printf("%c ", alphabet[i]);
	}

	printf("\n\r");
	
	// Deklarace prom?nné typu int
    int num = 42;  // Napl?te hodnotu 42

    // Deklarace ukazatele na tuto prom?nnou
    int *ptr = &num;  // Ukazatel ptr ukazuje na num

    // Vypis hodnoty pomocí ukazatele a adresy v pam?ti
    printf("Hodnota promenne num: %d\n\r", *ptr);  // Dereference ukazatele, abychom získali hodnotu
    printf("Adresa promenne num: %p\n\r", (void*)ptr);  // Vytiskneme adresu v pam?ti, kde je num uložen

	
	return 0; // P?idání návratové hodnoty z funkce main
}

