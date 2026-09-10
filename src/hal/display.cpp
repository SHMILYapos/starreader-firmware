/**
 * StarReader Firmware - E-Paper Display HAL Implementation
 */

#include "display.h"
#include "../config.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#endif

// SSD1677 Commands
#define SSD1677_DRIVER_CONTROL          0x01
#define SSD1677_GATE_VOLTAGE            0x03
#define SSD1677_SOURCE_VOLTAGE          0x04
#define SSD1677_DEEP_SLEEP              0x10
#define SSD1677_DATA_ENTRY_MODE         0x11
#define SSD1677_SW_RESET                0x12
#define SSD1677_TEMPERATURE_SENSOR      0x1A
#define SSD1677_MASTER_ACTIVATION       0x20
#define SSD1677_DISPLAY_UPDATE_CTRL_1   0x21
#define SSD1677_DISPLAY_UPDATE_CTRL_2   0x22
#define SSD1677_WRITE_RAM               0x24
#define SSD1677_WRITE_RED_RAM           0x26
#define SSD1677_VCOM_SENSE              0x28
#define SSD1677_VCOM_SENSE_DURATION     0x29
#define SSD1677_PROGRAM_VCOM            0x2A
#define SSD1677_VCOM_VOLTAGE            0x2C
#define SSD1677_PROGRAM_WS_OTP          0x30
#define SSD1677_LUT_REGISTER            0x32
#define SSD1677_PROGRAM_OTP_WS          0x36
#define SSD1677_WRITE_LUT_REGISTER      0x37
#define SSD1677_SET_DUMMY_LINE          0x3A
#define SSD1677_SET_GATE_TIME           0x3B
#define SSD1677_BORDER_WAVEFORM         0x3C
#define SSD1677_SET_RAM_X               0x44
#define SSD1677_SET_RAM_Y               0x45
#define SSD1677_SET_RAM_X_COUNTER       0x4E
#define SSD1677_SET_RAM_Y_COUNTER       0x4F

// Built-in 5x7 font (ASCII 32-127)
const uint8_t HalDisplay::font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // space
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // F
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
};

const uint8_t HalDisplay::font5x7_height = 7;
const uint8_t HalDisplay::font5x7_width = 5;

HalDisplay::HalDisplay()
    : m_framebuffer(nullptr)
    , m_width(EPAPER_WIDTH)
    , m_height(EPAPER_HEIGHT)
    , m_orientation(LANDSCAPE)
    , m_initialized(false)
    , m_busy(false) {
}

HalDisplay::~HalDisplay() {
    if (m_framebuffer) {
        delete[] m_framebuffer;
        m_framebuffer = nullptr;
    }
}

bool HalDisplay::begin() {
#ifdef ARDUINO
    // Allocate framebuffer: 1 bit per pixel
    size_t fbSize = (m_width * m_height) / 8;
    m_framebuffer = new uint8_t[fbSize];
    if (!m_framebuffer) {
        return false;
    }
    memset(m_framebuffer, 0xFF, fbSize);  // Start with white screen

    // Initialize SPI pins
    pinMode(EPAPER_PIN_CS, OUTPUT);
    pinMode(EPAPER_PIN_DC, OUTPUT);
    pinMode(EPAPER_PIN_RST, OUTPUT);
    pinMode(EPAPER_PIN_BUSY, INPUT);

    digitalWrite(EPAPER_PIN_CS, HIGH);
    digitalWrite(EPAPER_PIN_DC, HIGH);

    // Reset display
    reset();

    // Initialize display
    sendCommand(SSD1677_SW_RESET);
    delay(10);
    waitUntilIdle();

    // Driver output control
    sendCommand(SSD1677_DRIVER_CONTROL);
    sendData(0x2B);  // Gate 295
    sendData(0x01);
    sendData(0x00);

    // Data entry mode
    sendCommand(SSD1677_DATA_ENTRY_MODE);
    sendData(0x03);  // Increment X, increment Y

    // Set RAM window
    sendCommand(SSD1677_SET_RAM_X);
    sendData(0x00);
    sendData(0x31);  // 800/8-1 = 99 = 0x63? Adjusted for actual panel

    sendCommand(SSD1677_SET_RAM_Y);
    sendData(0x00);
    sendData(0x00);
    sendData(0xDF);  // 479 = 0x1DF
    sendData(0x01);

    // Border waveform
    sendCommand(SSD1677_BORDER_WAVEFORM);
    sendData(0x01);

    // Set dummy line
    sendCommand(SSD1677_SET_DUMMY_LINE);
    sendData(0x1B);

    // Set gate time
    sendCommand(SSD1677_SET_GATE_TIME);
    sendData(0x0B);

    // VCOM voltage
    sendCommand(SSD1677_VCOM_VOLTAGE);
    sendData(0x26);

    m_initialized = true;
    return true;
#else
    // Simulation mode
    size_t fbSize = (m_width * m_height) / 8;
    m_framebuffer = new uint8_t[fbSize];
    if (!m_framebuffer) {
        return false;
    }
    memset(m_framebuffer, 0xFF, fbSize);
    m_initialized = true;
    return true;
#endif
}

void HalDisplay::end() {
    m_initialized = false;
    if (m_framebuffer) {
        delete[] m_framebuffer;
        m_framebuffer = nullptr;
    }
}

uint16_t HalDisplay::getWidth() const {
    return m_width;
}

uint16_t HalDisplay::getHeight() const {
    return m_height;
}

uint16_t HalDisplay::getRotatedWidth() const {
    return (m_orientation == LANDSCAPE || m_orientation == LANDSCAPE_INVERTED)
        ? m_width : m_height;
}

uint16_t HalDisplay::getRotatedHeight() const {
    return (m_orientation == LANDSCAPE || m_orientation == LANDSCAPE_INVERTED)
        ? m_height : m_width;
}

void HalDisplay::setOrientation(Orientation orientation) {
    m_orientation = orientation;
}

HalDisplay::Orientation HalDisplay::getOrientation() const {
    return m_orientation;
}

uint8_t* HalDisplay::getFramebuffer() {
    return m_framebuffer;
}

size_t HalDisplay::getFramebufferSize() const {
    return (m_width * m_height) / 8;
}

void HalDisplay::clear(uint8_t color) {
    if (!m_framebuffer) return;
    memset(m_framebuffer, color, getFramebufferSize());
}

void HalDisplay::drawPixel(int16_t x, int16_t y, uint8_t color) {
    if (!m_framebuffer) return;
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;

    // Apply rotation
    int16_t rx = x, ry = y;
    switch (m_orientation) {
        case LANDSCAPE:
            rx = x; ry = y;
            break;
        case PORTRAIT:
            rx = y; ry = m_width - 1 - x;
            break;
        case LANDSCAPE_INVERTED:
            rx = m_width - 1 - x; ry = m_height - 1 - y;
            break;
        case PORTRAIT_INVERTED:
            rx = m_height - 1 - y; ry = x;
            break;
    }

    uint16_t byteIndex = (ry * m_width + rx) / 8;
    uint8_t bitMask = 0x80 >> (rx % 8);

    if (color == 0x00) {  // Black
        m_framebuffer[byteIndex] &= ~bitMask;
    } else {  // White
        m_framebuffer[byteIndex] |= bitMask;
    }
}

void HalDisplay::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
    for (int16_t j = y; j < y + h; j++) {
        for (int16_t i = x; i < x + w; i++) {
            drawPixel(i, j, color);
        }
    }
}

void HalDisplay::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
    drawLine(x, y, x + w - 1, y, color);
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    drawLine(x, y, x, y + h - 1, color);
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void HalDisplay::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
    int16_t dx = abs(x1 - x0);
    int16_t dy = -abs(y1 - y0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;

    while (true) {
        drawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void HalDisplay::drawChar(int16_t x, int16_t y, char c, uint8_t color, uint8_t size) {
    if (c < 32 || c > 126) c = '?';
    uint8_t idx = c - 32;

    for (uint8_t i = 0; i < font5x7_width; i++) {
        uint8_t line = font5x7[idx][i];
        for (uint8_t j = 0; j < font5x7_height; j++) {
            if (line & 0x01) {
                fillRect(x + i * size, y + j * size, size, size, color);
            }
            line >>= 1;
        }
    }
}

void HalDisplay::drawString(int16_t x, int16_t y, const char* str, uint8_t color, uint8_t size) {
    int16_t cx = x;
    while (*str) {
        drawChar(cx, y, *str, color, size);
        cx += (font5x7_width + 1) * size;
        str++;
    }
}

int16_t HalDisplay::getStringWidth(const char* str, uint8_t size) {
    int16_t width = 0;
    while (*str++) {
        width += (font5x7_width + 1) * size;
    }
    return width;
}

void HalDisplay::refresh(RefreshMode mode) {
    if (!m_initialized || !m_framebuffer) return;

#ifdef ARDUINO
    waitUntilIdle();

    // Set RAM pointer
    sendCommand(SSD1677_SET_RAM_X_COUNTER);
    sendData(0x00);
    sendCommand(SSD1677_SET_RAM_Y_COUNTER);
    sendData(0x00);
    sendData(0x00);

    // Write framebuffer
    sendCommand(SSD1677_WRITE_RAM);
    digitalWrite(EPAPER_PIN_DC, HIGH);
    digitalWrite(EPAPER_PIN_CS, LOW);

    for (size_t i = 0; i < getFramebufferSize(); i++) {
        SPI.transfer(m_framebuffer[i]);
    }

    digitalWrite(EPAPER_PIN_CS, HIGH);

    // Update display
    switch (mode) {
        case FULL_REFRESH:
            sendCommand(SSD1677_DISPLAY_UPDATE_CTRL_2);
            sendData(0xF7);  // Full refresh
            break;
        case PARTIAL_REFRESH:
            sendCommand(SSD1677_DISPLAY_UPDATE_CTRL_2);
            sendData(0xFF);  // Partial
            break;
        case FAST_REFRESH:
            sendCommand(SSD1677_DISPLAY_UPDATE_CTRL_2);
            sendData(0xC7);  // Fast
            break;
    }

    sendCommand(SSD1677_MASTER_ACTIVATION);
    waitUntilIdle();
#endif
}

void HalDisplay::deepSleep() {
#ifdef ARDUINO
    sendCommand(SSD1677_DEEP_SLEEP);
    sendData(0x01);  // Deep sleep mode 1
#endif
}

void HalDisplay::wakeUp() {
#ifdef ARDUINO
    reset();
    delay(10);
#endif
}

bool HalDisplay::isInitialized() const {
    return m_initialized;
}

bool HalDisplay::isBusy() const {
#ifdef ARDUINO
    return digitalRead(EPAPER_PIN_BUSY) == HIGH;
#else
    return false;
#endif
}

void HalDisplay::sendCommand(uint8_t command) {
#ifdef ARDUINO
    digitalWrite(EPAPER_PIN_DC, LOW);
    digitalWrite(EPAPER_PIN_CS, LOW);
    SPI.transfer(command);
    digitalWrite(EPAPER_PIN_CS, HIGH);
#endif
}

void HalDisplay::sendData(uint8_t data) {
#ifdef ARDUINO
    digitalWrite(EPAPER_PIN_DC, HIGH);
    digitalWrite(EPAPER_PIN_CS, LOW);
    SPI.transfer(data);
    digitalWrite(EPAPER_PIN_CS, HIGH);
#endif
}

void HalDisplay::sendDataBuffer(const uint8_t* data, size_t length) {
#ifdef ARDUINO
    digitalWrite(EPAPER_PIN_DC, HIGH);
    digitalWrite(EPAPER_PIN_CS, LOW);
    for (size_t i = 0; i < length; i++) {
        SPI.transfer(data[i]);
    }
    digitalWrite(EPAPER_PIN_CS, HIGH);
#endif
}

void HalDisplay::waitUntilIdle() {
#ifdef ARDUINO
    while (digitalRead(EPAPER_PIN_BUSY) == HIGH) {
        delay(10);
    }
#endif
}

void HalDisplay::reset() {
#ifdef ARDUINO
    digitalWrite(EPAPER_PIN_RST, HIGH);
    delay(20);
    digitalWrite(EPAPER_PIN_RST, LOW);
    delay(2);
    digitalWrite(EPAPER_PIN_RST, HIGH);
    delay(20);
#endif
}
