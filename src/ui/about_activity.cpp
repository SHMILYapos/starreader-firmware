/**
 * StarReader Pro Firmware - About Activity Implementation
 * 
 * 关于设备界面实现
 */

#include "about_activity.h"
#include "../config.h"
#include "../utils/i18n.h"
#include <string.h>

AboutActivity::AboutActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                             HalPowerManager* power, HalTouch* touch,
                             HalFrontLight* frontLight,
                             SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_touch(touch)
    , m_frontLight(frontLight)
    , m_settingsManager(settingsManager)
    , m_needsRender(true) {
}

AboutActivity::~AboutActivity() {
}

void AboutActivity::onEnter() {
    m_needsRender = true;
}

void AboutActivity::onExit() {
}

void AboutActivity::onResume() {
    m_needsRender = true;
}

void AboutActivity::loop() {
    m_input->update();
    ButtonState event = m_input->getLastEvent();

    if (event.event == EVENT_PRESSED) {
        handleButton(event);
    }

    if (m_touch) {
        TouchEvent touchEvent = m_touch->getLastEvent();
        if (touchEvent.gesture != GESTURE_NONE) {
            handleTouch(touchEvent);
        }
    }
}

void AboutActivity::render() {
    if (!m_needsRender) return;

    m_display->clear(0xFF);
    
    // 标题栏
    int16_t width = m_display->getRotatedWidth();
    m_display->fillRect(0, 0, width, 35, 0x00);
    const char* title = _(STR_ABOUT_TITLE);
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 8, title, 0xFF, 2);

    drawAboutInfo();

    // 底部提示
    const char* hint = _(STR_BACK);
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, m_display->getRotatedHeight() - 20, 
                         hint, 0x40, 1);

    m_display->refresh(HalDisplay::FULL_REFRESH);
    m_needsRender = false;
}

void AboutActivity::drawAboutInfo() {
    int y = 60;
    int x = 50;
    int lineHeight = 35;

    // 固件版本
    m_display->drawString(x, y, _(STR_FIRMWARE_VERSION), 0x00, 1);
    m_display->drawString(x + 200, y, "v0.2.0 Pro", 0x00, 1);
    y += lineHeight;

    // 设备型号
    m_display->drawString(x, y, _(STR_DEVICE_MODEL), 0x00, 1);
    m_display->drawString(x + 200, y, "阅星瞳 X4 Pro", 0x00, 1);
    y += lineHeight;

    // 主控芯片
    m_display->drawString(x, y, _(STR_MCU), 0x00, 1);
    m_display->drawString(x + 200, y, "ESP32-C3", 0x00, 1);
    y += lineHeight;

    // 屏幕分辨率
    m_display->drawString(x, y, _(STR_RESOLUTION), 0x00, 1);
    char resBuf[32];
    snprintf(resBuf, sizeof(resBuf), "%dx%d", EPAPER_WIDTH, EPAPER_HEIGHT);
    m_display->drawString(x + 200, y, resBuf, 0x00, 1);
    y += lineHeight;

    // 分隔线
    m_display->drawLine(x, y, x + 300, y, 0x40);
    y += 20;

    // 开发者
    m_display->drawString(x, y, _(STR_AUTHOR), 0x00, 1);
    m_display->drawString(x + 200, y, "StarReader Community", 0x00, 1);
    y += lineHeight;

    // 开源协议
    m_display->drawString(x, y, _(STR_LICENSE), 0x00, 1);
    m_display->drawString(x + 200, y, _(STR_MIT), 0x00, 1);
    y += lineHeight;

    // 开源社区
    m_display->drawString(x, y, _(STR_COMMUNITY), 0x00, 1);
    m_display->drawString(x + 200, y, "GitHub: SHMILYapos", 0x00, 1);
    y += lineHeight;

    // 感谢
    y += 20;
    m_display->drawString(x, y, "感谢开源社区贡献者", 0x40, 1);
    y += lineHeight;
    m_display->drawString(x + 20, y, "CrossPoint Reader / open-x4-epaper", 0x40, 1);
}

void AboutActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_BACK:
        case BTN_CONFIRM:
            if (m_manager) {
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}

void AboutActivity::handleTouch(TouchEvent event) {
    switch (event.gesture) {
        case GESTURE_TAP:
        case GESTURE_SWIPE_RIGHT:
            if (m_manager) {
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}
