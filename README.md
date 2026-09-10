# StarReader Firmware

An open-source e-reader firmware for **Xteink X4 / X4 Pro** (阅星瞳) e-paper devices.

## Overview

StarReader is a community-driven, fully open-source firmware designed for the Xteink X4 series of ultra-portable e-ink readers. Built from the ground up on the ESP32-C3 platform, it provides a clean, focused reading experience while remaining fully hackable and extensible.

**Target Hardware:**
- Xteink X4 (阅星瞳 X4)
- Xteink X4 Pro (阅星瞳 X4 Pro)

## Features

### Core Reading
- **TXT Reader** - Plain text file reader with page navigation
- **EPUB Support** - EPUB 2/3 rendering (planned)
- **Custom Fonts** - Install fonts from SD card
- **Layout Controls** - Margins, line spacing, justification
- **Reading Progress** - Automatic position saving

### Display
- **4.26" E-Ink** - 800×480 monochrome display (GDEQ0426T82)
- **Multiple Refresh Modes** - Full, Partial, Fast
- **Orientation Support** - Portrait / Landscape / Inverted
- **Status Bar** - Battery, page count, USB status

### Power Management
- **Deep Sleep** - Ultra-low power consumption
- **Auto Sleep** - Configurable timeout
- **Battery Monitoring** - Voltage and percentage readout
- **Wake on Power Button** - Instant resume

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
| Display | 4.26" E-Ink, 800×480, SSD1677 |
| Storage | microSD card slot |
| Battery | 650mAh Li-ion |
| Weight | ~70g |

For detailed pin definitions, see [docs/hardware.md](docs/hardware.md).

## Quick Start

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- Xteink X4 / X4 Pro device
- USB-C cable

### Build & Flash

```bash
# Clone the repository
git clone https://github.com/your-username/starreader-firmware.git
cd starreader-firmware

# Build
platformio run

# Flash (hold BOOT button while connecting USB)
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

StarReader uses a layered architecture:

1. **HAL Layer** - Hardware abstraction (display, input, storage, power)
2. **Activity Layer** - Screen management and navigation
3. **Application Layer** - Reading, settings, library

The activity-based navigation model provides a clean separation between screens, making it easy to add new features.

## Development Roadmap

- [x] Display driver (SSD1677)
- [x] Button input (resistor ladder ADC)
- [x] SD card storage
- [x] Power management (deep sleep)
- [x] TXT reader
- [x] Settings menu
- [ ] EPUB reader
- [ ] File browser / library
- [ ] WiFi web server (book upload)
- [ ] Touch panel support (X4 Pro)
- [ ] Front light control (X4 Pro)
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
