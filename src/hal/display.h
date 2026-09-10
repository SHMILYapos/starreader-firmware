/**
 * StarReader Firmware - E-Paper Display HAL
 * 
 * Provides display abstraction for the 4.26" E-Ink panel
 * (GDEQ0426T82 / SSD1677 controller)
 */

#ifndef STARREADER_HAL_DISPLAY_H
#define STARREADER_HAL_DISPLAY_H

#include <stdint.h>
#include <stddef.h>
#include "../config.h"

class HalDisplay {
public:
    enum Orientation {
        PORTRAIT = 0,
        LANDSCAPE = 1,
        PORTRAIT_INVERTED = 2,
        LANDSCAPE_INVERTED = 3
    };

    enum RefreshMode {
        FULL_REFRESH = 0,
        PARTIAL_REFRESH = 1,
        FAST_REFRESH = 2
    };

    HalDisplay();
    ~HalDisplay();

    // Initialization
    bool begin();
    void end();

    // Display dimensions
    uint16_t getWidth() const;
    uint16_t getHeight() const;
    uint16_t getRotatedWidth() const;
    uint16_t getRotatedHeight() const;

    // Orientation
    void setOrientation(Orientation orientation);
    Orientation getOrientation() const;

    // Framebuffer access
    uint8_t* getFramebuffer();
    size_t getFramebufferSize() const;

    // Drawing primitives (draw to framebuffer)
    void clear(uint8_t color = 0xFF);  // 0xFF = white, 0x00 = black
    void drawPixel(int16_t x, int16_t y, uint8_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);

    // Text rendering
    void drawChar(int16_t x, int16_t y, char c, uint8_t color, uint8_t size = 1);
    void drawString(int16_t x, int16_t y, const char* str, uint8_t color, uint8_t size = 1);
    int16_t getStringWidth(const char* str, uint8_t size = 1);

    // Refresh the physical display
    void refresh(RefreshMode mode = FULL_REFRESH);

    // Power management
    void deepSleep();
    void wakeUp();

    // Status
    bool isInitialized() const;
    bool isBusy() const;

private:
    // Low-level display commands
    void sendCommand(uint8_t command);
    void sendData(uint8_t data);
    void sendDataBuffer(const uint8_t* data, size_t length);
    void waitUntilIdle();
    void reset();

    // Framebuffer
    uint8_t* m_framebuffer;
    uint16_t m_width;
    uint16_t m_height;
    Orientation m_orientation;
    bool m_initialized;
    bool m_busy;

    // Font data (built-in 5x7)
    static const uint8_t font5x7[][5];
    static const uint8_t font5x7_height;
    static const uint8_t font5x7_width;
};

#endif // STARREADER_HAL_DISPLAY_H
