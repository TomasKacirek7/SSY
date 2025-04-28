# LWM_MSSY - Lehká Bezdrátová Komunikace s Využitím Diffie-Hellman

Tento projekt implementuje jednoduchou bezdrátovou komunikaci mezi dvěma uzly (Alice a Bob) s využitím protokolu Diffie-Hellman pro výměnu klíčů. Kód je napsán v jazyce C pro mikrokontroléry AVR a využívá knihovny Atmel Wireless Module (LWM) pro rádiovou komunikaci.

## Popis Souborů

* **`LWM_MSSY.c`**: Hlavní soubor obsahující implementaci logiky aplikace Alice, včetně inicializace, stavového automatu pro Diffie-Hellman výměnu klíčů a odesílání/příjmu dat.
* **`config.h`**: Konfigurační soubor pro nastavení parametrů sítě (PAN ID, kanál), adresace uzlů a povolení/zakázání bezpečnostních funkcí.
* **`hal.h`**, **`phy.h`**, **`sys.h`**, **`nwk.h`**, **`sysTimer.h`**, **`halBoard.h`**, **`halUart.h`**, **`main.h`**: Hlavičkové soubory poskytující rozhraní k nízkoúrovňovým hardwarovým funkcím (HAL), fyzické vrstvě (PHY), systémovým službám (SYS), síťové vrstvě (NWK), časovačům, podpoře desky a UART komunikaci. Tyto soubory jsou typickou součástí LWM frameworku.
* **`util/delay.h`**: Standardní hlavičkový soubor AVR pro implementaci zpoždění.

## Funkcionalita

Aplikace Alice prochází následujícími stavy:

1.  **`APP_STATE_INITIAL`**: Inicializace síťových parametrů, UART a časovače. Vypočítá svůj veřejný klíč (`public_A`) na základě sdílených parametrů (`P`, `G`) a svého tajného klíče (`secret_a`).
2.  **`APP_STATE_SEND_PUBLIC_KEY`**: Odešle svůj veřejný klíč (`public_A`) uzlu Bob přes bezdrátovou síť. Veřejný klíč je před odesláním převeden na ASCII znak.
3.  **`APP_STATE_WAIT_PUBLIC_KEY`**: Čeká na přijetí veřejného klíče (`public_B`) od uzlu Bob.
4.  **`APP_STATE_CALCULATE_SHARED_SECRET`**: Po přijetí veřejného klíče od Boba vypočítá sdílený tajný klíč (`shared_secret_a`) pomocí svého tajného klíče a přijatého veřejného klíče Boba. Výsledek je zobrazen přes UART.
5.  **`APP_STATE_IDLE`**: Aplikace je v klidovém stavu a může probíhat další komunikace (v tomto kódu není implementována).

## Diffie-Hellman Výměna Klíčů

Kód implementuje základní kroky Diffie-Hellman pro výměnu klíčů:

1.  **Sdílené Parametry**: Alice i Bob sdílejí veřejně známé parametry:
    * `P`: Velké prvočíslo (v kódu nastaveno na 23).
    * `G`: Primitivní kořen modulo P (v kódu nastaveno na 5).
2.  **Tajné Klíče**: Každý uzel si vybere svůj tajný klíč:
    * Alice: `secret_a` (v kódu nastaveno na 15).
    * Bob: `secret_b` (tento kód implementuje pouze Alici, Bob by měl mít podobnou logiku s vlastním tajným klíčem).
3.  **Výpočet Veřejných Klíčů**: Každý uzel vypočítá svůj veřejný klíč:
    * Alice: `public_A = G^secret_a mod P`
    * Bob: `public_B = G^secret_b mod P`
4.  **Výměna Veřejných Klíčů**: Alice a Bob si vymění své veřejné klíče.
5.  **Výpočet Sdíleného Tajného Klíče**: Každý uzel vypočítá sdílený tajný klíč pomocí svého tajného klíče a přijatého veřejného klíče druhého uzlu:
    * Alice: `shared_secret_a = public_B^secret_a mod P`
    * Bob: `shared_secret_b = public_A^secret_b mod P`

Po těchto krocích by se `shared_secret_a` a `shared_secret_b` měly rovnat a mohou být použity jako základ pro šifrování další komunikace.

## Použité Funkce

* **`uint32_t mod_exp(uint32_t base, uint32_t exp, uint32_t mod)`**: Funkce pro modulární exponentiaci, která efektivně vypočítá $\( (base^{exp}) \mod mod \)$.
* **`void print_result(const char* label, uint32_t value)`**: Pomocná funkce pro výpis označených výsledků přes UART.
* **Funkce LWM**: Kód využívá různé funkce z LWM knihoven pro inicializaci sítě (`NWK_SetAddr`, `NWK_SetPanId`, `PHY_SetChannel`, `NWK_OpenEndpoint`), odesílání dat (`NWK_DataReq`), příjem dat (`appDataInd`), práci s časovači (`SYS_TimerStart`, `SYS_TimerStop`), a UART komunikaci (`HAL_UartInit`, `HAL_UartWriteByte`, `HAL_UartReadByte`).

## Kompilace a Nahrání

Pro kompilaci a nahrání tohoto kódu na mikrokontrolér AVR budete potřebovat vývojové prostředí s podporou AVR kompilátoru (např. AVR-GCC) a nástroj pro nahrání kódu na cílové zařízení (např. AVRDUDE). Ujistěte se, že máte správně nastaveny cesty k LWM knihovnám a konfiguraci pro váš hardware.

## Poznámky

* Tento kód implementuje pouze stranu Alice. Pro plnou funkčnost Diffie-Hellman výměny klíčů je potřeba implementovat podobnou logiku i na straně Boba.
* Kód předpokládá základní konfiguraci bezdrátové sítě definovanou v `config.h`.
* Pro demonstraci je veřejný klíč Alice odeslán jako jeden ASCII znak. V reálných aplikacích by bylo vhodné odesílat veřejné klíče jako binární data pro efektivnější přenos.
* Implementovaná výměna klíčů není chráněna proti útokům typu man-in-the-middle. Pro zabezpečení komunikace by bylo nutné implementovat mechanismy pro autentizaci veřejných klíčů.

Tento projekt poskytuje základní implementaci Diffie-Hellman výměny klíčů v kontextu bezdrátové komunikace s využitím LWM frameworku pro AVR mikrokontroléry. Může sloužit jako výchozí bod pro další vývoj zabezpečených bezdrátových aplikací.

Screen terminálu Alice
![image](https://github.com/user-attachments/assets/02207bc0-e2e2-4499-9d39-8d0294ffe4af)

Screen terminálu Boba
![image](https://github.com/user-attachments/assets/e04af6d1-1f61-4cec-beaf-d5faea23b09e)

Na těchto screenech vidíme výměnu klíčů a následné vytvoření společného klíče.
