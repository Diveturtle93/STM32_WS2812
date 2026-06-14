# STM32 WS2812
 
Eine in C implementierte Bibliothek für STM32-Mikrocontroller zur Ansteuerung von
WS2812-RGB-LEDs über PWM und DMA. Die Datenübertragung erfolgt über einen Timer im
PWM-Modus mit DMA-Unterstützung, sodass die LED-Daten ohne CPU-Last im Hintergrund
übertragen werden.
 
## Beschreibung
 
WS2812-LEDs werden über ein einzelnes Datensignal angesteuert, bei dem jedes Bit als
PWM-Impuls mit unterschiedlicher Pulsbreite kodiert ist. Die Bibliothek nutzt **Timer 3,
Channel 2** im PWM-Modus zusammen mit **DMA1 Channel 5**, um die Bitfolge für alle LEDs
automatisch im Hintergrund auszugeben. Es werden zwei Sendepfade angeboten: einer mit
Wartezeit bis zur vollständigen Übertragung und einer nicht-blockierend.
 
## Dateien
 
| Datei      | Beschreibung                                                              |
|------------|---------------------------------------------------------------------------|
| `WS2812.h` | Farbdefinitionen, `WS2812_Color`-Struct, Konstanten, Funktionsdeklarationen |
| `WS2812.c` | Implementierung der LED-Steuerung und DMA-Übertragung                     |
 
## Hardware-Konfiguration
 
Die Bibliothek erfordert eine spezifische Timer- und DMA-Konfiguration in STM32CubeMX.
Alle Parameter müssen exakt wie folgt eingestellt werden, da die WS2812-Zeitvorgaben
sehr präzise eingehalten werden müssen.
 
### Timer 3 – Konfiguration
 
| Parameter             | Wert                  |
|-----------------------|-----------------------|
| Clock Source          | Internal Clock        |
| Channel               | Channel 2 – PWM Generation |
| Prescaler             | 0                     |
| Counter Mode          | Up                    |
| Period                | 135 − 1               |
| Internal Clock Division | 0                   |
| Auto-Reload Preload   | Disable               |
| Master/Slave Mode     | Disable               |
| Trigger Event         | Reset                 |
| PWM Mode              | PWM Mode 1            |
| Pulse                 | 0                     |
| Output Compare Preload | Enable               |
| Fast Mode             | Enable                |
| CH Polarity           | Low                   |
 
### Interrupts
 
| Interrupt               | Einstellung |
|-------------------------|-------------|
| Timer 3 Global Interrupt | Enable     |
| DMA1 Channel 5          | Enable      |
 
### GPIO-Konfiguration (Datenpin)
 
| Parameter       | Wert               |
|-----------------|--------------------|
| Mode            | Alternate Push-Pull |
| Pull-up/Pull-down | No pull-up/pull-down |
| Speed           | High               |
 
### DMA1 Channel 5 – Konfiguration
 
| Parameter  | Wert                |
|------------|---------------------|
| Mode       | Normal              |
| Increment  | Memory only         |
| Data Size  | Half Word / Half Word |
| Direction  | Memory to Peripheral |
| Priority   | High                |
 
## Konfigurationsparameter
 
| Konstante  | Standardwert | Beschreibung                      |
|------------|:------------:|-----------------------------------|
| `MAX_LED`  | 1            | Anzahl der angesteuerten WS2812-LEDs |
 
## API
 
```c
void     SetLED        (uint8_t LED_Num, uint8_t red, uint8_t green, uint8_t blue); // Farbe per RGB-Wert setzen
void     SetLED_color  (uint8_t LED_Num, WS2812_Color color);                       // Farbe per Farbkonstante setzen
void     WS2812_Send_Wait (void);                                                    // Daten senden, blockierend
uint8_t  WS2812_Send      (void);                                                    // Daten senden, nicht blockierend
```
 
`WS2812_Send_Wait()` wartet nach dem Senden, bis die DMA-Übertragung vollständig
abgeschlossen ist. `WS2812_Send()` startet die Übertragung und kehrt sofort zurück –
der Rückgabewert zeigt an, ob die Übertragung erfolgreich gestartet werden konnte.
 
## Vordefinierte Farben
 
Die Bibliothek stellt fertige `WS2812_Color`-Konstanten bereit:
 
| Konstante             | Farbe          | R    | G    | B    |
|-----------------------|----------------|------|------|------|
| `WS2812_AUS`          | Aus            | 0x00 | 0x00 | 0x00 |
| `WS2812_WHITE`        | Weiß           | 0xFF | 0xFF | 0xFF |
| `WS2812_RED`          | Rot            | 0xFF | 0x00 | 0x00 |
| `WS2812_ORANGE`       | Orange         | 0xFF | 0x80 | 0x00 |
| `WS2812_YELLOW`       | Gelb           | 0xFF | 0xFF | 0x00 |
| `WS2812_LIGHTGREEN`   | Hellgrün       | 0x80 | 0xFF | 0x00 |
| `WS2812_GREEN`        | Grün           | 0x00 | 0xFF | 0x00 |
| `WS2812_SPRINGGREEN`  | Frühlingsgrün  | 0x00 | 0xFF | 0x80 |
| `WS2812_CYAN`         | Cyan           | 0x00 | 0xFF | 0xFF |
| `WS2812_AZURE`        | Azurblau       | 0x00 | 0x80 | 0xFF |
| `WS2812_BLUE`         | Blau           | 0x00 | 0x00 | 0xFF |
| `WS2812_PURPLE`       | Lila           | 0x80 | 0x00 | 0xFF |
| `WS2812_MAGENTA`      | Magenta        | 0xFF | 0x00 | 0xFF |
| `WS2812_ROSE`         | Rosa           | 0xFF | 0x00 | 0x80 |
 
Eigene Farben können direkt als `WS2812_Color`-Struct übergeben werden:
 
```c
WS2812_Color meineFarbe = {0x40, 0x80, 0xFF};
SetLED_color(0, meineFarbe);
```
 
## Verwendung
 
### 1. Dateien einbinden
 
`WS2812.h` und `WS2812.c` in das STM32-Projekt kopieren und den Header einbinden:
 
```c
#include "WS2812.h"
```
 
### 2. Anzahl LEDs anpassen
 
In `WS2812.h` die Konstante `MAX_LED` auf die tatsächliche Anzahl der LEDs setzen:
 
```c
#define MAX_LED 8    // z. B. für 8 LEDs
```
 
### 3. LEDs ansteuern
 
```c
// Erste LED rot setzen und senden (blockierend)
SetLED(0, 0xFF, 0x00, 0x00);
WS2812_Send_Wait();
 
// Zweite LED mit Farbkonstante setzen
SetLED_color(1, WS2812_CYAN);
WS2812_Send_Wait();
 
// Alle LEDs ausschalten
for (uint8_t i = 0; i < MAX_LED; i++)
{
    SetLED_color(i, WS2812_AUS);
}
WS2812_Send_Wait();
```
 
## Abhängigkeiten
 
- `main.h` – STM32 HAL (inkl. Timer- und DMA-Handle)

## Lizenz
 
Dieses Projekt steht unter der [GPL-3.0 Lizenz](LICENSE).
