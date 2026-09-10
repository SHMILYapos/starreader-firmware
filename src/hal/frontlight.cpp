/**
 * StarReader Pro Firmware - Front Light HAL Implementation
 */

#include "frontlight.h"
#include "../config.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

HalFrontLight::HalFrontLight()
    : m_initialized(false)
    , m_enabled(false)
    , m_brightness(DEFAULT_BRIGHTNESS)
    , m_warmth(DEFAULT_WARMTH)
    , m_warmPwm(0)
    , m_coolPwm(0)
    , m_fading(false)
    , m_targetBrightness(DEFAULT_BRIGHTNESS)
    , m_fadeStartTime(0)
    , m_fadeDuration(500) {
}

HalFrontLight::~HalFrontLight() {
    end();
}

bool HalFrontLight::begin() {
#ifdef ARDUINO
    // Configure PWM pins for front light
    ledcSetup(0, FRONT_LIGHT_PWM_FREQ, FRONT_LIGHT_PWM_RES);
    ledcSetup(1, FRONT_LIGHT_PWM_FREQ, FRONT_LIGHT_PWM_RES);

    ledcAttachPin(FRONT_LIGHT_PWM_WARM, 0);
    ledcAttachPin(FRONT_LIGHT_PWM_COOL, 1);

    // Start off
    setBrightness(0);

    m_initialized = true;
    m_enabled = DEFAULT_FRONT_LIGHT_ON;

    if (m_enabled) {
        setBrightness(m_brightness);
    }

    return true;
#else
    m_initialized = true;
    m_enabled = false;
    return true;
#endif
}

void HalFrontLight::end() {
    off();
#ifdef ARDUINO
    ledcDetachPin(FRONT_LIGHT_PWM_WARM);
    ledcDetachPin(FRONT_LIGHT_PWM_COOL);
#endif
    m_initialized = false;
    m_enabled = false;
}

void HalFrontLight::on() {
    m_enabled = true;
    updatePwm();
}

void HalFrontLight::off() {
    m_enabled = false;
    m_warmPwm = 0;
    m_coolPwm = 0;
    updatePwm();
}

bool HalFrontLight::isOn() const {
    return m_enabled && m_brightness > 0;
}

void HalFrontLight::toggle() {
    if (isOn()) {
        off();
    } else {
        on();
    }
}

void HalFrontLight::setBrightness(uint8_t brightness) {
    if (brightness > 100) brightness = 100;
    m_brightness = brightness;
    updatePwm();
}

uint8_t HalFrontLight::getBrightness() const {
    return m_brightness;
}

void HalFrontLight::brightnessUp(uint8_t step) {
    uint8_t newBrightness = m_brightness + step;
    if (newBrightness > 100) newBrightness = 100;
    setBrightness(newBrightness);

    // Auto-enable when brightness increased
    if (newBrightness > 0 && !m_enabled) {
        on();
    }
}

void HalFrontLight::brightnessDown(uint8_t step) {
    if (m_brightness < step) {
        setBrightness(0);
        off();
    } else {
        setBrightness(m_brightness - step);
    }
}

void HalFrontLight::setWarmth(uint8_t warmth) {
    if (warmth > 100) warmth = 100;
    m_warmth = warmth;
    updatePwm();
}

uint8_t HalFrontLight::getWarmth() const {
    return m_warmth;
}

void HalFrontLight::warmthUp(uint8_t step) {
    uint8_t newWarmth = m_warmth + step;
    if (newWarmth > 100) newWarmth = 100;
    setWarmth(newWarmth);
}

void HalFrontLight::warmthDown(uint8_t step) {
    if (m_warmth < step) {
        setWarmth(0);
    } else {
        setWarmth(m_warmth - step);
    }
}

void HalFrontLight::setPresetReading() {
    setBrightness(50);
    setWarmth(50);
    on();
}

void HalFrontLight::setPresetNight() {
    setBrightness(20);
    setWarmth(80);  // Warm light for night reading
    on();
}

void HalFrontLight::setPresetDay() {
    setBrightness(80);
    setWarmth(20);  // Cool light for daytime
    on();
}

void HalFrontLight::setPresetAuto() {
    // TODO: Implement ambient light sensor reading
    setPresetReading();
}

void HalFrontLight::fadeTo(uint8_t brightness, uint16_t durationMs) {
    m_targetBrightness = brightness;
    m_fadeStartTime = millis();
    m_fadeDuration = durationMs;
    m_fading = true;
}

bool HalFrontLight::isInitialized() const {
    return m_initialized;
}

uint8_t HalFrontLight::getWarmLEDValue() const {
    return (m_warmPwm * 100) / 1023;
}

uint8_t HalFrontLight::getCoolLEDValue() const {
    return (m_coolPwm * 100) / 1023;
}

void HalFrontLight::setPwmChannel(uint8_t channel, uint16_t value) {
#ifdef ARDUINO
    ledcWrite(channel, value);
#endif
}

void HalFrontLight::updatePwm() {
    if (!m_initialized) return;

    if (!m_enabled) {
        m_warmPwm = 0;
        m_coolPwm = 0;
        setPwmChannel(0, 0);
        setPwmChannel(1, 0);
        return;
    }

    // Calculate total brightness as 0-1023
    uint16_t totalBrightness = (m_brightness * 1023) / 100;

    // Split between warm and cool based on warmth setting
    // warmth = 0 -> all cool, warmth = 100 -> all warm
    float warmRatio = m_warmth / 100.0f;
    float coolRatio = 1.0f - warmRatio;

    m_warmPwm = (uint16_t)(totalBrightness * warmRatio);
    m_coolPwm = (uint16_t)(totalBrightness * coolRatio);

    setPwmChannel(0, m_warmPwm);
    setPwmChannel(1, m_coolPwm);
}
