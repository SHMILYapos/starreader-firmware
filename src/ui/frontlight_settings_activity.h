/**
 * StarReader Pro Firmware - FrontLight Settings Activity
 * 
 * 前光设置子页：亮度/色温/预设
 * Front light settings sub-page
 */

#ifndef STARREADER_UI_FRONTLIGHT_SETTINGS_ACTIVITY_H
#define STARREADER_UI_FRONTLIGHT_SETTINGS_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class FrontLightSettingsActivity : public Activity {
public:
    FrontLightSettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                               HalPowerManager* power, HalTouch* touch = nullptr,
                               HalFrontLight* frontLight = nullptr,
                               SettingsManager* settingsManager = nullptr);
    ~FrontLightSettingsActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum FrontLightSetting {
        SETTING_TOGGLE = 0,
        SETTING_BRIGHTNESS,
        SETTING_WARMTH,
        SETTING_PRESET,
        SETTING_COUNT
    };

    int m_selectedItem;
    int m_brightness;
    int m_warmth;
    bool m_frontLightOn;

    static const int STATUS_BAR_HEIGHT = 28;
    static const int SLIDER_HEIGHT = 60;
    static const int LIST_PADDING = 10;

    void drawTitleBar();
    void drawToggleRow();
    void drawBrightnessSlider();
    void drawWarmthSlider();
    void drawPresetRow();
    void drawBottomHint();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void adjustValue(int setting, int direction);
    void applySettings();
};

#endif // STARREADER_UI_FRONTLIGHT_SETTINGS_ACTIVITY_H
