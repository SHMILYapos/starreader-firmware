/**
 * StarReader Pro Firmware - Display Settings Activity
 * 
 * 显示设置子页：字号/行距/边距/刷新模式/方向
 * Display settings sub-page
 */

#ifndef STARREADER_UI_DISPLAY_SETTINGS_ACTIVITY_H
#define STARREADER_UI_DISPLAY_SETTINGS_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class DisplaySettingsActivity : public Activity {
public:
    DisplaySettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                            HalPowerManager* power, HalTouch* touch = nullptr,
                            HalFrontLight* frontLight = nullptr,
                            SettingsManager* settingsManager = nullptr);
    ~DisplaySettingsActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum DisplaySetting {
        SETTING_FONT_SIZE = 0,
        SETTING_LINE_SPACING,
        SETTING_MARGINS,
        SETTING_REFRESH_MODE,
        SETTING_ORIENTATION,
        SETTING_AUTO_REFRESH,
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

#endif // STARREADER_UI_DISPLAY_SETTINGS_ACTIVITY_H
