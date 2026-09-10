# Build & Flash Guide

This guide covers building StarReader firmware from source and flashing it to your Xteink X4 / X4 Pro device.

## Prerequisites

### Software Requirements

1. **PlatformIO Core** (or PlatformIO IDE extension)
   - Install via pip: `pip install platformio`
   - Or use VS Code with PlatformIO extension

2. **USB Drivers**
   - CP210x or CH340 driver (depending on USB-UART chip)
   - Usually auto-installed on modern OSes

3. **esptool** (for advanced flashing)
   - `pip install esptool`

### Hardware Requirements

- Xteink X4 / X4 Pro
- USB-C data cable (not charge-only!)
- Computer (Windows/macOS/Linux)

## Building

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/starreader-firmware.git
cd starreader-firmware
```

### 2. Configure Environment

Edit `platformio.ini` if needed:

```ini
# Update upload port for your system
upload_port = /dev/ttyACM0       # Linux
# upload_port = COM3             # Windows
# upload_port = /dev/cu.usbserial-0001  # macOS
```

### 3. Build Firmware

```bash
# Debug build
platformio run

# Release build (optimized)
platformio run --environment release
```

The compiled firmware binary will be at:
```
.pio/build/esp32-c3-devkitm-1/firmware.bin
```

## Flashing

### Method 1: PlatformIO (Recommended)

1. Put device into flash mode:
   - Hold the **BOOT** button (or special key combo)
   - Connect USB cable to computer
   - Release button after 2 seconds

2. Flash:
```bash
platformio run -t upload
```

3. Monitor output:
```bash
platformio device monitor
```

### Method 2: esptool.py (Manual)

For advanced users or recovery:

```bash
# Full flash (16MB)
python -m esptool --chip esp32c3 --port /dev/ttyACM0 --baud 921600 write_flash 0x0 firmware.bin

# App partition only (faster)
python -m esptool --chip esp32c3 --port /dev/ttyACM0 --baud 921600 write_flash 0x10000 firmware.bin
```

### Method 3: SD Card Recovery

If the device is bricked:

1. Copy firmware binary to SD card as `update.bin`
2. Insert SD card into device
3. Power on while holding recovery button combo
4. Firmware will flash automatically

## Backup & Restore

### Backup Original Firmware

**Important:** Always back up your original firmware before flashing custom firmware!

```bash
# Read entire 16MB flash
python -m esptool --chip esp32c3 --port /dev/ttyACM0 read_flash 0x0 0x1000000 firmware_backup.bin

# Read only app partition (faster)
python -m esptool --chip esp32c3 --port /dev/ttyACM0 read_flash 0x10000 0x640000 app_backup.bin
```

### Restore Original Firmware

```bash
# Restore full backup
python -m esptool --chip esp32c3 --port /dev/ttyACM0 write_flash 0x0 firmware_backup.bin

# Restore app only
python -m esptool --chip esp32c3 --port /dev/ttyACM0 write_flash 0x10000 app_backup.bin
```

## OTA Updates

StarReader supports Over-The-Air updates via WiFi.

### Switching OTA Partitions

```bash
# Backup OTA data
python -m esptool --chip esp32c3 --port /dev/ttyACM0 read_flash 0xE000 0x2000 otadata_backup.bin

# Boot from app0
python -m esptool --chip esp32c3 --port /dev/ttyACM0 write_flash 0xE000 otadata_boot_app0.bin

# Boot from app1
python -m esptool --chip esp32c3 --port /dev/ttyACM0 write_flash 0xE000 otadata_boot_app1.bin
```

## Troubleshooting

### Device not detected

- Try a different USB cable (must be data cable, not charge-only)
- Try a different USB port
- Install CP210x/CH340 drivers
- On Linux: add user to `dialout` group

### Flash fails

- Lower baud rate: `--baud 115200`
- Ensure device is in flash mode (hold BOOT while plugging in)
- Check that the correct port is selected

### Garbled display

- Verify display orientation setting
- Try full refresh mode
- Check that GxEPD2 display class matches your panel

### Buttons not working

- Check ADC calibration values in `config.h`
- Adjust threshold ranges for your hardware revision
- Verify button polarity

### SD card not detected

- Check that SD card is FAT32 formatted
- Try a different SD card (some cards are incompatible)
- Verify SPI pin definitions

## Development Tips

### Debug Output

Enable verbose logging:
```cpp
#define CORE_DEBUG_LEVEL 3
```

### Memory Profiling

```cpp
Serial.printf("Free heap: %d bytes\n", esp_get_free_heap_size());
```

### Adding New Features

1. Create a new `Activity` subclass
2. Implement lifecycle methods (`onEnter`, `loop`, `render`)
3. Add navigation entry from existing activity
4. Test on real hardware

## Resources

- [ESP32-C3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [GxEPD2 Library](https://github.com/ZinggJM/GxEPD2)
- [esptool Documentation](https://docs.espressif.com/projects/esptool/)
