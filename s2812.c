//----------------------------------------------------------------------
// Titel	:	ws2812.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	01.12.2023
// Version	:	1.0
// Autor	:	Diveturtle93
// Projekt	:	WS2812
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------
#include "tim.h"
//----------------------------------------------------------------------

// Einfuegen der eigenen Include Dateien
//----------------------------------------------------------------------
#include "ws2812.h"
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
TIM_HandleTypeDef *ws2812_htim;												// Timer Handle
uint32_t channel;															// Timer Channel
uint8_t LED_Data[WS2812_MAX_LED][4];										// Farben fuer die einzelnen LEDs
uint16_t pwmData[20 + (24 * WS2812_MAX_LED) + 60 + 1];						// Array fuer PWM Daten
volatile uint8_t datasentflag = 1;											// Datenflag fuer Senden per DMA
//----------------------------------------------------------------------

// Initialisierung der Datenstrukturen
//----------------------------------------------------------------------
void WS2812_Init (TIM_HandleTypeDef *timer, uint32_t timer_channel)
{
	// Timer Handle muss vor der Initialisierung gesetzt werden, damit die Funktionen wissen mit welchem Timer sie arbeiten
	ws2812_htim = timer;

	// Channel muss vor der Initialisierung gesetzt werden, damit die Funktionen wissen mit welchem Channel sie arbeiten
	channel = timer_channel;

	// Alle LEDs auf Aus setzen
	for (uint8_t i = 0; i < WS2812_MAX_LED; i++)
	{
		// Setze LED mit fixem Farbwert auf Aus
		WS2812_SetLED_color(i + 1, WS2812_AUS);
	}
}
//----------------------------------------------------------------------

// Setze einzelne LED mit individuellem Farbwert
//----------------------------------------------------------------------
void WS2812_SetLED (uint8_t LED_Num, uint8_t red, uint8_t green, uint8_t blue)
{
	// Beschreibe Array und setze die Farben der einzelnen LEDs
	LED_Data[LED_Num - 1][0] = LED_Num;
	LED_Data[LED_Num - 1][1] = red;
	LED_Data[LED_Num - 1][2] = green;
	LED_Data[LED_Num - 1][3] = blue;
}
//----------------------------------------------------------------------

// Setze einzelne LED mit fixem Farbwert
//----------------------------------------------------------------------
void WS2812_SetLED_color (uint8_t LED_Num, WS2812_Color color)
{
	WS2812_SetLED(LED_Num, color.red, color.green, color.blue);
}
//----------------------------------------------------------------------

// Sende Daten an LEDs, mit Wartezeit
//----------------------------------------------------------------------
void WS2812_Send_Wait (void)
{
	// Variablen fuer Funktion definieren
	uint16_t indx = 0;
	uint32_t color = 0;

	// Initial 20 Bit als Low senden, zuruecksetzen WS2812
	for (uint8_t i = 0; i < 20; i++)
	{
		// Daten gleich 0 und Index hochzaehlen
		pwmData[indx] = 0;
		indx++;
	}

	// Daten der LED Farben konvertieren in PWM
	for (uint8_t i = 0; i < WS2812_MAX_LED; i++)
	{
		// Daten der LED
		color = ((LED_Data[i][1] << 16) | (LED_Data[i][2] << 8) | (LED_Data[i][3]));

		// Konvertieren
		for (uint8_t j = 23; j >= 0; j--)
		{
			// Bit = 1 langer High Pulse, Bit = 0 kurzer High Pulse
			if (color & (1 << j))
			{
				// ca. 2/3 High, 850ns
				pwmData[indx] = 87;
			}
			else
			{
				// ca. 1/3 High, 400ns
				pwmData[indx] = 48;
			}

			indx++;
		}

	}

	// Zuruecksetzen der WS2812, Low Puls, 50us gefordert, zur Sicherheit 60 mal senden
	// eine Periode 1,25us, Gesamt = 60 x 1,25us = 75us
	for (uint8_t i = 0; i < 60; i++)
	{
		pwmData[indx] = 0;
		indx++;
	}

	// Sicher Pin auf High ziehen
	pwmData[indx] = 255;
	indx++;

	// Starte DMA und sende Daten fuer PWM
	HAL_TIM_PWM_Start_DMA(ws2812_htim, channel, (uint32_t *)pwmData, indx);
	
	// Setze Flag zurueck
	datasentflag = 0;

	// Warten bis alle Daten gesendet wurden und Interrupt DMA stoppt
	while (!datasentflag){};
}
//----------------------------------------------------------------------

// Sende Daten an LEDs, keine Wartezeit
//----------------------------------------------------------------------
uint8_t WS2812_Send (void)
{
	// Wenn Sendeflag = 1, Daten senden
	if (datasentflag == 1)
	{
		// Variablen fuer Funktion definieren
		uint16_t indx = 0;
		uint32_t color;

		// Initial 20 Bit als Low senden, reset WS2812
		for (uint8_t i = 0; i < 20; i++)
		{
			// Daten gleich 0 und Index hochzaehlen
			pwmData[indx] = 0;
			indx++;
		}

		// Daten der LED Farben konvertieren in PWM
		for (uint8_t i = 0; i < WS2812_MAX_LED; i++)
		{
			// Daten der LED
			color = ((LED_Data[i][1] << 16) | (LED_Data[i][2] << 8) | (LED_Data[i][3]));

			// Konvertieren
			for (uint8_t j = 23; j >= 0; j--)
			{
				// Bit = 1 langer High Pulse, Bit = 0 kurzer High Pulse
				if (color & (1 << j))
				{
					// ca. 2/3 High, 850ns
					pwmData[indx] = 87;
				}
				else
				{
					// ca. 1/3 High, 400ns
					pwmData[indx] = 48;
				}

				indx++;
			}

		}

		// Zuruecksetzen der WS2812, Low Puls, 50us gefordert, zur Sicherheit 60 mal senden
		// eine Periode 1,25us, Gesamt = 60 x 1,25us = 75us
		for (uint8_t i = 0; i < 60; i++)
		{
			pwmData[indx] = 0;
			indx++;
		}

		// Sicher Pin auf High ziehen
		pwmData[indx] = 255;
		indx++;

		// Starte DMA und sende Daten fuer PWM
		HAL_TIM_PWM_Start_DMA(ws2812_htim, channel, (uint32_t *)pwmData, indx);

		// Datenflag zuruecksetzen, ueberspringt naechste Uebertragung, wenn noch nicht abgeschlossen
		datasentflag = 0;

		return 1;
	}
	// Wenn Sendeflag = 0, Datenuebertragung ueberspringen
	else
	{
		return 0;
	}
}
//----------------------------------------------------------------------

// Timer Callback fuer DMA
//----------------------------------------------------------------------
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == ws2812_htim)
	{
		// DMA stoppen
		HAL_TIM_PWM_Stop_DMA(ws2812_htim, channel);

		// Flag setzen damit "while" aus WS2812_send verlassen werden kann
		// Flag auch wichtig fuer WS2812_Send, damit naechste Uebertragung gestartet werden kann
		datasentflag = 1;
	}
}
//----------------------------------------------------------------------
