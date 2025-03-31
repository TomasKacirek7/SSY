#include <avr/io.h>
#include <util/delay.h>
#include "libs/libprintfuart.h"
#include <stdio.h>

// Parametry pro Diffie-Hellman (stejne jako u Alice)
#define P 23            // Stejne prvocislo
#define G 5             // Stejny generator

FILE uart_str = FDEV_SETUP_STREAM(printCHAR, NULL, _FDEV_SETUP_RW);

// Prototypy funkci
void board_init();
uint32_t mod_exp(uint32_t base, uint32_t exp, uint32_t mod);
void print_result(const char* label, uint32_t value);

// Inicializace serioveho rozhrani
void board_init() {
    UART_init(38400);   // Stejna rychlost jako u Alice
    stdout = &uart_str; // Presmerovani STDOUT
}

// Funkce pro modularni exponenciaci (a^b mod p)
uint32_t mod_exp(uint32_t base, uint32_t exp, uint32_t mod) {
    uint32_t result = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp & 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

// Vypis vysledku na seriovy port
void print_result(const char* label, uint32_t value) {
    printf("%s %lu\n\r", label, value);
}

int main(void) {
    board_init();
    _delay_ms(1000);

    // Uvodni text
    printf("Diffie-Hellman Exchange - Bob\n\r");

    // Parametry (stejne jako u Alice)
    printf("P (prvocislo): %u\n\r", P);
    printf("G (generator): %u\n\r", G);

    // Tajny klic pro Boba
    uint32_t secret_b = 15;  // Tajny klic pro Boba
    printf("Tajny klic Boba: %lu\n\r", secret_b);

    // Vypocet verejneho klice Boba
    uint32_t public_B = mod_exp(G, secret_b, P);
    print_result("Verejny klic Boba:", public_B);

    // Cekani na verejny klic Alice (v realnem pouziti by se cekalo na UART)
    printf("Cekam na verejny klic Alice...\n\r");
    // Simulace prijmu verejneho klice Alice (pro testovani nastaveno na 8)
    uint32_t public_A = 8;
    printf("Prijaty verejny klic Alice: %lu\n\r", public_A);

    // Vypocet spolecneho tajneho klice
    uint32_t shared_key_B = mod_exp(public_A, secret_b, P);
    print_result("Spolecny tajny klic Boba:", shared_key_B);

    // Konec programu
    printf("\n\r");

    return 0;
}