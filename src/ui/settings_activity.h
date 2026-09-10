/**
 * StarReader Pro Firmware - Settings Activity
 * 
 * 设置主界面：分组列表（显示/阅读/前光/网络/系统）
 * Settings: grouped list - Display / Reader / Frontlight / Network / System
 */

#ifndef STARREADER_UI_SETTINGS_ACTIVITY_H
#define STARREADER_UI_SETTINGS_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class SettingsActivity : public Activity {
public:
    SettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                     HalPowerManager* power, HalTouch* touch = nullptr,
                     HalFrontLight* frontLight = nullptr,
                     SettingsManager* settingsManager = nullptr);
    ~SettingsActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum SettingsGroup {
        GROUP_DISPLAY = 0,    // 显示
        GROUP_READER,         // 阅读
        GROUP_FRONTLIGHT,     // 前光
        GROUP_NETWORK,        // 网络
        GROUP_SYSTEM,         // 系统
        GROUP_COUNT
    };

    int m_selectedIndex;
    int m_scrollOffset;

    static const int STATUS_BAR_HEIGHT = 28;
    static const int LIST_ITEM_HEIGHT = 48;
    static const int LIST_PADDING = 10;

    void drawTitleBar();
    void drawSettingsList();
    void drawBottomHint();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void launchGroup(int group);
};

#endif // STARREADER_UI_SETTINGS_ACTIVITY_H
