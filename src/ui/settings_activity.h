/**
 * StarReader Pro Firmware - Settings Activity
 * 
 * 设置菜单界面（双语支持）
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
    enum SettingsItem {
        // Front Light group
        SETTING_FRONT_LIGHT_ON = 0,
        SETTING_BRIGHTNESS,
        SETTING_WARMTH,
        SETTING_PRESETS,

        // Display group
        SETTING_FONT_SIZE,
        SETTING_ORIENTATION,
        SETTING_REFRESH_MODE,

        // Reader group
        SETTING_LINE_SPACING,
        SETTING_JUSTIFICATION,

        // Power group
        SETTING_SLEEP_TIMEOUT,
        SETTING_AUTO_REFRESH,

        // System
        SETTING_LANGUAGE,
        SETTING_ABOUT,
        SETTING_COUNT
    };

    int m_selectedItem;
    bool m_needsRender;

    // Hardware references
    HalTouch* m_touch;
    HalFrontLight* m_frontLight;
    SettingsManager* m_settingsManager;

    // Settings values
    bool m_frontLightOn;
    uint8_t m_brightness;
    uint8_t m_warmth;
    uint8_t m_fontSize;
    uint8_t m_orientation;
    uint8_t m_refreshMode;
    uint8_t m_lineSpacing;
    uint8_t m_justification;
    uint8_t m_language;
    uint16_t m_sleepTimeoutMin;
    uint16_t m_autoRefreshPages;

    static const int MENU_START_Y = 50;
    static const int MENU_ITEM_HEIGHT = 32;

    void drawSettings();
    void drawSlider(int y, const char* label, int value, int maxValue);
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void cycleSetting(int item, int direction);
    const char* getSettingValueText(int item);
    void applySettings();
};

#endif // STARREADER_UI_SETTINGS_ACTIVITY_H
