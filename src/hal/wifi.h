/**
 * StarReader Pro Firmware - WiFi Manager
 * 
 * WiFi 连接管理模块
 * 支持 AP 热点模式和 STA 客户端模式
 */

#ifndef STARREADER_HAL_WIFI_H
#define STARREADER_HAL_WIFI_H

#include <stdint.h>
#include <stdbool.h>
#include "../config.h"

// WiFi 工作模式
enum WifiMode {
    STAR_WIFI_MODE_OFF = 0,      // WiFi 关闭
    STAR_WIFI_MODE_STA,          // 客户端模式（连接路由器）
    STAR_WIFI_MODE_AP,           // 热点模式（设备自身作为 AP）
    STAR_WIFI_MODE_AP_STA        // 混合模式
};

// WiFi 状态
enum WifiStatus {
    WIFI_STATUS_DISCONNECTED = 0,   // 未连接
    WIFI_STATUS_CONNECTING,         // 连接中
    WIFI_STATUS_CONNECTED,          // 已连接
    WIFI_STATUS_CONNECT_FAILED,     // 连接失败
    WIFI_STATUS_TIMEOUT,           // 连接超时
    WIFI_STATUS_AP_STARTED          // AP 模式已启动
};

// WiFi 接入点信息
struct WifiApInfo {
    char ssid[32];          // SSID 名称
    int32_t rssi;           // 信号强度 (dBm)
    uint8_t encryption;     // 加密类型
};

class HalWifi {
public:
    HalWifi();
    ~HalWifi();

    // 初始化
    bool begin();
    void end();

    // 模式控制
    void setMode(WifiMode mode);
    WifiMode getMode() const;

    // STA 模式（连接路由器）
    bool connect(const char* ssid, const char* password, uint32_t timeoutMs = 10000);
    void disconnect();
    bool isConnected() const;
    WifiStatus getStatus() const;
    const char* getIpAddress() const;
    const char* getSsid() const;
    int32_t getRssi() const;

    // AP 模式（设备热点）
    bool startAp(const char* ssid, const char* password = nullptr);
    void stopAp();
    uint8_t getApClientCount() const;

    // 扫描
    int scanNetworks(WifiApInfo* results, int maxResults);
    bool isScanning() const;

    // 工具
    bool isEnabled() const;
    const char* getStatusString() const;

private:
    bool m_initialized;
    WifiMode m_mode;
    WifiStatus m_status;
    char m_ssid[32];
    char m_ip[16];
    int32_t m_rssi;
    uint8_t m_apClients;

    void updateStatus();
};

#endif // STARREADER_HAL_WIFI_H
