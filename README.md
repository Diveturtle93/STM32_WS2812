# STM32 WS2812

Eine in C implementierte Bibliothek für STM32-Mikrocontroller zur Ansteuerung von
WS2812-RGB-LEDs. Die Bitfolge wird von einem Timer im PWM-Modus erzeugt und per DMA
nachgeladen, sodass die Übertragung ohne CPU-Last im Hintergrund läuft.

## Beschreibung

WS2812-LEDs werden über eine einzelne Datenleitung in Reihe angesteuert. Jedes Bit dauert
1,25 µs (800 kHz) und wird über die Länge des High-Pulses kodiert: ein langer Puls ist
eine 1, ein kurzer eine 0. Jede LED übernimmt die ersten 24 Bit und reicht den Rest an
die nächste weiter. Eine Low-Phase von mindestens 50 µs schließt die Übertragung ab.

Die Bibliothek legt für jedes Bit einen Compare-Wert in einem Puffer ab und übergibt
ihn per `HAL_TIM_PWM_Start_DMA()` an den Timer. Ist der Puffer ausgegeben, stoppt der
Callback `HAL_TIM_PWM_PulseFinishedCallback()` den DMA und gibt die nächste Übertragung
frei. Timer und Channel werden bei `WS2812_Init()` übergeben.

Aufbau einer Übertragung:

| Abschnitt | Perioden | Compare-Wert | Dauer |
|-----------|---------:|-------------:|------:|
| Reset vor den Daten | 20 | 0 | 25 µs |
| Bit = 1 | je 1 | 87 | ca. 0,81 µs High |
| Bit = 0 | je 1 | 48 | ca. 0,44 µs High |
| Reset nach den Daten | 60 | 0 | 75 µs |
| Abschluss | 1 | 255 | Pin dauerhaft aktiv |

Je LED werden 24 Bit gesendet, das höchstwertige Bit zuerst, in der Reihenfolge Rot, Grün, Blau.

## Dateien

| Datei      | Beschreibung                                                                   |
|------------|--------------------------------------------------------------------------------|
| `ws2812.h` | Farbtyp `WS2812_Color`, Farbkonstanten, `WS2812_MAX_LED`, Version, Prototypen   |
| `ws2812.c` | Pufferaufbau, DMA-Übertragung und Timer-Callback                               |

## Hardware-Konfiguration

Die Compare-Werte in `ws2812.c` sind auf eine Periode von 135 Timer-Takten ausgelegt.
135 Takte ergeben 1,25 µs bei einem Timer-Takt von **108 MHz**. Die folgenden
Einstellungen in STM32CubeMX gelten für diesen Takt. Timer und Channel sind frei
wählbar, das Beispiel verwendet Timer 3, Channel 2.

### Timer

| Parameter               | Wert                       |
|-------------------------|----------------------------|
| Clock Source            | Internal Clock             |
| Channel                 | PWM Generation             |
| Prescaler               | 0                          |
| Counter Mode            | Up                         |
| Period                  | 135 - 1                    |
| Internal Clock Division | 0                          |
| Auto-Reload Preload     | Disable                    |
| Master/Slave Mode       | Disable                    |
| Trigger Event           | Reset                      |
| PWM Mode                | PWM Mode 1                 |
| Pulse                   | 0                          |
| Output Compare Preload  | Enable                     |
| Fast Mode               | Enable                     |
| CH Polarity             | Low                        |

### DMA

DMA-Request des gewählten Timer-Channels, z. B. `TIM3_CH2`:

| Parameter  | Wert                  |
|------------|-----------------------|
| Mode       | Normal                |
| Increment  | Memory                |
| Data Width | Half Word / Half Word |
| Direction  | Memory to Peripheral  |
| Priority   | High                  |

### Interrupts

| Interrupt                           | Einstellung |
|-------------------------------------|-------------|
| Timer Global Interrupt              | Enable      |
| DMA-Stream bzw. -Channel des Timers | Enable      |

### GPIO (Datenpin)

| Parameter          | Wert                 |
|--------------------|----------------------|
| Mode               | Alternate Push-Pull  |
| Pull-up/Pull-down  | No pull-up/pull-down |
| Speed              | High                 |

## Konfigurationsparameter

| Konstante        | Standardwert | Beschreibung                             |
|------------------|:------------:|------------------------------------------|
| `WS2812_MAX_LED` | 1            | Anzahl der LEDs in der Kette, in `ws2812.h` |

Der DMA-Puffer belegt `(81 + 24 × WS2812_MAX_LED) × 2` Byte RAM.

## Vordefinierte Farben

| Konstante            | Farbe         | R    | G    | B    |
|----------------------|---------------|------|------|------|
| `WS2812_AUS`         | Aus           | 0x00 | 0x00 | 0x00 |
| `WS2812_WHITE`       | Weiß          | 0xFF | 0xFF | 0xFF |
| `WS2812_RED`         | Rot           | 0xFF | 0x00 | 0x00 |
| `WS2812_ORANGE`      | Orange        | 0xFF | 0x80 | 0x00 |
| `WS2812_YELLOW`      | Gelb          | 0xFF | 0xFF | 0x00 |
| `WS2812_LIGHTGREEN`  | Hellgrün      | 0x80 | 0xFF | 0x00 |
| `WS2812_GREEN`       | Grün          | 0x00 | 0xFF | 0x00 |
| `WS2812_SPRINGGREEN` | Frühlingsgrün | 0x00 | 0xFF | 0x80 |
| `WS2812_CYAN`        | Cyan          | 0x00 | 0xFF | 0xFF |
| `WS2812_AZURE`       | Azurblau      | 0x00 | 0x80 | 0xFF |
| `WS2812_BLUE`        | Blau          | 0x00 | 0x00 | 0xFF |
| `WS2812_PURPLE`      | Lila          | 0x80 | 0x00 | 0xFF |
| `WS2812_MAGENTA`     | Magenta       | 0xFF | 0x00 | 0xFF |
| `WS2812_ROSE`        | Rosa          | 0xFF | 0x00 | 0x80 |

Eigene Farben werden als `WS2812_Color` angelegt:

```c
WS2812_Color meineFarbe = {0x40, 0x80, 0xFF};
WS2812_SetLED_color(1, meineFarbe);
```

## API

```c
void WS2812_Init (TIM_HandleTypeDef *timer, uint32_t timer_channel);				// Timer und Channel setzen, alle LEDs aus
void WS2812_SetLED (uint8_t LED_Num, uint8_t red, uint8_t green, uint8_t blue);	// Farbe einer LED per RGB-Wert setzen
void WS2812_SetLED_color (uint8_t LED_Num, WS2812_Color color);					// Farbe einer LED per Farbkonstante setzen
void WS2812_Send_Wait (void);														// Daten senden, wartet bis zum Ende
uint8_t WS2812_Send (void);														// Daten senden, ohne Wartezeit
```

- Die LEDs werden ab **1** gezählt, gültig ist `1` bis `WS2812_MAX_LED`. Der Wert wird nicht geprüft.
- `WS2812_SetLED()` und `WS2812_SetLED_color()` ändern nur den Puffer. Sichtbar wird die
  Änderung erst mit `WS2812_Send_Wait()` oder `WS2812_Send()`.
- `WS2812_Send()` gibt `1` zurück, wenn die Übertragung gestartet wurde, und `0`, wenn
  die vorherige noch läuft. Die Daten werden dann nicht gesendet.

## Verwendung

### 1. Dateien einbinden

`ws2812.h` und `ws2812.c` in das STM32-Projekt kopieren und den Header nach `tim.h`
einbinden:

```c
#include "tim.h"
#include "ws2812.h"
```

### 2. Anzahl LEDs anpassen

In `ws2812.h` `WS2812_MAX_LED` auf die Anzahl der LEDs in der Kette setzen:

```c
#define WS2812_MAX_LED							8							// Anzahl der LEDs
```

### 3. Initialisieren

Nach den CubeMX-Initialisierungen Timer und Channel übergeben:

```c
MX_DMA_Init();
MX_TIM3_Init();

WS2812_Init(&htim3, TIM_CHANNEL_2);
```

### 4. LEDs ansteuern

```c
// Erste LED rot, zweite LED cyan, dann senden und warten
WS2812_SetLED(1, 0xFF, 0x00, 0x00);
WS2812_SetLED_color(2, WS2812_CYAN);
WS2812_Send_Wait();

// Alle LEDs ausschalten, ohne zu warten
for (uint8_t i = 1; i <= WS2812_MAX_LED; i++)
{
	WS2812_SetLED_color(i, WS2812_AUS);
}
WS2812_Send();
```

## Hinweise

- Die Bibliothek definiert `HAL_TIM_PWM_PulseFinishedCallback()`. Wird der Callback im
  Projekt noch für andere Timer gebraucht, muss er an einer Stelle zusammengeführt werden.
- Bei einem anderen Timer-Takt als 108 MHz müssen Period sowie die Compare-Werte `87` und
  `48` in `ws2812.c` angepasst werden. Period = Timer-Takt × 1,25 µs.
- `WS2812_Send_Wait()` wartet aktiv in einer Schleife und blockiert dabei die Hauptschleife.
  Nicht aus einem Interrupt aufrufen, sonst wird das Ende der Übertragung nie erkannt.

## Quellen

- [WS2812B Datenblatt (Worldsemi)](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)

## Abhängigkeiten

- `tim.h` – von STM32CubeMX erzeugter Timer-Header mit STM32 HAL und Timer-Handle

## Lizenz

Dieses Projekt steht unter der [GPL-3.0 Lizenz](LICENSE).
