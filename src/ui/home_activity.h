/**
 * StarReader Firmware - Home Activity (Main Menu)
 */

#ifndef STARREADER_UI_HOME_ACTIVITY_H
#define STARREADER_UI_HOME_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/storage.h"
#include "../hal/power.h"

class HomeActivity : public Activity {
public:
    HomeActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power);
    ~HomeActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum MenuItem {
        MENU_LIBRARY = 0,
        MENU_SETTINGS,
        MENU_ABOUT,
        MENU_SLEEP,
        MENU_COUNT
    };

    int m_selectedItem;
    uint32_t m_lastButtonTime;
    bool m_needsRender;

    // UI layout constants
    static const int MENU_START_Y = 100;
    static const int MENU_ITEM_HEIGHT = 60;
    static const int MENU_ITEM_PADDING = 20;

    void drawStatusBar();
    void drawMenu();
    void handleButton(ButtonState event);
};

#endif // STARREADER_UI_HOME_ACTIVITY_H
