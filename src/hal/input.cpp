/**
 * StarReader Firmware - Input HAL Implementation
 */

#include "input.h"
#include "../config.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

HalInput::HalInput()
    : m_lastPollTime(0)
    , m_adcResolution(4095)
    , m_adcVref(3.3f) {
    // Initialize button tracking
    for (int i = 0; i < 7; i++) {
        m_buttons[i].id = BTN_NONE;
        m_buttons[i].pressed = false;
        m_buttons[i].pressStartTime = 0;
        m_buttons[i].debounceCount = 0;
        m_buttons[i].longPressFired = false;
    }
    m_lastEvent.id = BTN_NONE;
    m_lastEvent.event = EVENT_NONE;
    m_lastEvent.pressDuration = 0;
}

HalInput::~HalInput() {
}

bool HalInput::begin() {
#ifdef ARDUINO
    // Configure ADC pins
    pinMode(BTN_ADC_PIN_1, INPUT);
    pinMode(BTN_ADC_PIN_2, INPUT);
    pinMode(BTN_POWER_PIN, INPUT_PULLUP);

    // Set ADC resolution (ESP32-C3 default is 12-bit)
    analogReadResolution(12);

    return true;
#else
    return true;
#endif
}

void HalInput::end() {
}

void HalInput::update() {
#ifdef ARDUINO
    uint32_t now = millis();

    // Poll at configured interval
    if (now - m_lastPollTime < BUTTON_POLL_INTERVAL_MS) {
        return;
    }
    m_lastPollTime = now;

    // Read ADC pins
    uint16_t adc1 = analogRead(BTN_ADC_PIN_1);
    uint16_t adc2 = analogRead(BTN_ADC_PIN_2);

    // Identify buttons
    ButtonId btn1 = identifyButton(1, adc1);
    ButtonId btn2 = identifyButton(2, adc2);

    // Check power button (digital)
    bool powerPressed = (digitalRead(BTN_POWER_PIN) == LOW);

    // Update button states with debouncing
    // Front buttons (GPIO1)
    ButtonId frontButtons[] = {BTN_BACK, BTN_CONFIRM, BTN_LEFT, BTN_RIGHT};
    for (int i = 0; i < 4; i++) {
        ButtonId expectedBtn = frontButtons[i];
        bool isNowPressed = (btn1 == expectedBtn);
        ButtonTrack* track = &m_buttons[i];

        if (isNowPressed != track->pressed) {
            track->debounceCount++;
            if (track->debounceCount >= DEBOUNCE_COUNT) {
                track->debounceCount = 0;
                track->pressed = isNowPressed;

                if (isNowPressed) {
                    track->pressStartTime = now;
                    track->longPressFired = false;
                    m_lastEvent.id = expectedBtn;
                    m_lastEvent.event = EVENT_PRESSED;
                    m_lastEvent.pressDuration = 0;
                } else {
                    track->pressDuration = now - track->pressStartTime;
                    m_lastEvent.id = expectedBtn;
                    m_lastEvent.event = EVENT_RELEASED;
                    m_lastEvent.pressDuration = track->pressDuration;
                }
            }
        } else {
            track->debounceCount = 0;
        }

        // Check for long press
        if (track->pressed && !track->longPressFired) {
            if (now - track->pressStartTime > 1000) {  // 1 second long press
                track->longPressFired = true;
                m_lastEvent.id = expectedBtn;
                m_lastEvent.event = EVENT_LONG_PRESS;
                m_lastEvent.pressDuration = now - track->pressStartTime;
            }
        }
    }

    // Side buttons (GPIO2)
    ButtonId sideButtons[] = {BTN_VOL_UP, BTN_VOL_DOWN};
    for (int i = 0; i < 2; i++) {
        ButtonId expectedBtn = sideButtons[i];
        bool isNowPressed = (btn2 == expectedBtn);
        ButtonTrack* track = &m_buttons[4 + i];

        if (isNowPressed != track->pressed) {
            track->debounceCount++;
            if (track->debounceCount >= DEBOUNCE_COUNT) {
                track->debounceCount = 0;
                track->pressed = isNowPressed;

                if (isNowPressed) {
                    track->pressStartTime = now;
                    track->longPressFired = false;
                    m_lastEvent.id = expectedBtn;
                    m_lastEvent.event = EVENT_PRESSED;
                    m_lastEvent.pressDuration = 0;
                } else {
                    track->pressDuration = now - track->pressStartTime;
                    m_lastEvent.id = expectedBtn;
                    m_lastEvent.event = EVENT_RELEASED;
                    m_lastEvent.pressDuration = track->pressDuration;
                }
            }
        } else {
            track->debounceCount = 0;
        }
    }

    // Power button
    ButtonTrack* powerTrack = &m_buttons[6];
    if (powerPressed != powerTrack->pressed) {
        powerTrack->debounceCount++;
        if (powerTrack->debounceCount >= DEBOUNCE_COUNT) {
            powerTrack->debounceCount = 0;
            powerTrack->pressed = powerPressed;

            if (powerPressed) {
                powerTrack->pressStartTime = now;
                powerTrack->longPressFired = false;
                m_lastEvent.id = BTN_POWER;
                m_lastEvent.event = EVENT_PRESSED;
                m_lastEvent.pressDuration = 0;
            } else {
                powerTrack->pressDuration = now - powerTrack->pressStartTime;
                m_lastEvent.id = BTN_POWER;
                m_lastEvent.event = EVENT_RELEASED;
                m_lastEvent.pressDuration = powerTrack->pressDuration;
            }
        }
    } else {
        powerTrack->debounceCount = 0;
    }
#endif
}

bool HalInput::isPressed(ButtonId id) const {
    for (int i = 0; i < 7; i++) {
        if (m_buttons[i].id == id) {
            return m_buttons[i].pressed;
        }
    }
    return false;
}

ButtonState HalInput::getLastEvent() {
    ButtonState event = m_lastEvent;
    m_lastEvent.event = EVENT_NONE;  // Clear after reading
    return event;
}

ButtonId HalInput::waitForButton(uint32_t timeoutMs) {
#ifdef ARDUINO
    uint32_t startTime = millis();

    while (true) {
        update();
        ButtonState event = getLastEvent();

        if (event.event == EVENT_PRESSED) {
            return event.id;
        }

        if (timeoutMs > 0 && (millis() - startTime) > timeoutMs) {
            return BTN_NONE;
        }

        delay(10);
    }
#else
    return BTN_NONE;
#endif
}

bool HalInput::isPowerButtonPressed() const {
#ifdef ARDUINO
    return digitalRead(BTN_POWER_PIN) == LOW;
#else
    return false;
#endif
}

uint32_t HalInput::getPowerButtonHoldTime() const {
#ifdef ARDUINO
    if (m_buttons[6].pressed) {
        return millis() - m_buttons[6].pressStartTime;
    }
#endif
    return 0;
}

uint16_t HalInput::readAdcPin(uint8_t pin) {
#ifdef ARDUINO
    return analogRead(pin);
#else
    return 0;
#endif
}

ButtonId HalInput::identifyButton(uint8_t adcPin, uint16_t value) {
    if (adcPin == 1) {
        // GPIO1 - 4 front buttons
        if (value >= BTN_BACK_MIN && value <= BTN_BACK_MAX) return BTN_BACK;
        if (value >= BTN_CONFIRM_MIN && value <= BTN_CONFIRM_MAX) return BTN_CONFIRM;
        if (value >= BTN_LEFT_MIN && value <= BTN_LEFT_MAX) return BTN_LEFT;
        if (value >= BTN_RIGHT_MIN && value <= BTN_RIGHT_MAX) return BTN_RIGHT;
    } else if (adcPin == 2) {
        // GPIO2 - 2 side buttons
        if (value >= BTN_VOLUP_MIN && value <= BTN_VOLUP_MAX) return BTN_VOL_UP;
        if (value >= BTN_VOLDOWN_MIN && value <= BTN_VOLDOWN_MAX) return BTN_VOL_DOWN;
    }
    return BTN_NONE;
}
