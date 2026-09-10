# Xteink X4 / X4 Pro Hardware Documentation

## Overview

The Xteink X4 series are ultra-compact e-ink readers based on the ESP32-C3 microcontroller. This document summarizes the hardware specifications and pin definitions discovered through community reverse engineering.

## Hardware Specifications

### MCU
- **Chip:** ESP32-C3FH4
- **Architecture:** RISC-V 32-bit, single core
- **Clock Speed:** 160 MHz
- **Flash:** 4MB (on-chip) + external 16MB SPI flash
- **RAM:** 400KB SRAM (~380KB usable for application)
- **WiFi:** 802.11 b/g/n
- **Bluetooth:** BLE 5.0

### Display
- **Type:** E-Ink (EPD)
- **Size:** 4.26 inches diagonal
- **Resolution:** 800 × 480 pixels
- **Driver IC:** SSD1677
- **Panel Model:** GDEQ0426T82
- **Interface:** SPI
- **Grayscale:** Monochrome (1-bit), 4-level grayscale supported via multi-pass

### Storage
- **microSD Card:** Yes (SPI interface)
- **File System:** FAT32

### Input
- **Front Buttons:** 4 (Back, Confirm, Left, Right)
- **Side Buttons:** 2 (Volume Up, Volume Down)
- **Power Button:** 1 (on top)
- **Touch:** X4 Pro only (capacitive)

### Power
- **Battery:** 650mAh Li-polymer
- **Charging:** USB-C
- **Battery Monitoring:** ADC with voltage divider
- **Deep Sleep:** Supported

### Physical
- **Dimensions:** ~114 × 69 × 5.98 mm
- **Weight:** ~70-75g
- **Magnetic:** Yes (X4 Pro)

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

### microSD Card (SPI - shared with EPD)

| Signal | GPIO | Notes |
|--------|------|-------|
| CS (SS) | GPIO12 | Card select |
| MISO | GPIO7 | Data In to ESP32 |
| MOSI | GPIO10 | Data Out from ESP32 (shared) |
| SCK | GPIO8 | Clock (shared) |

### Button Input (Resistor Ladder ADC)

Buttons are connected via resistor ladder networks to ADC pins. Each button produces a unique analog voltage.

**GPIO1 (Front Buttons - 4 buttons):**

| Button | ADC Value (approx) | Voltage (approx) |
|--------|---------------------|------------------|
| Back | ~3470 | 2.79V |
| Confirm | ~2655 | 2.14V |
| Left | ~1470 | 1.18V |
| Right | ~3 | 0.00V |

**GPIO2 (Side Buttons - 2 buttons):**

| Button | ADC Value (approx) | Voltage (approx) |
|--------|---------------------|------------------|
| Volume Up | ~2205 | 1.78V |
| Volume Down | ~3 | 0.00V |

**Power Button:**

| Signal | GPIO | Notes |
|--------|------|-------|
| POWER | GPIO3 | Active LOW |

### Battery Monitoring

| Signal | GPIO | Notes |
|--------|------|-------|
| BAT_ADC | GPIO0 | Battery voltage via 2×10K divider (reads 1/2) |

### USB Detection

| Signal | GPIO | Notes |
|--------|------|-------|
| USB_DET | GPIO20 | UART0_RXD - HIGH when USB connected |

### Touch Panel (X4 Pro Only)

*Pin definitions to be verified for X4 Pro hardware revision.*

| Signal | GPIO | Notes |
|--------|------|-------|
| SDA | TBD | I2C Data |
| SCL | TBD | I2C Clock |
| INT | TBD | Touch interrupt |

### Front Light (X4 Pro Only)

*Pin definitions to be verified for X4 Pro hardware revision.*

| Signal | GPIO | Notes |
|--------|------|-------|
| PWM | TBD | Brightness control |
| WARM | TBD | Warm/cold color control |

## Boot Modes

### Normal Boot
- Power button press > 1 second
- Boots from active OTA partition

### Flash Mode (Download Mode)
1. Hold BOOT button (or connect with special key combo)
2. Connect USB
3. Device enters download mode
4. Use esptool to flash firmware

### Recovery Mode
- If both OTA partitions are corrupted, device may brick
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

## Known Issues & Notes

1. **SC_CS connection is unusual** - The display CS connection has a non-standard routing (per hardware teardown).
2. **Shared SPI bus** - EPD and SD card share the same SPI bus. CS signals are used to arbitrate.
3. **Button debouncing** - Use threshold ranges with hysteresis to avoid false triggers.
4. **ADC noise** - ADC readings may vary slightly; use ranges with margins.
5. **X4 vs X4 Pro** - The Pro version adds touch and front light hardware. Verify pin mappings for your specific hardware revision.

## References

- [CrossPoint Reader HAL](https://github.com/crosspoint-reader/crosspoint-reader/tree/master/lib/hal)
- [sample-firmware](https://github.com/open-x4-epaper/sample-firmware)
- [Xteink-X4 hardware info](https://github.com/sunwoods/Xteink-X4)
- [Adafruit CircuitPython pinout](https://learn.adafruit.com/circuitpython-on-the-xteink-x4-ereader/pinouts)
