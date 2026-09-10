# StarReader Firmware for Xteink X4 Pro

An open-source e-reader firmware specifically designed for **Xteink X4 Pro** (阅星瞳 X4 Pro) e-paper device.

## Overview

StarReader Pro is a community-driven, fully open-source firmware built exclusively for the Xteink X4 Pro — the premium ultra-portable e-ink reader with touchscreen and dual-tone front light.

**Target Hardware:**
- Xteink X4 Pro (阅星瞳 X4 Pro)

## What's Special About X4 Pro

- **4.3" E-Ink Touchscreen** - Capacitive touch support for tap and swipe gestures
- **Dual-Tone Front Light** - Adjustable brightness and warm/cool color temperature
- **Capacitive Home Button** - Touch-sensitive home key on the bottom
- **Magnetic Charging** - No USB-C port, uses magnetic pogo pins
- **Ultra-Slim Design** - Only 5.95mm thick, ~70g weight

## Features

### Touch & Navigation
- **Capacitive Touch** - Tap to select, swipe to turn pages
- **Capacitive Home Key** - Bottom touch-sensitive button
- **Physical Buttons** - Side page turn buttons still supported
- **Gesture Support** - Swipe left/right for page navigation

### Front Light
- **Dual-Tone LED** - Warm and cool white channels
- **Brightness Control** - 0-100% adjustable via PWM
- **Color Temperature** - Mix warm/cool for perfect reading light
- **Quick Toggle** - One tap to turn on/off

### Reading Experience
- **TXT Reader** - Plain text file reader with page navigation
- **EPUB Support** - EPUB 2/3 rendering (planned)
- **Custom Fonts** - Install fonts from SD card
- **Layout Controls** - Margins, line spacing, justification
- **Reading Progress** - Automatic position saving

### Display
- **4.3" E-Ink** - 800×480 monochrome display (GDEQ0426T82)
- **Multiple Refresh Modes** - Full, Partial, Fast
- **Orientation Support** - Portrait / Landscape / Inverted
- **Status Bar** - Battery, page count, front light status

### Power Management
- **Deep Sleep** - Ultra-low power consumption
- **Auto Sleep** - Configurable timeout
- **Battery Monitoring** - Voltage and percentage readout
- **Wake on Touch / Power Button** - Instant resume

### Storage
- **microSD Card** - Expandable storage
- **Settings Persistence** - Save configuration to SD
- **Book Cache** - Pre-rendered page cache

## Hardware Specifications

| Component | Specification |
|-----------|--------------|
| MCU | ESP32-C3 (RISC-V, 160MHz) |
| Flash | 16MB SPI |
| RAM | ~400KB (~380KB usable) |
| Display | 4.3" E-Ink, 800×480, SSD1677 |
| Touch | Capacitive (I2C, GT911 compatible) |
| Front Light | Dual-tone (warm + cool), PWM controlled |
| Storage | microSD card slot |
| Battery | Li-ion (magnetic charging) |
| Thickness | 5.95mm |
| Weight | ~70g |
| Magnetic | Yes (charging + data) |

For detailed pin definitions, see [docs/hardware.md](docs/hardware.md).

## Quick Start

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- Xteink X4 Pro device
- Magnetic data cable / pogo pin adapter

### Build & Flash

```bash
# Clone the repository
git clone https://github.com/SHMILYapos/starreader-firmware.git
cd starreader-firmware

# Build
platformio run

# Flash (enter download mode via special key combo)
platformio run -t upload

# Open serial monitor
platformio device monitor
```

For detailed build instructions, see [docs/build.md](docs/build.md).

## Project Structure

```
starreader-firmware/
├── src/
│   ├── main.cpp              # Entry point
│   ├── config.h              # Hardware pin definitions
│   ├── hal/                  # Hardware Abstraction Layer
│   │   ├── display.h/.cpp    # E-Ink display driver
│   │   ├── input.h/.cpp      # Button input handling
│   │   ├── touch.h/.cpp      # Capacitive touchscreen
│   │   ├── frontlight.h/.cpp # Front light control
│   │   ├── storage.h/.cpp    # SD card storage
│   │   └── power.h/.cpp     # Power management
│   ├── ui/                   # User Interface
│   │   ├── activity.h        # Activity base class
│   │   ├── home_activity.h/.cpp    # Main menu
│   │   ├── txt_reader_activity.h/.cpp  # TXT reader
│   │   └── settings_activity.h/.cpp    # Settings
│   └── utils/                # Utilities
│       └── settings.h/.cpp   # Settings persistence
├── docs/
│   ├── hardware.md           # Hardware documentation
│   └── build.md              # Build instructions
├── platformio.ini           # PlatformIO configuration
├── partitions.csv            # Flash partition table
├── LICENSE                   # MIT License
└── README.md                 # This file
```

## Architecture

StarReader Pro uses a layered architecture:

1. **HAL Layer** - Hardware abstraction (display, touch, buttons, front light, storage, power)
2. **Input Layer** - Unified input event handling (touch + buttons)
3. **Activity Layer** - Screen management and navigation
4. **Application Layer** - Reading, settings, library

The activity-based navigation model provides a clean separation between screens, making it easy to add new features.

## Development Roadmap

- [x] Display driver (SSD1677)
- [x] Button input (resistor ladder ADC)
- [x] SD card storage
- [x] Power management (deep sleep)
- [x] TXT reader
- [x] Settings menu
- [x] Capacitive touchscreen driver
- [x] Dual-tone front light control
- [ ] Gesture navigation (swipe left/right/up/down)
- [ ] Pull-down quick settings panel
- [ ] EPUB reader
- [ ] File browser / library
- [ ] WiFi web server (book upload via magnetic)
- [ ] Custom sleep screens
- [ ] OTA updates

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.

## Acknowledgments

This project builds upon the work of the open-source e-reader community:

- [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader) - Pioneering open firmware
- [open-x4-epaper](https://github.com/open-x4-epaper) - Community SDK
- [sunwoods/Xteink-X4](https://github.com/sunwoods/Xteink-X4) - Hardware reverse engineering

## License

MIT License - see [LICENSE](LICENSE) file for details.

**Disclaimer:** This is unofficial community firmware. Use at your own risk. Always back up your original firmware before flashing.
