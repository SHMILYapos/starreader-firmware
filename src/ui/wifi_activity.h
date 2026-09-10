/**
 * StarReader Pro Firmware - WiFi Activity
 * 
 * WiFi 网络配置界面
 * WiFi network configuration screen
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
    HalTouch* m_touch;
    HalFrontLight* m_frontLight;
    SettingsManager* m_settingsManager;
    bool m_needsRender;
    bool m_connecting;
    int m_selectedItem;
    uint32_t m_connectStartTime;

    static const int MENU_START_Y = 50;
    static const int MENU_ITEM_HEIGHT = 36;

    void drawWifiMenu();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void connectToWifi();
    void startWebServer();
};

#endif // STARREADER_UI_WIFI_ACTIVITY_H
