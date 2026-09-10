/**
 * StarReader Pro Firmware - About Activity
 * 
 * 关于设备界面
 * About screen showing device information
 */

#ifndef STARREADER_UI_ABOUT_ACTIVITY_H
#define STARREADER_UI_ABOUT_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class AboutActivity : public Activity {
public:
    AboutActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                  HalPowerManager* power, HalTouch* touch = nullptr,
                  HalFrontLight* frontLight = nullptr,
                  SettingsManager* settingsManager = nullptr);
    ~AboutActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    HalTouch* m_touch;
    HalFrontLight* m_frontLight;
    SettingsManager* m_settingsManager;
    bool m_needsRender;

    void drawAboutInfo();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
};

#endif // STARREADER_UI_ABOUT_ACTIVITY_H
