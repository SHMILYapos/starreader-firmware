/**
 * StarReader Pro Firmware - Hardware Configuration
 * 
 * Target: Xteink X4 Pro (阅星瞳 X4 Pro)
 * MCU: ESP32-C3
 * 
 * Pin definitions based on community reverse engineering:
 * - sample-firmware (open-x4-epaper)
 * - Xteink-X4 hardware repo (sunwoods)
 * - CrossPoint Reader HAL
 * - X4 Pro product specs (touch + front light)
 */

#ifndef STARREADER_CONFIG_H
#define STARREADER_CONFIG_H

// ==================== MCU Configuration ====================
#define HW_MCU_ESP32_C3
#define HW_DEVICE_X4_PRO

// ==================== E-Paper Display Pins ====================
// 4.3" E-Ink (800x480, GDEQ0426T82, SSD1677 controller)
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

// ==================== Touch Panel (X4 Pro) ====================
// Capacitive touchscreen - I2C interface
// Common touch IC: GT911 / FT6336U
#define TOUCH_I2C_SDA       9
#define TOUCH_I2C_SCL       10
#define TOUCH_PIN_INT       7
#define TOUCH_PIN_RST       2

// Touch I2C address (GT911: 0x5D or 0x14, FT6336: 0x38)
#define TOUCH_I2C_ADDR      0x5D

// Touch gesture thresholds
#define TOUCH_SWIPE_THRESHOLD   30  // pixels
#define TOUCH_TAP_MAX_DURATION  300 // ms
#define TOUCH_TAP_MAX_DISTANCE  10  // pixels

// ==================== Front Light (X4 Pro) ====================
// Dual-tone front light: warm + cool white
#define FRONT_LIGHT_PWM_WARM    1   // Warm white LED PWM
#define FRONT_LIGHT_PWM_COOL    2   // Cool white LED PWM

// Front light PWM configuration
#define FRONT_LIGHT_PWM_FREQ    5000    // 5kHz
#define FRONT_LIGHT_PWM_RES     10      // 10-bit resolution (0-1023)
#define FRONT_LIGHT_MAX_BRIGHT  100     // 0-100%

// ==================== microSD Card Pins ====================
// Shared SPI bus with EPD
#define SD_PIN_CS           12
#define SD_PIN_MISO         7
#define SD_PIN_MOSI         10
#define SD_PIN_SCK          8

// ==================== Button Input Pins ====================
// Resistor ladder on ADC pins
// X4 Pro: side buttons + capacitive home key
#define BTN_ADC_PIN_1       1   // Side buttons (Left/Right page turn)
#define BTN_ADC_PIN_2       2   // Additional buttons

// Button ADC thresholds (GPIO1 - side buttons)
#define BTN_LEFT_MIN        1200
#define BTN_LEFT_MAX        1700
#define BTN_RIGHT_MIN       0
#define BTN_RIGHT_MAX       200

// Button ADC thresholds (GPIO2)
#define BTN_CONFIRM_MIN     2400
#define BTN_CONFIRM_MAX     2900
#define BTN_BACK_MIN        3200
#define BTN_BACK_MAX        3700

// Power button
#define BTN_POWER_PIN       3

// Capacitive home button (X4 Pro)
#define HOME_BUTTON_PIN     20  // TBD - capacitive touch IC

// ==================== Battery Monitoring ====================
#define BATTERY_ADC_PIN     0   // Voltage divider (2x10K), reads 1/2 voltage
#define BATTERY_DIVIDER_RATIO 2.0f

// Magnetic charging detection
#define MAGNETIC_DET_PIN    20  // USB_DETECT_PIN equivalent

// ==================== Timing ====================
#define BUTTON_POLL_INTERVAL_MS     50
#define TOUCH_POLL_INTERVAL_MS      30
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

// ==================== Front Light Defaults ====================
#define DEFAULT_BRIGHTNESS      50      // 0-100%
#define DEFAULT_WARMTH          50      // 0=cool, 100=warm
#define DEFAULT_FRONT_LIGHT_ON  false

#endif // STARREADER_CONFIG_H
