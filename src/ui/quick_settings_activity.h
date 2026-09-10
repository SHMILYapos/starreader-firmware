/**
 * StarReader Pro Firmware - Quick Settings Panel
 * 
 * 顶部下拉快捷面板（前光/亮度/色温）
 * Quick settings dropdown panel
 */

#ifndef STARREADER_UI_QUICK_SETTINGS_ACTIVITY_H
#define STARREADER_UI_QUICK_SETTINGS_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class QuickSettingsActivity : public Activity {
public:
    QuickSettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                          HalPowerManager* power, HalTouch* touch = nullptr,
                          HalFrontLight* frontLight = nullptr,
                          SettingsManager* settingsManager = nullptr);
    ~QuickSettingsActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum SliderType {
        SLIDER_BRIGHTNESS = 0,
        SLIDER_WARMTH,
        SLIDER_COUNT
    };

    int m_selectedSlider;
    int m_brightness;
    int m_warmth;
    bool m_frontLightOn;

    static const int PANEL_HEIGHT = 200;
    static const int SLIDER_HEIGHT = 50;
    static const int SLIDER_PADDING = 20;

    void drawPanelBackground();
    void drawToggleSwitch(int y, const char* label, bool on);
    void drawSlider(int y, const char* label, int value, int maxValue);
    void drawCloseHint();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void adjustSlider(int slider, int direction);
};

#endif // STARREADER_UI_QUICK_SETTINGS_ACTIVITY_H
