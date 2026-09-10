/**
 * StarReader Pro Firmware - Sleep Screen Activity
 * 
 * 睡眠屏幕/待机表盘界面
 * Sleep screen / standby watchface
 */

#ifndef STARREADER_UI_SLEEP_SCREEN_ACTIVITY_H
#define STARREADER_UI_SLEEP_SCREEN_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class SleepScreenActivity : public Activity {
public:
    SleepScreenActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                        HalPowerManager* power, HalTouch* touch = nullptr,
                        HalFrontLight* frontLight = nullptr,
                        SettingsManager* settingsManager = nullptr);
    ~SleepScreenActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

    bool preventAutoSleep() override { return false; }

private:
    enum WatchfaceType {
        WATCHFACE_CLOCK = 0,
        WATCHFACE_BOOK,
        WATCHFACE_CALENDAR,
        WATCHFACE_COUNT
    };

    int m_watchfaceType;
    bool m_touched;
    uint32_t m_lastUpdate;
    
    // 模拟时间数据
    int m_hour;
    int m_minute;
    int m_day;
    int m_month;
    int m_year;
    int m_weekday;

    static const int PADDING = 20;

    void drawClockWatchface();
    void drawBookWatchface();
    void drawCalendarWatchface();
    void drawBatteryIndicator();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void updateTime();
    void wakeUp();
};

#endif // STARREADER_UI_SLEEP_SCREEN_ACTIVITY_H
