/**
 * StarReader Pro Firmware - Apps Hub
 * 
 * 应用中心：图标宫格（阅读统计/游戏/表盘/工具）
 * Apps hub: icon grid (stats/games/widgets/tools)
 */

#ifndef STARREADER_UI_APPS_HUB_ACTIVITY_H
#define STARREADER_UI_APPS_HUB_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class AppsHubActivity : public Activity {
public:
    AppsHubActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                    HalPowerManager* power, HalTouch* touch = nullptr,
                    HalFrontLight* frontLight = nullptr,
                    SettingsManager* settingsManager = nullptr);
    ~AppsHubActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum AppItem {
        APP_READING_STATS = 0,
        APP_GAME_2048,
        APP_GAME_SUDOKU,
        APP_CALCULATOR,
        APP_CLOCK_WIDGET,
        APP_SLEEP_SCREEN,
        APP_WIFI,
        APP_ABOUT,
        APP_COUNT
    };

    int m_selectedRow;
    int m_selectedCol;

    static const int STATUS_BAR_HEIGHT = 28;
    static const int GRID_COLS = 4;
    static const int GRID_ROWS = 2;
    static const int GRID_PADDING = 15;
    static const int GRID_GAP = 10;

    void drawTitleBar();
    void drawAppGrid();
    void drawBottomHint();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void launchApp(int app);
    int getSelectedIndex();
};

#endif // STARREADER_UI_APPS_HUB_ACTIVITY_H
