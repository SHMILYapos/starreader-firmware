/**
 * StarReader Pro Firmware - WiFi Activity
 * 
 * WiFi 网络设置界面
 * WiFi network settings screen
 */

#ifndef STARREADER_UI_WIFI_ACTIVITY_H
#define STARREADER_UI_WIFI_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class WifiActivity : public Activity {
public:
    WifiActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                 HalPowerManager* power, HalTouch* touch = nullptr,
                 HalFrontLight* frontLight = nullptr,
                 SettingsManager* settingsManager = nullptr);
    ~WifiActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum WifiMenuItem {
        MENU_WIFI_TOGGLE = 0,
        MENU_SCAN,
        MENU_CONNECTED,
        MENU_WEBSERVER,
        MENU_OTA,
        MENU_ITEM_COUNT
    };

    enum ViewMode {
        VIEW_MAIN = 0,
        VIEW_SCANNING,
        VIEW_NETWORK_LIST
    };

    int m_selectedItem;
    int m_selectedNetwork;
    ViewMode m_viewMode;
    bool m_wifiEnabled;
    bool m_connecting;
    bool m_scanning;
    uint32_t m_connectStartTime;
    uint32_t m_scanStartTime;
    char m_currentSsid[32];
    char m_currentIp[16];
    
    // 扫描结果
    static const int MAX_NETWORKS = 10;
    char m_networkSsids[MAX_NETWORKS][32];
    int m_networkRssi[MAX_NETWORKS];
    int m_networkCount;

    static const int STATUS_BAR_HEIGHT = 28;
    static const int LIST_ITEM_HEIGHT = 48;
    static const int LIST_PADDING = 10;

    void drawTitleBar();
    void drawMainMenu();
    void drawScanningView();
    void drawNetworkList();
    void drawBottomHint();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void toggleWifi();
    void scanNetworks();
    void connectToNetwork(int index);
    void startWebServer();
    void checkOta();
    void updateScanResults();
};

#endif // STARREADER_UI_WIFI_ACTIVITY_H
