/**
 * StarReader Pro Firmware - Language Settings Activity
 * 
 * 语言设置界面
 * Language settings screen
 */

#ifndef STARREADER_UI_LANGUAGE_SETTINGS_ACTIVITY_H
#define STARREADER_UI_LANGUAGE_SETTINGS_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class LanguageSettingsActivity : public Activity {
public:
    LanguageSettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                             HalPowerManager* power, HalTouch* touch = nullptr,
                             HalFrontLight* frontLight = nullptr,
                             SettingsManager* settingsManager = nullptr);
    ~LanguageSettingsActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum LanguageItem {
        LANG_CHINESE = 0,
        LANG_ENGLISH,
        LANG_COUNT
    };

    int m_selectedItem;

    static const int STATUS_BAR_HEIGHT = 28;
    static const int LIST_ITEM_HEIGHT = 56;
    static const int LIST_PADDING = 20;

    void drawTitleBar();
    void drawLanguageList();
    void drawBottomHint();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void applyLanguage(int lang);
};

#endif // STARREADER_UI_LANGUAGE_SETTINGS_ACTIVITY_H
