/**
 * StarReader Firmware - Settings Persistence Implementation
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
    m_settings.refreshMode = 0;       // Full refresh
    m_settings.fontSize = 1;          // Medium
    m_settings.padding = 0;

    // Reader defaults
    m_settings.lineSpacing = 1;
    m_settings.margins = 20;
    m_settings.justification = 1;
    m_settings.padding2 = 0;

    // Power defaults
    m_settings.sleepTimeoutSec = 300;  // 5 minutes
    m_settings.autoRefreshPages = 50;

    // System defaults
    m_settings.language = 0;         // English
    m_settings.brightness = 128;
    m_settings.frontLightWarmth = 128;
    m_settings.version = 1;

    // WiFi defaults
    memset(m_settings.wifiSsid, 0, sizeof(m_settings.wifiSsid));
    memset(m_settings.wifiPassword, 0, sizeof(m_settings.wifiPassword));

    m_settings.checksum = calculateChecksum(&m_settings);
}

StarReaderSettings* SettingsManager::getSettings() {
    return &m_settings;
}

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
