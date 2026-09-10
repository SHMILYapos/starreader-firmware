/**
 * StarReader Pro Firmware - Reader Settings Activity
 * 
 * 阅读设置子页：翻页方式/状态栏/屏幕方向/进度同步
 * Reader settings sub-page
 */

#ifndef STARREADER_UI_READER_SETTINGS_ACTIVITY_H
#define STARREADER_UI_READER_SETTINGS_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class ReaderSettingsActivity : public Activity {
public:
    ReaderSettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                           HalPowerManager* power, HalTouch* touch = nullptr,
                           HalFrontLight* frontLight = nullptr,
                           SettingsManager* settingsManager = nullptr);
    ~ReaderSettingsActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum ReaderSetting {
        SETTING_PAGE_TURN = 0,
        SETTING_STATUS_BAR,
        SETTING_PROGRESS_SYNC,
        SETTING_BATTERY_DISPLAY,
        SETTING_COUNT
    };

    int m_selectedItem;

    static const int STATUS_BAR_HEIGHT = 28;
    static const int LIST_ITEM_HEIGHT = 48;
    static const int LIST_PADDING = 10;

    void drawTitleBar();
    void drawSettingList();
    void drawBottomHint();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void cycleSetting(int item, int direction);
    const char* getSettingValueText(int item);
    void applySettings();
};

#endif // STARREADER_UI_READER_SETTINGS_ACTIVITY_H
