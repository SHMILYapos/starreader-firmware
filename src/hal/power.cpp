/**
 * StarReader Firmware - Power Management HAL Implementation
 */

#include "power.h"
#include "../config.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <esp_sleep.h>
#endif

HalPowerManager::HalPowerManager()
    : m_initialized(false)
    , m_bootTime(0)
    , m_batteryMinVoltage(3.0f)
    , m_batteryMaxVoltage(4.2f) {
}

HalPowerManager::~HalPowerManager() {
    end();
}

bool HalPowerManager::begin() {
#ifdef ARDUINO
    // Configure battery ADC pin
    pinMode(BATTERY_ADC_PIN, INPUT);
    analogReadResolution(12);

    // Configure USB/magnetic detect pin
    pinMode(MAGNETIC_DET_PIN, INPUT);

    // Configure power button as wakeup source
    // TODO: ESP32-C3 GPIO wakeup configuration
    // esp_deep_sleep_enable_gpio_wakeup(BTN_POWER_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);

    m_bootTime = millis();
    m_initialized = true;
    return true;
#else
    m_bootTime = 0;
    m_initialized = true;
    return true;
#endif
}

void HalPowerManager::end() {
    m_initialized = false;
}

float HalPowerManager::getBatteryVoltage() {
    return readBatteryVoltage();
}

float HalPowerManager::readBatteryVoltage() {
#ifdef ARDUINO
    uint16_t adcValue = analogRead(BATTERY_ADC_PIN);
    // Convert ADC to voltage (12-bit, 3.3V reference)
    float adcVoltage = (adcValue / 4095.0f) * 3.3f;
    // Apply voltage divider ratio
    float batteryVoltage = adcVoltage * BATTERY_DIVIDER_RATIO;
    return batteryVoltage;
#else
    return 3.7f;  // Simulation
#endif
}

uint8_t HalPowerManager::getBatteryPercentage() {
    float voltage = getBatteryVoltage();

    // Linear approximation between min and max
    float percentage = (voltage - m_batteryMinVoltage) /
                       (m_batteryMaxVoltage - m_batteryMinVoltage) * 100.0f;

    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    return (uint8_t)percentage;
}

bool HalPowerManager::isBatteryLow() {
    return getBatteryVoltage() < 3.4f;
}

ChargingStatus HalPowerManager::getChargingStatus() {
#ifdef ARDUINO
    if (isUsbConnected()) {
        float voltage = getBatteryVoltage();
        if (voltage >= 4.15f) {
            return CHARGING_COMPLETE;
        } else {
            return CHARGING_IN_PROGRESS;
        }
    } else {
        return CHARGING_NOT_CHARGING;
    }
#else
    return CHARGING_UNKNOWN;
#endif
}

bool HalPowerManager::isUsbConnected() {
#ifdef ARDUINO
    // Magnetic/USB detect pin - HIGH when connected
    return digitalRead(MAGNETIC_DET_PIN) == HIGH;
#else
    return false;
#endif
}

void HalPowerManager::enterDeepSleep(uint32_t timeoutMs) {
#ifdef ARDUINO
    // Save state before sleep
    // ... (application should save state before calling this)

    // Configure wakeup sources
    // TODO: ESP32-C3 GPIO wakeup configuration
    // esp_deep_sleep_enable_gpio_wakeup(BTN_POWER_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);

    if (timeoutMs > 0) {
        esp_sleep_enable_timer_wakeup(timeoutMs * 1000ULL);
    }

    // Enter deep sleep
    esp_deep_sleep_start();
#endif
}

bool HalPowerManager::wasWokenFromSleep() {
#ifdef ARDUINO
    return esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED;
#else
    return false;
#endif
}

uint8_t HalPowerManager::getWakeupReason() {
#ifdef ARDUINO
    switch (esp_sleep_get_wakeup_cause()) {
        case ESP_SLEEP_WAKEUP_EXT0:
            return 1;  // Power button
        case ESP_SLEEP_WAKEUP_TIMER:
            return 2;  // Timer
        case ESP_SLEEP_WAKEUP_EXT1:
            return 3;  // Other GPIO
        default:
            return 0;  // Power on / unknown
    }
#else
    return 0;
#endif
}

uint32_t HalPowerManager::getUptimeSeconds() {
#ifdef ARDUINO
    return (millis() - m_bootTime) / 1000;
#else
    return 0;
#endif
}

float HalPowerManager::getAverageCurrent() {
    // TODO: Implement current measurement if hardware supports it
    return 0.0f;
}
