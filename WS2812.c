//----------------------------------------------------------------------
// Titel	:	WS2812.c
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	Dec 1, 2023
// Version	:	1.0
// Autor	:	Diveturtle93
// Projekt	:	Motorsteuergeraet
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
#include "WS2812.h"
//----------------------------------------------------------------------

// Variablen definieren
//----------------------------------------------------------------------
uint8_t LED_Data[MAX_LED][4];												// Farben fuer die einzelnen LEDs
uint16_t pwmData[20 + (24 * MAX_LED) + 60 + 1];								// Array fuer PWM Daten
volatile uint8_t datasentflag = 0;											// Datenflag fuer Senden per DMA
//----------------------------------------------------------------------

// Setze einzelne LED mit individuellem Farbwert
//----------------------------------------------------------------------
void SetLED (uint8_t LED_Num, uint8_t red, uint8_t green, uint8_t blue)
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
void SetLED_color (uint8_t LED_Num, WS2812_Color color)
{
	SetLED(LED_Num, color.red, color.green, color.blue);
}
//----------------------------------------------------------------------

// Sende Daten an LEDs, mit Wartezeit
//----------------------------------------------------------------------
void WS2812_Send_Wait (void)
{
	// Variablen fuer Funktion definieren
	uint16_t indx = 0;
	uint32_t color = 0;

	// Initial 20 Bit als Low senden, reset WS2812
	for (int i = 0; i < 20; i++)
	{
		// Daten gleich 0 und Index hochzaehlen
		pwmData[indx] = 0;
		indx++;
	}

	// Daten der LED Farben konvertieren in PWM
	for (int i = 0; i < MAX_LED; i++)
	{
		// Daten der LED
		color = ((LED_Data[i][1] << 16) | (LED_Data[i][2] << 8) | (LED_Data[i][3]));

		// Konvertieren
		for (int i = 23; i >= 0; i--)
		{
			// Bit = 1 langer High Pulse, Bit = 0 kurzer High Pulse
			if (color & (1 << i))
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

	// Reset der WS2812, Low Pulse, 50us gefordert, zur Sicherheit 60 mal senden
	// eine Periode 1,25us, Gesamt = 60 x 1,25us = 75us
	for (int i = 0; i < 60; i++)
	{
		pwmData[indx] = 0;
		indx++;
	}

	// Sicher Pin auf High ziehen
	pwmData[indx] = 255;
	indx++;

	// Starte DMA und sende Daten fuer PWM
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t *)pwmData, indx);

	// Warten bis alle Daten gesendet wurden und Interrupt DMA stoppt
	while (!datasentflag){};
	datasentflag = 1;
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
		for (int i = 0; i < 20; i++)
		{
			// Daten gleich 0 und Index hochzaehlen
			pwmData[indx] = 0;
			indx++;
		}

		// Daten der LED Farben konvertieren in PWM
		for (int i = 0; i < MAX_LED; i++)
		{
			// Daten der LED
			color = ((LED_Data[i][1] << 16) | (LED_Data[i][2] << 8) | (LED_Data[i][3]));

			// Konvertieren
			for (int i = 23; i >= 0; i--)
			{
				// Bit = 1 langer High Pulse, Bit = 0 kurzer High Pulse
				if (color & (1 << i))
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

		// Reset der WS2812, Low Pulse, 50us gefordert, zur Sicherheit 60 mal senden
		// eine Periode 1,25us, Gesamt = 60 x 1,25us = 75us
		for (int i = 0; i < 60; i++)
		{
			pwmData[indx] = 0;
			indx++;
		}

		// Sicher Pin auf High ziehen
		pwmData[indx] = 255;
		indx++;

		// Starte DMA und sende Daten fuer PWM
		HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t *)pwmData, indx);

		// Datenflag zuruecksetzen, ueberspringt naechste Uebertragung, wenn noch nicht abgeschlossen
		datasentflag = 0;

		return 1;
	}
	// Wenn Sendeflag = 0, Datenübertragung ueberspringen
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
	if (htim == &htim3)
	{
		// Stop DMA
		HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_2);

		// Flag setzen damit while aus WS2812_send verlassen werden kann
		datasentflag = 1;
	}
}
//----------------------------------------------------------------------
