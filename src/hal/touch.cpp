/**
 * StarReader Pro Firmware - Touch Screen HAL Implementation
 */

#include "touch.h"
#include "../config.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <Wire.h>
#endif

// GT911 Register Map
#define GT911_REG_ID                0x8140
#define GT911_REG_FW_VER            0x8141
#define GT911_REG_NUM_POINTS        0x814E
#define GT911_REG_POINT_1           0x8150
#define GT911_REG_SWITCH_1          0x804D
#define GT911_REG_CONFIG_START      0x8050
#define GT911_REG_COMMAND           0x8040

// GT911 Commands
#define GT911_CMD_SOFT_RESET        0x05
#define GT911_CMD_READ_COORDS       0x80

HalTouch::HalTouch()
    : m_initialized(false)
    , m_touchActive(false)
    , m_gesturesEnabled(true)
    , m_sensitivity(70)
    , m_calibrated(false)
    , m_screenWidth(EPAPER_WIDTH)
    , m_screenHeight(EPAPER_HEIGHT)
    , m_lastPollTime(0) {
    m_lastEvent.gesture = GESTURE_NONE;
    m_lastEvent.duration = 0;
}

HalTouch::~HalTouch() {
    end();
}

bool HalTouch::begin() {
#ifdef ARDUINO
    // Initialize I2C for touch panel
    Wire.begin(TOUCH_I2C_SDA, TOUCH_I2C_SCL);
    Wire.setClock(400000);  // 400kHz fast mode

    // Touch interrupt pin
    pinMode(TOUCH_PIN_INT, INPUT_PULLUP);

    // Touch reset pin
    pinMode(TOUCH_PIN_RST, OUTPUT);
    digitalWrite(TOUCH_PIN_RST, LOW);
    delay(10);
    digitalWrite(TOUCH_PIN_RST, HIGH);
    delay(100);

    // Initialize GT911
    if (gt911Init()) {
        m_initialized = true;
        return true;
    }

    return false;
#else
    m_initialized = true;
    return true;
#endif
}

void HalTouch::end() {
#ifdef ARDUINO
    Wire.end();
#endif
    m_initialized = false;
    m_touchActive = false;
}

void HalTouch::update() {
#ifdef ARDUINO
    uint32_t now = millis();

    if (now - m_lastPollTime < TOUCH_POLL_INTERVAL_MS) {
        return;
    }
    m_lastPollTime = now;

    if (!m_initialized) return;

    // Check if touch data is available
    uint8_t intStatus = digitalRead(TOUCH_PIN_INT);

    if (intStatus == LOW) {
        // Touch event available
        TouchPoint points[5];
        uint8_t count = 0;

        if (gt911ReadTouch(points, &count)) {
            if (count > 0) {
                m_currentPoint = points[0];

                if (!m_touchActive) {
                    // Touch just started
                    m_touchActive = true;
                    m_startPoint = points[0];
                    m_touchStartTime = now;
                }
            } else {
                // Touch ended - calculate gesture
                if (m_touchActive) {
                    m_touchActive = false;
                    uint32_t duration = now - m_touchStartTime;

                    if (m_gesturesEnabled) {
                        m_lastEvent.gesture = detectGesture(
                            m_startPoint, m_currentPoint, duration);
                    } else {
                        m_lastEvent.gesture = GESTURE_TAP;
                    }

                    m_lastEvent.startPoint = m_startPoint;
                    m_lastEvent.endPoint = m_currentPoint;
                    m_lastEvent.duration = duration;
                }
            }
        }
    }
#endif
}

TouchEvent HalTouch::getLastEvent() {
    TouchEvent event = m_lastEvent;
    m_lastEvent.gesture = GESTURE_NONE;
    return event;
}

bool HalTouch::isTouchActive() const {
    return m_touchActive;
}

bool HalTouch::getCurrentTouch(TouchPoint* point) {
    if (m_touchActive && point) {
        *point = m_currentPoint;
        return true;
    }
    return false;
}

void HalTouch::calibrate() {
    // TODO: Implement touch calibration
    m_calibrated = true;
}

bool HalTouch::isCalibrated() const {
    return m_calibrated;
}

void HalTouch::setSensitivity(uint8_t sensitivity) {
    if (sensitivity > 100) sensitivity = 100;
    m_sensitivity = sensitivity;
}

uint8_t HalTouch::getSensitivity() const {
    return m_sensitivity;
}

void HalTouch::enableGestures(bool enable) {
    m_gesturesEnabled = enable;
}

bool HalTouch::areGesturesEnabled() const {
    return m_gesturesEnabled;
}

bool HalTouch::touchWrite(uint8_t reg, uint8_t data) {
#ifdef ARDUINO
    Wire.beginTransmission(TOUCH_I2C_ADDR);
    Wire.write(reg >> 8);
    Wire.write(reg & 0xFF);
    Wire.write(data);
    return (Wire.endTransmission() == 0);
#else
    return true;
#endif
}

bool HalTouch::touchRead(uint8_t reg, uint8_t* data, uint8_t len) {
#ifdef ARDUINO
    Wire.beginTransmission(TOUCH_I2C_ADDR);
    Wire.write(reg >> 8);
    Wire.write(reg & 0xFF);
    if (Wire.endTransmission(false) != 0) return false;

    uint8_t readLen = Wire.requestFrom(TOUCH_I2C_ADDR, len);
    if (readLen != len) return false;

    for (uint8_t i = 0; i < len; i++) {
        data[i] = Wire.read();
    }
    return true;
#else
    return false;
#endif
}

bool HalTouch::gt911Init() {
#ifdef ARDUINO
    // Read chip ID
    uint8_t id[2];
    if (!touchRead(GT911_REG_ID, id, 2)) {
        return false;
    }

    // GT911 ID should be 0x39 0x31 ('9', '1')
    if (id[0] != 0x39 || id[1] != 0x31) {
        // Try alternative address
        // TODO: Scan for touch IC address
        return false;
    }

    // Soft reset
    touchWrite(GT911_REG_COMMAND, GT911_CMD_SOFT_RESET);
    delay(50);

    return true;
#else
    return true;
#endif
}

bool HalTouch::gt911ReadTouch(TouchPoint* points, uint8_t* count) {
#ifdef ARDUINO
    uint8_t data[40];

    // Read number of touch points
    uint8_t numPoints;
    if (!touchRead(GT911_REG_NUM_POINTS, &numPoints, 1)) {
        return false;
    }

    numPoints &= 0x0F;  // Lower 4 bits = number of points
    *count = numPoints;

    if (numPoints == 0 || numPoints > 5) {
        // Clear status register
        touchWrite(GT911_REG_NUM_POINTS, 0x00);
        return true;
    }

    // Read touch point data
    if (!touchRead(GT911_REG_POINT_1, data, numPoints * 8)) {
        return false;
    }

    for (uint8_t i = 0; i < numPoints; i++) {
        uint8_t* p = &data[i * 8];
        points[i].x = p[1] | (p[2] << 8);
        points[i].y = p[3] | (p[4] << 8);
        points[i].pressure = p[5];
    }

    // Clear status register
    touchWrite(GT911_REG_NUM_POINTS, 0x00);

    return true;
#else
    *count = 0;
    return true;
#endif
}

TouchGesture HalTouch::detectGesture(const TouchPoint& start, const TouchPoint& end, uint32_t duration) {
    int16_t dx = end.x - start.x;
    int16_t dy = end.y - start.y;
    uint16_t absDx = abs(dx);
    uint16_t absDy = abs(dy);

    // Tap detection
    if (absDx < TOUCH_TAP_MAX_DISTANCE && absDy < TOUCH_TAP_MAX_DISTANCE) {
        if (duration < TOUCH_TAP_MAX_DURATION) {
            return GESTURE_TAP;
        } else if (duration > 1000) {
            return GESTURE_LONG_PRESS;
        }
    }

    // Swipe detection
    if (absDx > absDy) {
        // Horizontal swipe
        if (absDx > TOUCH_SWIPE_THRESHOLD) {
            if (dx > 0) {
                return GESTURE_SWIPE_RIGHT;  // Swipe right = prev page
            } else {
                return GESTURE_SWIPE_LEFT;   // Swipe left = next page
            }
        }
    } else {
        // Vertical swipe
        if (absDy > TOUCH_SWIPE_THRESHOLD) {
            if (dy > 0) {
                return GESTURE_SWIPE_DOWN;
            } else {
                return GESTURE_SWIPE_UP;
            }
        }
    }

    return GESTURE_NONE;
}
