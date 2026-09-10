/**
 * StarReader Pro Firmware - Touch Screen HAL
 * 
 * Capacitive touchscreen driver for X4 Pro
 * Supports GT911 / FT6336 compatible touch ICs via I2C
 */

#ifndef STARREADER_HAL_TOUCH_H
#define STARREADER_HAL_TOUCH_H

#include <stdint.h>
#include "../config.h"

// Touch gesture types
enum TouchGesture {
    GESTURE_NONE = 0,
    GESTURE_TAP,           // Single tap
    GESTURE_DOUBLE_TAP,    // Double tap
    GESTURE_SWIPE_LEFT,    // Swipe left (next page)
    GESTURE_SWIPE_RIGHT,   // Swipe right (prev page)
    GESTURE_SWIPE_UP,      // Swipe up (open menu)
    GESTURE_SWIPE_DOWN,    // Swipe down (quick settings)
    GESTURE_LONG_PRESS     // Long press
};

// Touch point data
struct TouchPoint {
    uint16_t x;
    uint16_t y;
    uint8_t pressure;
};

// Touch event
struct TouchEvent {
    TouchGesture gesture;
    TouchPoint startPoint;
    TouchPoint endPoint;
    uint32_t duration;  // ms
};

class HalTouch {
public:
    HalTouch();
    ~HalTouch();

    bool begin();
    void end();

    // Poll for touch events - call frequently
    void update();

    // Get last touch event (non-blocking)
    TouchEvent getLastEvent();

    // Check if touch is currently active
    bool isTouchActive() const;

    // Get current touch point (if touching)
    bool getCurrentTouch(TouchPoint* point);

    // Calibration
    void calibrate();
    bool isCalibrated() const;

    // Touch settings
    void setSensitivity(uint8_t sensitivity);  // 0-100
    uint8_t getSensitivity() const;

    void enableGestures(bool enable);
    bool areGesturesEnabled() const;

private:
    // I2C communication
    bool touchWrite(uint8_t reg, uint8_t data);
    bool touchRead(uint8_t reg, uint8_t* data, uint8_t len);

    // GT911 specific functions
    bool gt911Init();
    bool gt911ReadTouch(TouchPoint* points, uint8_t* count);

    // Gesture detection
    TouchGesture detectGesture(const TouchPoint& start, const TouchPoint& end, uint32_t duration);

    // State tracking
    bool m_initialized;
    bool m_touchActive;
    bool m_gesturesEnabled;
    uint8_t m_sensitivity;
    bool m_calibrated;

    // Touch tracking
    TouchPoint m_startPoint;
    uint32_t m_touchStartTime;
    TouchPoint m_currentPoint;
    TouchEvent m_lastEvent;

    // Configuration
    uint16_t m_screenWidth;
    uint16_t m_screenHeight;

    // Last poll time
    uint32_t m_lastPollTime;
};

#endif // STARREADER_HAL_TOUCH_H
