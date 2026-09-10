/**
 * StarReader Firmware - Settings Activity
 */

#ifndef STARREADER_UI_SETTINGS_ACTIVITY_H
#define STARREADER_UI_SETTINGS_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/storage.h"
#include "../hal/power.h"

class SettingsActivity : public Activity {
public:
    SettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power);
    ~SettingsActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum SettingsItem {
        SETTING_FONT_SIZE = 0,
        SETTING_ORIENTATION,
        SETTING_REFRESH_MODE,
        SETTING_SLEEP_TIMEOUT,
        SETTING_ABOUT,
        SETTING_COUNT
    };

    int m_selectedItem;
    bool m_needsRender;

    // Settings values
    uint8_t m_fontSize;
    uint8_t m_orientation;
    uint8_t m_refreshMode;
    uint16_t m_sleepTimeoutMin;

    static const int MENU_START_Y = 60;
    static const int MENU_ITEM_HEIGHT = 40;

    void drawSettings();
    void handleButton(ButtonState event);
    void cycleSetting(int item);
    const char* getSettingValueText(int item);
};

#endif // STARREADER_UI_SETTINGS_ACTIVITY_H
