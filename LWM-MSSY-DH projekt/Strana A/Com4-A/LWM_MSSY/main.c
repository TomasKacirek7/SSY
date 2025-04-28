/*
 * LWM_MSSY.c
 *
 * Created: 28/4/2025
 * Author : Tomáš Ka?írek, 241044
 */

#include <avr/io.h>
/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include "config.h"
#include "hal.h"
#include "phy.h"
#include "sys.h"
#include "nwk.h"
#include "sysTimer.h"
#include "halBoard.h"
#include "halUart.h"
#include "main.h"
#include "util/delay.h"

/*- Definitions ------------------------------------------------------------*/
#ifdef NWK_ENABLE_SECURITY
#define APP_BUFFER_SIZE    (NWK_MAX_PAYLOAD_SIZE - NWK_SECURITY_MIC_SIZE)
#else
#define APP_BUFFER_SIZE    NWK_MAX_PAYLOAD_SIZE
#endif
/*- Types ------------------------------------------------------------------*/
typedef enum AppState_t
{
	APP_STATE_INITIAL,
	APP_STATE_SEND_PUBLIC_KEY,
	APP_STATE_WAIT_PUBLIC_KEY,
	APP_STATE_CALCULATE_SHARED_SECRET,
	APP_STATE_IDLE,
} AppState_t;

/*- Prototypes -------------------------------------------------------------*/
static void appSendData(void);
static int uart_putchar(char c, FILE *stream);

void board_init();
uint32_t mod_exp(uint32_t base, uint32_t exp, uint32_t mod);
void print_result(const char* label, uint32_t value);

/*- Variables --------------------------------------------------------------*/
static AppState_t appState = APP_STATE_INITIAL;
static SYS_Timer_t appTimer;
static NWK_DataReq_t appDataReq;
static bool appDataReqBusy = false;
static uint8_t appDataReqBuffer[APP_BUFFER_SIZE];
static uint8_t appUartBuffer[APP_BUFFER_SIZE];
volatile uint8_t appUartBufferPtr = 0;
volatile uint8_t received_public_B = 0;
volatile uint32_t shared_secret_a = 0;

uint8_t P = 23; // P musi byt prvocislo
uint8_t G = 5;  // G je primitivni koren modulo P
uint8_t secret_a = 15;  // Tajny klic pro Alici
uint8_t public_A;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static void appDataConf(NWK_DataReq_t *req)
{
appDataReqBusy = false;
(void)req;
}


/*************************************************************************//**
*****************************************************************************/
static void appSendData(void)
{
if (appDataReqBusy || 0 == appUartBufferPtr)
return;

memcpy(appDataReqBuffer, appUartBuffer, appUartBufferPtr);

appDataReq.dstAddr = 1-APP_ADDR;
appDataReq.dstEndpoint = APP_ENDPOINT;
appDataReq.srcEndpoint = APP_ENDPOINT;
appDataReq.options = NWK_OPT_ENABLE_SECURITY;
appDataReq.data = appDataReqBuffer;
appDataReq.size = appUartBufferPtr;
appDataReq.confirm = appDataConf;
NWK_DataReq(&appDataReq);

appUartBufferPtr = 0;
appDataReqBusy = true;
}

/*************************************************************************//**
*****************************************************************************/
void HAL_UartBytesReceived(uint16_t bytes)
{
	for (uint16_t i = 0; i < bytes; i++)
	{
		uint8_t byte = HAL_UartReadByte();

		if (appUartBufferPtr == sizeof(appUartBuffer))
		appSendData();

		if (appUartBufferPtr < sizeof(appUartBuffer))
		{
			appUartBuffer[appUartBufferPtr++] = byte;

			// Vypis prijateho znaku a jeho ciselne hodnoty
			printf("Prijaty znak: '%c' (hodnota: %u)\n\r", byte, byte);
		}
	}

	SYS_TimerStop(&appTimer);
	SYS_TimerStart(&appTimer);
}

/*************************************************************************//**
*****************************************************************************/
static void appTimerHandler(SYS_Timer_t *timer)
{
appSendData();
(void)timer;
}

/*************************************************************************//**
*****************************************************************************/
static bool appDataInd(NWK_DataInd_t *ind)
{
	if (ind->size >= 1)
	{
		received_public_B = ind->data[0] - 'a'; // Prevod z ASCII na cislo
		printf("Prijat verejny klic Boba: %u ('%c')\n\r", received_public_B, ind->data[0]);
		appState = APP_STATE_CALCULATE_SHARED_SECRET; // Presun do stavu vypoctu sdileneho klice
	}
	return true;
}

/*************************************************************************//**
*****************************************************************************/
static void appInit(void)
{
NWK_SetAddr(APP_ADDR);
NWK_SetPanId(APP_PANID);
PHY_SetChannel(APP_CHANNEL);
#ifdef PHY_AT86RF212
PHY_SetBand(APP_BAND);
PHY_SetModulation(APP_MODULATION);
#endif
PHY_SetRxState(true);

NWK_OpenEndpoint(APP_ENDPOINT, appDataInd);

HAL_BoardInit();

appTimer.interval = APP_FLUSH_TIMER_INTERVAL;
appTimer.mode = SYS_TIMER_INTERVAL_MODE;
appTimer.handler = appTimerHandler;

public_A = mod_exp(G, secret_a, P);
}

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
switch (appState)
{
case APP_STATE_INITIAL:
{
appInit();
appState = APP_STATE_SEND_PUBLIC_KEY;
} break;

case APP_STATE_SEND_PUBLIC_KEY:
{
	uint8_t char_public_A = public_A + 'a';
	appUartBuffer[0] = char_public_A;
	appUartBufferPtr = 1;
	appSendData();
	printf("Odeslan verejny klic Alice: %u ('%c')\n\r", public_A, char_public_A);
	appState = APP_STATE_WAIT_PUBLIC_KEY;
} break;

case APP_STATE_WAIT_PUBLIC_KEY:
	// Cekame na prijeti verejneho klice od Boba (udelano v appDataInd)
	break;

case APP_STATE_CALCULATE_SHARED_SECRET:
{
	shared_secret_a = mod_exp(received_public_B, secret_a, P);
	print_result("Spojeny tajny klic (Alice):", shared_secret_a);
	appState = APP_STATE_IDLE;
} break;

case APP_STATE_IDLE:
	// Dale probiha komunikace nebo nic
	break;

default:
break;
}
}

/*************************************************************************//**
*****************************************************************************/
static int uart_putchar(char c, FILE *stream) {
	if (c == '\n') {
		HAL_UartWriteByte('\r');
	}
	HAL_UartWriteByte(c);
	return 0;
}

static FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	// Presmerovani standardniho vystupu printf na UART
	stdout = &uart_str;

	// Uvodni text pro Alici
	printf("Diffie-Hellman Exchange - Alice\n\r");
	
	SYS_Init();
	HAL_UartInit(38400);

	while (1)
	{
		SYS_TaskHandler();
		HAL_UartTaskHandler();
		APP_TaskHandler();
	}
}

/*************************************************************************//**
 *****************************************************************************/
uint32_t mod_exp(uint32_t base, uint32_t exp, uint32_t mod) {
	uint32_t res = 1;
	base = base % mod;
	while (exp > 0) {
		if (exp % 2 == 1) res = (res * base) % mod;
		base = (base * base) % mod;
		exp /= 2;
	}
	return res;
}

/*************************************************************************//**
 *****************************************************************************/
void print_result(const char* label, uint32_t value) {
	printf("%s %lu\n\r", label, value);
}