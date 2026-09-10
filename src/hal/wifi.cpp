/**
 * StarReader Pro Firmware - WiFi Manager Implementation
 * 
 * WiFi 连接管理模块实现
 */

#include "wifi.h"
#include "../config.h"
#include <string.h>

#ifdef ARDUINO
#include <Arduino.h>
#include <WiFi.h>
#endif

HalWifi::HalWifi()
    : m_initialized(false)
    , m_mode(STAR_WIFI_MODE_OFF)
    , m_status(WIFI_STATUS_DISCONNECTED)
    , m_rssi(0)
    , m_apClients(0) {
    memset(m_ssid, 0, sizeof(m_ssid));
    memset(m_ip, 0, sizeof(m_ip));
}

HalWifi::~HalWifi() {
    end();
}

bool HalWifi::begin() {
#ifdef ARDUINO
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    
    m_initialized = true;
    m_status = WIFI_STATUS_DISCONNECTED;
    return true;
#else
    m_initialized = true;
    return true;
#endif
}

void HalWifi::end() {
#ifdef ARDUINO
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
#endif
    m_initialized = false;
    m_mode = STAR_WIFI_MODE_OFF;
    m_status = WIFI_STATUS_DISCONNECTED;
}

void HalWifi::setMode(WifiMode mode) {
#ifdef ARDUINO
    m_mode = mode;
    switch (mode) {
        case STAR_WIFI_MODE_OFF:
            WiFi.mode(WIFI_OFF);
            m_status = WIFI_STATUS_DISCONNECTED;
            break;
        case STAR_WIFI_MODE_STA:
            WiFi.mode(WIFI_STA);
            break;
        case STAR_WIFI_MODE_AP:
            WiFi.mode(WIFI_AP);
            break;
        case STAR_WIFI_MODE_AP_STA:
            WiFi.mode(WIFI_AP_STA);
            break;
    }
#endif
}

WifiMode HalWifi::getMode() const {
    return m_mode;
}

bool HalWifi::connect(const char* ssid, const char* password, uint32_t timeoutMs) {
#ifdef ARDUINO
    if (m_mode != STAR_WIFI_MODE_STA && m_mode != STAR_WIFI_MODE_AP_STA) {
        setMode(STAR_WIFI_MODE_STA);
    }

    m_status = WIFI_STATUS_CONNECTING;
    strncpy(m_ssid, ssid, sizeof(m_ssid) - 1);

    WiFi.begin(ssid, password);

    uint32_t startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > timeoutMs) {
            m_status = WIFI_STATUS_TIMEOUT;
            return false;
        }
        delay(500);
    }

    m_status = WIFI_STATUS_CONNECTED;
    strncpy(m_ip, WiFi.localIP().toString().c_str(), sizeof(m_ip) - 1);
    m_rssi = WiFi.RSSI();
    return true;
#else
    (void)ssid; (void)password; (void)timeoutMs;
    return false;
#endif
}

void HalWifi::disconnect() {
#ifdef ARDUINO
    WiFi.disconnect();
#endif
    m_status = WIFI_STATUS_DISCONNECTED;
}

bool HalWifi::isConnected() const {
    return m_status == WIFI_STATUS_CONNECTED;
}

WifiStatus HalWifi::getStatus() const {
    return m_status;
}

const char* HalWifi::getIpAddress() const {
    return m_ip;
}

const char* HalWifi::getSsid() const {
    return m_ssid;
}

int32_t HalWifi::getRssi() const {
    return m_rssi;
}

bool HalWifi::startAp(const char* ssid, const char* password) {
#ifdef ARDUINO
    if (m_mode != STAR_WIFI_MODE_AP && m_mode != STAR_WIFI_MODE_AP_STA) {
        setMode(STAR_WIFI_MODE_AP);
    }

    bool success;
    if (password && strlen(password) > 0) {
        success = WiFi.softAP(ssid, password);
    } else {
        success = WiFi.softAP(ssid);
    }

    if (success) {
        m_status = WIFI_STATUS_AP_STARTED;
        strncpy(m_ip, WiFi.softAPIP().toString().c_str(), sizeof(m_ip) - 1);
        return true;
    }
    return false;
#else
    (void)ssid; (void)password;
    return false;
#endif
}

void HalWifi::stopAp() {
#ifdef ARDUINO
    WiFi.softAPdisconnect(true);
#endif
    if (m_mode == STAR_WIFI_MODE_AP) {
        m_status = WIFI_STATUS_DISCONNECTED;
    }
}

uint8_t HalWifi::getApClientCount() const {
#ifdef ARDUINO
    return WiFi.softAPgetStationNum();
#else
    return 0;
#endif
}

int HalWifi::scanNetworks(WifiApInfo* results, int maxResults) {
#ifdef ARDUINO
    int n = WiFi.scanNetworks();
    int count = (n < maxResults) ? n : maxResults;
    
    for (int i = 0; i < count; i++) {
        strncpy(results[i].ssid, WiFi.SSID(i).c_str(), sizeof(results[i].ssid) - 1);
        results[i].rssi = WiFi.RSSI(i);
        results[i].encryption = WiFi.encryptionType(i);
    }
    
    WiFi.scanDelete();
    return count;
#else
    (void)results; (void)maxResults;
    return 0;
#endif
}

bool HalWifi::isScanning() const {
#ifdef ARDUINO
    return WiFi.scanComplete() == WIFI_SCAN_RUNNING;
#else
    return false;
#endif
}

bool HalWifi::isEnabled() const {
    return m_initialized && m_mode != STAR_WIFI_MODE_OFF;
}

const char* HalWifi::getStatusString() const {
    switch (m_status) {
        case WIFI_STATUS_DISCONNECTED: return "未连接";
        case WIFI_STATUS_CONNECTING: return "连接中...";
        case WIFI_STATUS_CONNECTED: return "已连接";
        case WIFI_STATUS_CONNECT_FAILED: return "连接失败";
        case WIFI_STATUS_TIMEOUT: return "连接超时";
        case WIFI_STATUS_AP_STARTED: return "热点已启动";
        default: return "未知";
    }
}

void HalWifi::updateStatus() {
#ifdef ARDUINO
    if (m_mode == STAR_WIFI_MODE_STA || m_mode == STAR_WIFI_MODE_AP_STA) {
        if (WiFi.status() == WL_CONNECTED) {
            m_status = WIFI_STATUS_CONNECTED;
            m_rssi = WiFi.RSSI();
        } else {
            m_status = WIFI_STATUS_DISCONNECTED;
        }
    }
#endif
}
