# Xteink X4 Pro Hardware Documentation

## Overview

The Xteink X4 Pro is the premium ultra-portable e-ink reader with capacitive touchscreen and dual-tone front light, based on the ESP32-C3 microcontroller. This document summarizes the hardware specifications and pin definitions discovered through community reverse engineering and product specifications.

## Hardware Specifications

### MCU
- **Chip:** ESP32-C3FH4
- **Architecture:** RISC-V 32-bit, single core
- **Clock Speed:** 160 MHz
- **Flash:** 16MB SPI flash
- **RAM:** 400KB SRAM (~380KB usable for application)
- **WiFi:** 802.11 b/g/n
- **Bluetooth:** BLE 5.0

### Display
- **Type:** E-Ink (EPD)
- **Size:** 4.3 inches diagonal
- **Resolution:** 800 × 480 pixels
- **Driver IC:** SSD1677
- **Panel Model:** GDEQ0426T82
- **Interface:** SPI
- **Grayscale:** Monochrome (1-bit), 4-level grayscale supported via multi-pass
- **Touch:** Capacitive touchscreen (I2C)

### Touch Panel
- **Type:** Capacitive multi-touch
- **Interface:** I2C (400kHz)
- **Touch IC:** GT911 compatible
- **Features:** Tap, double-tap, swipe gestures, long press
- **Home Key:** Capacitive touch button on bottom bezel

### Front Light
- **Type:** Dual-tone LED (warm + cool white)
- **Control:** PWM dimming
- **Brightness:** 0-100% adjustable
- **Color Temperature:** Fully adjustable between warm and cool
- **Uniformity:** Even illumination across screen

### Storage
- **microSD Card:** Yes (SPI interface)
- **File System:** FAT32

### Input
- **Touch Screen:** Capacitive, tap + swipe gestures
- **Side Buttons:** 2 (Left / Right page turn)
- **Confirm / Back:** Via touch UI
- **Power Button:** 1 (top)
- **Capacitive Home Key:** Bottom touch-sensitive button

### Power
- **Battery:** Li-polymer (capacity TBD)
- **Charging:** Magnetic pogo pins (no USB-C port)
- **Battery Monitoring:** ADC with voltage divider
- **Deep Sleep:** Supported

### Physical
- **Dimensions:** Ultra-slim form factor
- **Thickness:** 5.95mm
- **Weight:** ~70g
- **Magnetic:** Yes (charging + data transfer)

## Pin Definitions

### E-Paper Display (SPI)

| Signal | GPIO | Notes |
|--------|------|-------|
| SCLK | GPIO8 | SPI Clock |
| MOSI | GPIO10 | SPI Data (Master Out) |
| CS | GPIO21 | Chip Select (active LOW) |
| DC | GPIO4 | Data/Command select |
| RST | GPIO5 | Reset (active LOW) |
| BUSY | GPIO6 | Busy signal (HIGH = busy) |

### Touch Panel (I2C)

| Signal | GPIO | Notes |
|--------|------|-------|
| SDA | GPIO9 | I2C Data |
| SCL | GPIO10 | I2C Clock |
| INT | GPIO7 | Touch interrupt (active LOW) |
| RST | GPIO2 | Touch reset |

*Note: SCL pin is shared with SPI MOSI - verify actual pinout on hardware revision.*

### Front Light (PWM)

| Signal | GPIO | Notes |
|--------|------|-------|
| Warm LED PWM | GPIO1 | Warm white channel brightness |
| Cool LED PWM | GPIO2 | Cool white channel brightness |

*PWM frequency: 5kHz, 10-bit resolution (0-1023)*

### microSD Card (SPI - shared with EPD)

| Signal | GPIO | Notes |
|--------|------|-------|
| CS (SS) | GPIO12 | Card select |
| MISO | GPIO7 | Data In to ESP32 |
| MOSI | GPIO10 | Data Out from ESP32 (shared) |
| SCK | GPIO8 | Clock (shared) |

### Button Input (Resistor Ladder ADC)

Buttons are connected via resistor ladder networks to ADC pins.

**GPIO1 (Side Buttons):**

| Button | ADC Value (approx) | Voltage (approx) |
|--------|---------------------|------------------|
| Left (Prev Page) | ~1470 | 1.18V |
| Right (Next Page) | ~3 | 0.00V |

**GPIO2 (Additional Buttons):**

| Button | ADC Value (approx) | Voltage (approx) |
|--------|---------------------|------------------|
| Confirm | ~2655 | 2.14V |
| Back | ~3470 | 2.79V |

**Power Button:**

| Signal | GPIO | Notes |
|--------|------|-------|
| POWER | GPIO3 | Active LOW |

**Capacitive Home Key:**

| Signal | GPIO | Notes |
|--------|------|-------|
| HOME | GPIO20 | Capacitive touch button |

### Battery Monitoring

| Signal | GPIO | Notes |
|--------|------|-------|
| BAT_ADC | GPIO0 | Battery voltage via 2×10K divider (reads 1/2) |

### Magnetic Charging Detection

| Signal | GPIO | Notes |
|--------|------|-------|
| MAG_DET | GPIO20 | Magnetic dock / charger detection |

## Boot Modes

### Normal Boot
- Press power button
- Boots from active OTA partition

### Flash Mode (Download Mode)
1. Hold specific key combo during power on
2. Connect magnetic data adapter
3. Device enters download mode
4. Use esptool to flash firmware

### Recovery Mode
- Use SD card recovery (update.bin on SD card root)

## Flash Layout

```
0x000000  Bootloader
0x090000  NVS (Non-Volatile Storage)
0x0E0000  OTA Data
0x100000  App0 (OTA Partition 0) - ~6.5MB
0x650000  App1 (OTA Partition 1) - ~6.5MB
0xC90000  SPIFFS / File System - ~3.5MB
```

## X4 Pro Exclusive Features

### Touch Gestures
The X4 Pro supports intuitive touch gestures:
- **Tap** - Select / open
- **Swipe Left** - Next page
- **Swipe Right** - Previous page
- **Swipe Down** - Quick settings panel (front light, brightness)
- **Swipe Up** - Main menu
- **Long Press** - Context menu

### Front Light Control
- Pull down from top edge to open quick settings
- Adjust brightness slider
- Adjust warmth slider (warm ↔ cool)
- Tap to toggle on/off

### Magnetic Connectivity
- Magnetic charging pogo pins
- Magnetic data transfer (connect to computer)
- Magnetic attachment to phone back

## Known Issues & Notes

1. **Shared SPI bus** - EPD and SD card share the same SPI bus. CS signals are used to arbitrate.
2. **Touch I2C shared pins** - Some I2C pins may overlap with SPI. Verify actual routing.
3. **Button debouncing** - Use threshold ranges with hysteresis to avoid false triggers.
4. **ADC noise** - ADC readings may vary slightly; use ranges with margins.
5. **Magnetic port** - No traditional USB-C; uses magnetic pogo pins for data and power.

## References

- [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader) - Open firmware project
- [open-x4-epaper](https://github.com/open-x4-epaper) - Community SDK
- [Xteink X4 Pro Product Page](https://www.xteink.com/products/xteink-x4pro) - Official specs
- [Digital Trends Review](https://www.digitaltrends.com/phones/xteink-x4-pro-review/) - X4 Pro review
