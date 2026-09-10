/**
 * StarReader Firmware - Settings Persistence
 * 
 * Binary settings storage on SD card
 */

#ifndef STARREADER_UTILS_SETTINGS_H
#define STARREADER_UTILS_SETTINGS_H

#include <stdint.h>
#include "../hal/storage.h"

struct StarReaderSettings {
    // Display
    uint8_t orientation;
    uint8_t refreshMode;
    uint8_t fontSize;
    uint8_t padding;

    // Reader
    uint8_t lineSpacing;
    uint8_t margins;
    uint8_t justification;
    uint8_t padding2;

    // Power
    uint16_t sleepTimeoutSec;
    uint16_t autoRefreshPages;

    // System
    uint8_t language;
    uint8_t brightness;
    uint8_t frontLightWarmth;
    uint8_t version;

    // WiFi
    char wifiSsid[32];
    char wifiPassword[64];

    // Checksum
    uint32_t checksum;
};

class SettingsManager {
public:
    SettingsManager(HalStorage* storage);
    ~SettingsManager();

    bool load();
    bool save();
    void resetToDefaults();

    StarReaderSettings* getSettings();

private:
    HalStorage* m_storage;
    StarReaderSettings m_settings;

    uint32_t calculateChecksum(const StarReaderSettings* settings);
    bool validateChecksum(const StarReaderSettings* settings);
};

#endif // STARREADER_UTILS_SETTINGS_H
