/**
 * StarReader Firmware - Hardware Configuration
 * 
 * Target: Xteink X4 / X4 Pro (阅星瞳)
 * MCU: ESP32-C3
 * 
 * Pin definitions based on community reverse engineering:
 * - sample-firmware (open-x4-epaper)
 * - Xteink-X4 hardware repo (sunwoods)
 * - CrossPoint Reader HAL
 */

#ifndef STARREADER_CONFIG_H
#define STARREADER_CONFIG_H

// ==================== MCU Configuration ====================
#define HW_MCU_ESP32_C3

// ==================== E-Paper Display Pins ====================
// 4.26" E-Ink (800x480, GDEQ0426T82, SSD1677 controller)
#define EPAPER_PIN_SCLK     8
#define EPAPER_PIN_MOSI     10
#define EPAPER_PIN_CS       21
#define EPAPER_PIN_DC       4
#define EPAPER_PIN_RST      5
#define EPAPER_PIN_BUSY     6

// Display dimensions
#define EPAPER_WIDTH        800
#define EPAPER_HEIGHT       480

// Display refresh modes
#define REFRESH_FULL        0
#define REFRESH_PARTIAL     1
#define REFRESH_FAST        2

// ==================== microSD Card Pins ====================
// Shared SPI bus with EPD
#define SD_PIN_CS           12
#define SD_PIN_MISO         7
#define SD_PIN_MOSI         10
#define SD_PIN_SCK          8

// ==================== Button Input Pins ====================
// Resistor ladder on ADC pins
#define BTN_ADC_PIN_1       1   // 4 front buttons
#define BTN_ADC_PIN_2       2   // 2 side buttons

// Button ADC thresholds (GPIO1)
#define BTN_BACK_MIN        3200
#define BTN_BACK_MAX        3700
#define BTN_CONFIRM_MIN     2400
#define BTN_CONFIRM_MAX     2900
#define BTN_LEFT_MIN        1200
#define BTN_LEFT_MAX        1700
#define BTN_RIGHT_MIN       0
#define BTN_RIGHT_MAX       200

// Button ADC thresholds (GPIO2)
#define BTN_VOLUP_MIN       2000
#define BTN_VOLUP_MAX       2500
#define BTN_VOLDOWN_MIN     0
#define BTN_VOLDOWN_MAX     200

// Power button
#define BTN_POWER_PIN       3

// ==================== Battery Monitoring ====================
#define BATTERY_ADC_PIN     0   // Voltage divider (2x10K), reads 1/2 voltage
#define BATTERY_DIVIDER_RATIO 2.0f

// USB detection
#define USB_DETECT_PIN      20  // UART0_RXD

// ==================== Touch Panel (X4 Pro only) ====================
// TODO: Verify touch panel pins for X4 Pro
#define TOUCH_I2C_SDA       -1  // TBD
#define TOUCH_I2C_SCL       -1  // TBD
#define TOUCH_INT_PIN       -1  // TBD

// ==================== Front Light (X4 Pro only) ====================
// TODO: Verify front light control pins for X4 Pro
#define FRONT_LIGHT_PWM_PIN -1  // TBD
#define FRONT_LIGHT_WARM_PIN -1 // TBD

// ==================== Timing ====================
#define BUTTON_POLL_INTERVAL_MS     50
#define DEBOUNCE_COUNT              2
#define AUTO_SLEEP_TIMEOUT_MS       300000  // 5 minutes

// ==================== SD Card Paths ====================
#define SD_ROOT_PATH            "/"
#define SD_SYSTEM_DIR          "/.starreader"
#define SD_SETTINGS_FILE       "/.starreader/settings.bin"
#define SD_STATE_FILE          "/.starreader/state.bin"
#define SD_RECENT_FILE         "/.starreader/recent.bin"
#define SD_BOOK_CACHE_DIR      "/.starreader/cache"
#define SD_SLEEP_IMAGES_DIR    "/sleep"

#endif // STARREADER_CONFIG_H
