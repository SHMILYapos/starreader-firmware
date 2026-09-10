/**
 * StarReader Pro Firmware - Reader Menu
 * 
 * 阅读器底部弹出菜单
 * Reader popup menu (bottom sheet)
 */

#ifndef STARREADER_UI_READER_MENU_ACTIVITY_H
#define STARREADER_UI_READER_MENU_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class ReaderMenuActivity : public Activity {
public:
    ReaderMenuActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                       HalPowerManager* power, HalTouch* touch = nullptr,
                       HalFrontLight* frontLight = nullptr,
                       SettingsManager* settingsManager = nullptr);
    ~ReaderMenuActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum MenuItem {
        MENU_TOC = 0,
        MENU_BOOKMARK,
        MENU_SETTINGS,
        MENU_NIGHT_MODE,
        MENU_RETURN_HOME,
        MENU_COUNT
    };

    int m_selectedItem;

    static const int MENU_HEIGHT = 240;
    static const int MENU_ITEM_HEIGHT = 45;

    void drawMenuBackground();
    void drawMenuItems();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void selectItem(int item);
};

#endif // STARREADER_UI_READER_MENU_ACTIVITY_H
