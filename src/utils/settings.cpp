/**
 * StarReader Pro Firmware - Settings Persistence Implementation
 */

#include "settings.h"
#include "../config.h"
#include <string.h>

SettingsManager::SettingsManager(HalStorage* storage)
    : m_storage(storage) {
    resetToDefaults();
}

SettingsManager::~SettingsManager() {
}

bool SettingsManager::load() {
    if (!m_storage->fileExists(SD_SETTINGS_FILE)) {
        resetToDefaults();
        return false;
    }

    size_t bytesRead = 0;
    if (m_storage->readFile(SD_SETTINGS_FILE, (uint8_t*)&m_settings,
                           sizeof(StarReaderSettings), &bytesRead)) {
        if (bytesRead == sizeof(StarReaderSettings)) {
            if (validateChecksum(&m_settings)) {
                // Check version - migrate if needed
                if (m_settings.version == SETTINGS_VERSION) {
                    return true;
                }
                // Version mismatch - keep compatible fields, reset rest
                uint8_t oldVersion = m_settings.version;
                StarReaderSettings defaults;
                memset(&defaults, 0, sizeof(defaults));
                resetToDefaults();
                // Copy compatible fields from old settings
                m_settings.orientation = defaults.orientation;
                m_settings.brightness = defaults.brightness;
                m_settings.frontLightWarmth = defaults.frontLightWarmth;
                m_settings.sleepTimeoutSec = defaults.sleepTimeoutSec;
                m_settings.version = SETTINGS_VERSION;
                save();
                return true;
            }
        }
    }

    // Load failed, use defaults
    resetToDefaults();
    return false;
}

bool SettingsManager::save() {
    m_settings.checksum = calculateChecksum(&m_settings);
    return m_storage->writeFile(SD_SETTINGS_FILE, (uint8_t*)&m_settings,
                               sizeof(StarReaderSettings));
}

void SettingsManager::resetToDefaults() {
    memset(&m_settings, 0, sizeof(StarReaderSettings));

    // Display defaults
    m_settings.orientation = 1;      // Landscape
    m_settings.refreshMode = 1;      // Partial refresh
    m_settings.fontSize = 1;        // Medium
    m_settings._pad1 = 0;

    // Reader defaults
    m_settings.lineSpacing = 1;      // Normal
    m_settings.margins = 20;         // pixels
    m_settings.justification = 1;    // Justified
    m_settings._pad2 = 0;

    // Front light defaults (X4 Pro)
    m_settings.brightness = DEFAULT_BRIGHTNESS;
    m_settings.frontLightWarmth = DEFAULT_WARMTH;
    m_settings.frontLightOn = DEFAULT_FRONT_LIGHT_ON ? 1 : 0;
    m_settings.gestureSensitivity = 70;

    // Power defaults
    m_settings.sleepTimeoutSec = 300;  // 5 minutes
    m_settings.autoRefreshPages = 50;

    // System defaults
    m_settings.language = 1;         // Chinese
    m_settings.touchEnabled = 1;     // Touch on by default
    m_settings.statusBarEnabled = 1; // Status bar on
    m_settings.version = SETTINGS_VERSION;

    // Reading position
    memset(m_settings.lastBookPath, 0, sizeof(m_settings.lastBookPath));
    m_settings.lastBookPosition = 0;
    m_settings.lastBookPage = 0;

    // WiFi defaults
    memset(m_settings.wifiSsid, 0, sizeof(m_settings.wifiSsid));
    memset(m_settings.wifiPassword, 0, sizeof(m_settings.wifiPassword));

    m_settings.checksum = calculateChecksum(&m_settings);
}

StarReaderSettings* SettingsManager::getSettings() {
    return &m_settings;
}

// ===== Convenience Setters =====

void SettingsManager::setBrightness(uint8_t brightness) {
    if (brightness > 100) brightness = 100;
    m_settings.brightness = brightness;
    save();
}

void SettingsManager::setWarmth(uint8_t warmth) {
    if (warmth > 100) warmth = 100;
    m_settings.frontLightWarmth = warmth;
    save();
}

void SettingsManager::setFrontLightOn(bool on) {
    m_settings.frontLightOn = on ? 1 : 0;
    save();
}

void SettingsManager::setOrientation(uint8_t orientation) {
    m_settings.orientation = orientation;
    save();
}

void SettingsManager::setFontSize(uint8_t size) {
    m_settings.fontSize = size;
    save();
}

void SettingsManager::setSleepTimeout(uint16_t seconds) {
    m_settings.sleepTimeoutSec = seconds;
    save();
}

void SettingsManager::setLanguage(uint8_t lang) {
    m_settings.language = lang;
    save();
}

void SettingsManager::setTouchEnabled(bool enabled) {
    m_settings.touchEnabled = enabled ? 1 : 0;
    save();
}

void SettingsManager::setStatusBarEnabled(bool enabled) {
    m_settings.statusBarEnabled = enabled ? 1 : 0;
    save();
}

// ===== Internal =====

uint32_t SettingsManager::calculateChecksum(const StarReaderSettings* settings) {
    uint32_t sum = 0;
    const uint8_t* ptr = (const uint8_t*)settings;
    size_t size = sizeof(StarReaderSettings) - sizeof(uint32_t);  // Exclude checksum field

    for (size_t i = 0; i < size; i++) {
        sum += ptr[i];
    }

    return sum;
}

bool SettingsManager::validateChecksum(const StarReaderSettings* settings) {
    uint32_t expected = calculateChecksum(settings);
    return (expected == settings->checksum);
}
