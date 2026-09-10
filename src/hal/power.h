/**
 * StarReader Firmware - Power Management HAL
 * 
 * Battery monitoring, sleep/wake management
 */

#ifndef STARREADER_HAL_POWER_H
#define STARREADER_HAL_POWER_H

#include <stdint.h>
#include "../config.h"

enum ChargingStatus {
    CHARGING_UNKNOWN = 0,
    CHARGING_NOT_CHARGING,
    CHARGING_IN_PROGRESS,
    CHARGING_COMPLETE
};

class HalPowerManager {
public:
    HalPowerManager();
    ~HalPowerManager();

    bool begin();
    void end();

    // Battery
    float getBatteryVoltage();
    uint8_t getBatteryPercentage();
    bool isBatteryLow();

    // Charging status
    ChargingStatus getChargingStatus();
    bool isUsbConnected();

    // Sleep / Wake
    void enterDeepSleep(uint32_t timeoutMs = 0);
    bool wasWokenFromSleep();
    uint8_t getWakeupReason();

    // Power statistics
    uint32_t getUptimeSeconds();
    float getAverageCurrent();  // mA

private:
    bool m_initialized;
    uint32_t m_bootTime;

    // Battery calibration
    float m_batteryMinVoltage;
    float m_batteryMaxVoltage;

    // Internal helpers
    float readBatteryVoltage();
};

#endif // STARREADER_HAL_POWER_H
