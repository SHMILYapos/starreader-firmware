/**
 * StarReader Pro Firmware - Settings Persistence
 * 
 * Binary settings storage on SD card
 */

#ifndef STARREADER_UTILS_SETTINGS_H
#define STARREADER_UTILS_SETTINGS_H

#include <stdint.h>
#include "../hal/storage.h"

// Settings version for migration
#define SETTINGS_VERSION 2

struct StarReaderSettings {
    // ===== Display Settings =====
    uint8_t orientation;           // 0=Portrait, 1=Landscape, 2=Inverted Portrait, 3=Inverted Landscape
    uint8_t refreshMode;           // 0=Full, 1=Partial, 2=Fast
    uint8_t fontSize;              // 0=Small, 1=Medium, 2=Large, 3=XL
    uint8_t _pad1;

    // ===== Reader Settings =====
    uint8_t lineSpacing;           // 0=Tight, 1=Normal, 2=Relaxed
    uint8_t margins;               // pixels
    uint8_t justification;          // 0=Left, 1=Justified
    uint8_t _pad2;

    // ===== Front Light Settings (X4 Pro) =====
    uint8_t brightness;            // 0-100%
    uint8_t frontLightWarmth;      // 0-100% (0=cool, 100=warm)
    uint8_t frontLightOn;          // 0=off, 1=on
    uint8_t gestureSensitivity;    // 0-100%

    // ===== Power Settings =====
    uint16_t sleepTimeoutSec;      // auto sleep timeout
    uint16_t autoRefreshPages;     // force full refresh every N pages

    // ===== System Settings =====
    uint8_t language;              // 0=English, 1=Chinese
    uint8_t touchEnabled;           // 0=off, 1=on
    uint8_t statusBarEnabled;      // 0=off, 1=on
    uint8_t version;               // settings version

    // ===== Reading Position =====
    char lastBookPath[128];        // last opened book
    uint32_t lastBookPosition;     // last position (byte offset)
    uint16_t lastBookPage;         // last page number
    uint16_t _pad3;

    // ===== WiFi Settings (future) =====
    char wifiSsid[32];
    char wifiPassword[64];

    // Checksum (must be last)
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

    // Convenience setters that auto-save
    void setBrightness(uint8_t brightness);
    void setWarmth(uint8_t warmth);
    void setFrontLightOn(bool on);
    void setOrientation(uint8_t orientation);
    void setFontSize(uint8_t size);
    void setSleepTimeout(uint16_t seconds);

private:
    HalStorage* m_storage;
    StarReaderSettings m_settings;

    uint32_t calculateChecksum(const StarReaderSettings* settings);
    bool validateChecksum(const StarReaderSettings* settings);
};

#endif // STARREADER_UTILS_SETTINGS_H
