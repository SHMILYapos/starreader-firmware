/**
 * StarReader Pro Firmware - Home Activity (Main Screen)
 * 
 * 主页：最近在读 + 四大入口（书架/文件/Apps/设置）
 * Home: Continue reading card + 4 main entries
 */

#ifndef STARREADER_UI_HOME_ACTIVITY_H
#define STARREADER_UI_HOME_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class HomeActivity : public Activity {
public:
    HomeActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power,
                 HalTouch* touch = nullptr, HalFrontLight* frontLight = nullptr,
                 SettingsManager* settingsManager = nullptr);
    ~HomeActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum MainMenu {
        MENU_CONTINUE = 0,    // 最近在读（大卡片）
        MENU_LIBRARY,         // 书架
        MENU_FILES,           // 文件
        MENU_APPS,            // Apps
        MENU_SETTINGS,        // 设置
        MENU_COUNT
    };

    int m_selectedIndex;

    // 布局常量
    static const int STATUS_BAR_HEIGHT = 28;
    static const int CARD_PADDING = 15;
    static const int CONTINUE_CARD_HEIGHT = 110;
    static const int GRID_GAP = 10;
    static const int GRID_ROWS = 2;
    static const int GRID_COLS = 2;

    void drawStatusBar();
    void drawContinueCard();
    void drawGridMenu();
    void drawBottomHint();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void launchMenuItem(int item);
};

#endif // STARREADER_UI_HOME_ACTIVITY_H
