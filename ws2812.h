//----------------------------------------------------------------------
// Titel	:	ws2812.h
//----------------------------------------------------------------------
// Sprache	:	C
// Datum	:	01.12.2023
// Autor	:	Diveturtle93
// Projekt	:	WS2812
// Quelle	:
//----------------------------------------------------------------------

// Sicherheitssymbol
//----------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------

// Dateiheader definieren
//----------------------------------------------------------------------
#ifndef INC_WS2812_H_
#define INC_WS2812_H_
//----------------------------------------------------------------------

// Einfuegen der standard Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der STM Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Einfuegen der eigenen Include-Dateien
//----------------------------------------------------------------------

//----------------------------------------------------------------------

// Definiere Debug-Symbole
//----------------------------------------------------------------------
#ifdef DEBUG
	#define DEBUG_WS2812
#endif
//----------------------------------------------------------------------

// Version definieren
//----------------------------------------------------------------------
#define WS2812_MAJOR							0
#define WS2812_MINOR							0
#define WS2812_PATCH							0
#define WS2812_DEV								1
//----------------------------------------------------------------------

// Typedefines definieren
//----------------------------------------------------------------------
typedef struct																// TypeDef fuer feste Farbenzuordnung
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} WS2812_Color;
//----------------------------------------------------------------------

// Konstanten definieren
//----------------------------------------------------------------------
#define WS2812_MAX_LED							1							// Anzahl der LEDs
//----------------------------------------------------------------------

// Farben definieren
//----------------------------------------------------------------------
static const WS2812_Color WS2812_AUS = {0, 0, 0};							// Aus
static const WS2812_Color WS2812_WHITE = {0xFF, 0xFF, 0xFF};				// White
static const WS2812_Color WS2812_RED = {0xFF, 0, 0};						// Rot
static const WS2812_Color WS2812_ORANGE = {0xFF, 0x80, 0};					// Orange
static const WS2812_Color WS2812_YELLOW = {0xFF, 0xFF, 0};					// Yellow
static const WS2812_Color WS2812_LIGHTGREEN = {0x80, 0xFF, 0};				// Light Green, Chartreuse
static const WS2812_Color WS2812_GREEN = {0, 0xFF, 0};						// Green
static const WS2812_Color WS2812_SPRINGGREEN = {0, 0xFF, 0x80};				// Spring Green
static const WS2812_Color WS2812_CYAN = {0, 0xFF, 0xFF};					// Cyan
static const WS2812_Color WS2812_AZURE = {0, 0x80, 0xFF};					// Azure Blue
static const WS2812_Color WS2812_BLUE = {0, 0, 0xFF};						// Blue
static const WS2812_Color WS2812_PURPLE = {0x80, 0, 0xFF};					// Purple, Electric Indigo
static const WS2812_Color WS2812_MAGENTA = {0xFF, 0, 0xFF};					// Magenta
static const WS2812_Color WS2812_ROSE = {0xFF, 0, 0x80};					// Rosa, Rose
//----------------------------------------------------------------------

// Funktionen definieren
//----------------------------------------------------------------------
void WS2812_Init (TIM_HandleTypeDef *timer, uint32_t timer_channel);		// Initialisierung der Datenstrukturen
void WS2812_SetLED (uint8_t LED_Num, uint8_t red, uint8_t green, uint8_t blue);	// Setze die Farben der einzelnen LEDs
void WS2812_SetLED_color (uint8_t LED_Num, WS2812_Color color);				// Setze die Farbe mit fixem Farbwert
void WS2812_Send_Wait (void);												// Sende Daten als PWM, mit Wartezeit
uint8_t WS2812_Send (void);													// Sende Daten als PWM, keine Wartezeit
//----------------------------------------------------------------------

#endif /* INC_WS2812_H_ */
//----------------------------------------------------------------------
