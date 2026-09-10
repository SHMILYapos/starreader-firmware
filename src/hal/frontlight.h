/**
 * StarReader Pro Firmware - Front Light HAL
 * 
 * Dual-tone front light control for X4 Pro
 * Warm + Cool white LED channels, PWM dimming
 */

#ifndef STARREADER_HAL_FRONTLIGHT_H
#define STARREADER_HAL_FRONTLIGHT_H

#include <stdint.h>
#include "../config.h"

class HalFrontLight {
public:
    HalFrontLight();
    ~HalFrontLight();

    bool begin();
    void end();

    // Power on/off
    void on();
    void off();
    bool isOn() const;
    void toggle();

    // Brightness control (0-100%)
    void setBrightness(uint8_t brightness);
    uint8_t getBrightness() const;
    void brightnessUp(uint8_t step = 10);
    void brightnessDown(uint8_t step = 10);

    // Color temperature (0=cool, 100=warm)
    void setWarmth(uint8_t warmth);
    uint8_t getWarmth() const;
    void warmthUp(uint8_t step = 10);
    void warmthDown(uint8_t step = 10);

    // Preset modes
    void setPresetReading();    // Balanced, medium brightness
    void setPresetNight();      // Warm, low brightness
    void setPresetDay();        // Cool, high brightness
    void setPresetAuto();       // TODO: Ambient light sensor

    // Ramp animation
    void fadeTo(uint8_t brightness, uint16_t durationMs = 500);

    // Status
    bool isInitialized() const;
    uint8_t getWarmLEDValue() const;
    uint8_t getCoolLEDValue() const;

private:
    // PWM control
    void setPwmChannel(uint8_t channel, uint16_t value);
    void updatePwm();

    // State
    bool m_initialized;
    bool m_enabled;
    uint8_t m_brightness;   // 0-100%
    uint8_t m_warmth;       // 0-100% (0=cool, 100=warm)

    // Calculated PWM values (0-1023 for 10-bit)
    uint16_t m_warmPwm;
    uint16_t m_coolPwm;

    // Fade animation
    bool m_fading;
    uint8_t m_targetBrightness;
    uint32_t m_fadeStartTime;
    uint16_t m_fadeDuration;
};

#endif // STARREADER_HAL_FRONTLIGHT_H
