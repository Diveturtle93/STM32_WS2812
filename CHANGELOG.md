# Changelog

Alle nennenswerten Änderungen an diesem Projekt werden hier dokumentiert.
Format nach [Keep a Changelog](https://keepachangelog.com/de/1.1.0/),
Versionierung nach [Semantic Versioning](https://semver.org/lang/de/).

## [Unveröffentlicht]

## [1.0.0] - 2026-11-02

### Hinzugefügt
- `WS2812_Init()` übernimmt Timer-Handle und Channel für die Ausgabe und schaltet alle LEDs aus.
- `WS2812_SetLED()` und `WS2812_SetLED_color()` setzen die Farbe einer LED per RGB-Wert bzw. per Farbkonstante.
- `WS2812_Send_Wait()` sendet die LED-Daten per DMA und wartet, bis die Übertragung abgeschlossen ist.
- `WS2812_Send()` startet die Übertragung ohne zu warten und gibt 0 zurück, solange die vorherige noch läuft.
- Vordefinierte Farben `WS2812_AUS` bis `WS2812_ROSE` als `WS2812_Color`-Konstanten.
- Anzahl der LEDs als `WS2812_MAX_LED` in `ws2812.h`.
- Versionsnummer als Defines `WS2812_MAJOR`, `WS2812_MINOR`, `WS2812_PATCH` und `WS2812_DEV` in `ws2812.h`.
- Debug-Symbol `DEBUG_WS2812`, das bei gesetztem `DEBUG` definiert wird.
- README mit Hardware-Konfiguration, API und Verwendung.
- CHANGELOG.
- Lizenz GPL-3.0.

[Unveröffentlicht]: https://github.com/Diveturtle93/STM32_WS2812/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/Diveturtle93/STM32_WS2812/releases/tag/v1.0.0
