/**
 * StarReader Firmware - Input HAL
 * 
 * Handles button input via resistor ladder on ADC pins
 */

#ifndef STARREADER_HAL_INPUT_H
#define STARREADER_HAL_INPUT_H

#include <stdint.h>
#include "../config.h"

enum ButtonId {
    BTN_NONE = 0,
    BTN_BACK,
    BTN_CONFIRM,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_VOL_UP,
    BTN_VOL_DOWN,
    BTN_POWER
};

enum ButtonEvent {
    EVENT_NONE = 0,
    EVENT_PRESSED,
    EVENT_RELEASED,
    EVENT_LONG_PRESS
};

struct ButtonState {
    ButtonId id;
    ButtonEvent event;
    uint32_t pressDuration;  // in ms
};

class HalInput {
public:
    HalInput();
    ~HalInput();

    bool begin();
    void end();

    // Poll for button events - call frequently
    void update();

    // Check if any button is currently pressed
    bool isPressed(ButtonId id) const;

    // Get the last button event (non-blocking)
    ButtonState getLastEvent();

    // Wait for button press (blocking)
    ButtonId waitForButton(uint32_t timeoutMs = 0);

    // Power button handling
    bool isPowerButtonPressed() const;
    uint32_t getPowerButtonHoldTime() const;

private:
    // Read ADC values
    uint16_t readAdcPin(uint8_t pin);

    // Identify button from ADC value
    ButtonId identifyButton(uint8_t adcPin, uint16_t value);

    // Button state tracking
    struct ButtonTrack {
        ButtonId id;
        bool pressed;
        uint32_t pressStartTime;
        uint32_t pressDuration;
        uint8_t debounceCount;
        bool longPressFired;
    };

    ButtonTrack m_buttons[7];  // 7 buttons total
    ButtonState m_lastEvent;
    uint32_t m_lastPollTime;

    // Configuration
    uint16_t m_adcResolution;
    float m_adcVref;
};

#endif // STARREADER_HAL_INPUT_H
