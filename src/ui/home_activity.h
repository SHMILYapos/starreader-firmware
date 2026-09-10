/**
 * StarReader Pro Firmware - Home Activity (Main Menu)
 * 
 * Main menu with touch support and status display
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
    enum MenuItem {
        MENU_CONTINUE_READING = 0,
        MENU_LIBRARY,
        MENU_WIFI,
        MENU_SETTINGS,
        MENU_ABOUT,
        MENU_SLEEP,
        MENU_COUNT
    };

    int m_selectedItem;
    uint32_t m_lastButtonTime;
    bool m_needsRender;

    // Hardware references
    HalTouch* m_touch;
    HalFrontLight* m_frontLight;
    SettingsManager* m_settingsManager;

    // UI layout constants
    static const int MENU_START_Y = 80;
    static const int MENU_ITEM_HEIGHT = 55;
    static const int MENU_ITEM_PADDING = 20;

    void drawStatusBar();
    void drawMenu();
    void drawFooter();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void launchMenuItem(int item);
};

#endif // STARREADER_UI_HOME_ACTIVITY_H
