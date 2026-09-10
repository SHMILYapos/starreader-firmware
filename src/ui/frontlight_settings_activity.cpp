/**
 * StarReader Pro Firmware - FrontLight Settings Implementation
 * 
 * 前光设置子页实现
 */

#include "frontlight_settings_activity.h"
#include "../config.h"
#include <string.h>
#include <cstdio>

FrontLightSettingsActivity::FrontLightSettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                                       HalPowerManager* power, HalTouch* touch,
                                                       HalFrontLight* frontLight,
                                                       SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedItem(0)
    , m_brightness(50)
    , m_warmth(50)
    , m_frontLightOn(false) {
}

FrontLightSettingsActivity::~FrontLightSettingsActivity() {
}

void FrontLightSettingsActivity::onEnter() {
    if (m_frontLight) {
        m_frontLightOn = m_frontLight->isOn();
        m_brightness = m_frontLight->getBrightness();
        m_warmth = m_frontLight->getWarmth();
    }
    m_selectedItem = 0;
    requestUpdate();
}

void FrontLightSettingsActivity::onExit() {
    applySettings();
}

void FrontLightSettingsActivity::onResume() {
    requestUpdate();
}

void FrontLightSettingsActivity::loop() {
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

void FrontLightSettingsActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    drawTitleBar();
    drawToggleRow();
    drawBrightnessSlider();
    drawWarmthSlider();
    drawPresetRow();
    drawBottomHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void FrontLightSettingsActivity::drawTitleBar() {
    int16_t width = m_display->getRotatedWidth();

    m_display->fillRect(0, 0, width, STATUS_BAR_HEIGHT, 0x00);

    m_display->drawString(10, 6, "< 返回", 0xFF, 1);

    const char* title = "前光设置";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 4, title, 0xFF, 2);
}

void FrontLightSettingsActivity::drawToggleRow() {
    int16_t width = m_display->getRotatedWidth();
    int y = STATUS_BAR_HEIGHT + LIST_PADDING;

    bool selected = (m_selectedItem == SETTING_TOGGLE);

    if (selected) {
        m_display->fillRect(LIST_PADDING, y, width - 2 * LIST_PADDING, 50, 0x00);
        m_display->drawString(LIST_PADDING + 15, y + 15, "前光开关", 0xFF, 2);
        m_display->drawString(width - 100, y + 17, m_frontLightOn ? "ON" : "OFF", 0xFF, 1);
    } else {
        m_display->drawString(LIST_PADDING + 15, y + 15, "前光开关", 0x00, 2);
        m_display->drawString(width - 100, y + 17, m_frontLightOn ? "ON" : "OFF", 0x60, 1);
    }
}

void FrontLightSettingsActivity::drawBrightnessSlider() {
    int16_t width = m_display->getRotatedWidth();
    int y = STATUS_BAR_HEIGHT + LIST_PADDING + SLIDER_HEIGHT;

    bool selected = (m_selectedItem == SETTING_BRIGHTNESS);

    // 标签
    m_display->drawString(LIST_PADDING + 15, y + 10, "亮度", 0x00, 2);

    // 数值
    char valueText[16];
    snprintf(valueText, sizeof(valueText), "%d%%", m_brightness);
    m_display->drawString(width - 80, y + 12, valueText, 0x00, 2);

    // 滑块轨道
    int trackX = LIST_PADDING + 80;
    int trackY = y + 35;
    int trackW = width - trackX - 80;
    int trackH = 12;

    m_display->drawRect(trackX, trackY, trackW, trackH, 0x00);

    int fillW = (trackW * m_brightness) / 100;
    if (fillW > 0) {
        m_display->fillRect(trackX + 1, trackY + 1, fillW - 2, trackH - 2, 0x00);
    }

    if (selected) {
        m_display->drawString(LIST_PADDING, y + 12, ">", 0x00, 2);
    }
}

void FrontLightSettingsActivity::drawWarmthSlider() {
    int16_t width = m_display->getRotatedWidth();
    int y = STATUS_BAR_HEIGHT + LIST_PADDING + SLIDER_HEIGHT * 2;

    bool selected = (m_selectedItem == SETTING_WARMTH);

    // 标签
    m_display->drawString(LIST_PADDING + 15, y + 10, "色温", 0x00, 2);

    // 数值
    char valueText[16];
    snprintf(valueText, sizeof(valueText), "%d%%", m_warmth);
    m_display->drawString(width - 80, y + 12, valueText, 0x00, 2);

    // 滑块轨道
    int trackX = LIST_PADDING + 80;
    int trackY = y + 35;
    int trackW = width - trackX - 80;
    int trackH = 12;

    m_display->drawRect(trackX, trackY, trackW, trackH, 0x00);

    int fillW = (trackW * m_warmth) / 100;
    if (fillW > 0) {
        m_display->fillRect(trackX + 1, trackY + 1, fillW - 2, trackH - 2, 0x00);
    }

    if (selected) {
        m_display->drawString(LIST_PADDING, y + 12, ">", 0x00, 2);
    }
}

void FrontLightSettingsActivity::drawPresetRow() {
    int16_t width = m_display->getRotatedWidth();
    int y = STATUS_BAR_HEIGHT + LIST_PADDING + SLIDER_HEIGHT * 3;

    bool selected = (m_selectedItem == SETTING_PRESET);

    if (selected) {
        m_display->fillRect(LIST_PADDING, y, width - 2 * LIST_PADDING, 50, 0x00);
        m_display->drawString(LIST_PADDING + 15, y + 15, "预设模式", 0xFF, 2);
        m_display->drawString(width - 100, y + 17, "阅读模式", 0xFF, 1);
    } else {
        m_display->drawString(LIST_PADDING + 15, y + 15, "预设模式", 0x00, 2);
        m_display->drawString(width - 100, y + 17, "阅读模式", 0x60, 1);
    }
}

void FrontLightSettingsActivity::drawBottomHint() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint = "上下键: 选择  左右键: 调节  确认: 切换";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x60, 1);
}

void FrontLightSettingsActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_VOL_UP:
        case BTN_LEFT:
            if (m_selectedItem > 0) {
                m_selectedItem--;
                requestUpdate();
            }
            break;

        case BTN_VOL_DOWN:
        case BTN_RIGHT:
            if (m_selectedItem < SETTING_COUNT - 1) {
                m_selectedItem++;
                requestUpdate();
            }
            break;

        case BTN_CONFIRM:
            if (m_selectedItem == SETTING_TOGGLE) {
                m_frontLightOn = !m_frontLightOn;
                if (m_frontLight) {
                    if (m_frontLightOn) {
                        m_frontLight->setBrightness(m_brightness);
                        m_frontLight->setWarmth(m_warmth);
                        m_frontLight->on();
                    } else {
                        m_frontLight->off();
                    }
                }
            } else if (m_selectedItem == SETTING_PRESET) {
                // 循环切换预设
                static int presetIdx = 0;
                presetIdx = (presetIdx + 1) % 3;
            }
            requestUpdate();
            break;

        case BTN_BACK:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void FrontLightSettingsActivity::handleTouch(TouchEvent event) {
    int16_t width = m_display->getRotatedWidth();

    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;
            int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

            // 开关行
            if (y < startY + 50) {
                m_selectedItem = SETTING_TOGGLE;
                m_frontLightOn = !m_frontLightOn;
                if (m_frontLight) {
                    if (m_frontLightOn) {
                        m_frontLight->setBrightness(m_brightness);
                        m_frontLight->setWarmth(m_warmth);
                        m_frontLight->on();
                    } else {
                        m_frontLight->off();
                    }
                }
                requestUpdate();
                break;
            }

            // 亮度滑块
            int brightY = startY + SLIDER_HEIGHT;
            if (y >= brightY && y < brightY + SLIDER_HEIGHT) {
                m_selectedItem = SETTING_BRIGHTNESS;
                int trackX = LIST_PADDING + 80;
                int trackW = width - trackX - 80;
                int clickX = event.endPoint.x;
                if (clickX > trackX && clickX < trackX + trackW) {
                    m_brightness = ((clickX - trackX) * 100) / trackW;
                    if (m_frontLight && m_frontLightOn) {
                        m_frontLight->setBrightness(m_brightness);
                    }
                }
                requestUpdate();
                break;
            }

            // 色温滑块
            int warmY = startY + SLIDER_HEIGHT * 2;
            if (y >= warmY && y < warmY + SLIDER_HEIGHT) {
                m_selectedItem = SETTING_WARMTH;
                int trackX = LIST_PADDING + 80;
                int trackW = width - trackX - 80;
                int clickX = event.endPoint.x;
                if (clickX > trackX && clickX < trackX + trackW) {
                    m_warmth = ((clickX - trackX) * 100) / trackW;
                    if (m_frontLight && m_frontLightOn) {
                        m_frontLight->setWarmth(m_warmth);
                    }
                }
                requestUpdate();
                break;
            }

            // 预设行
            int presetY = startY + SLIDER_HEIGHT * 3;
            if (y >= presetY && y < presetY + 50) {
                m_selectedItem = SETTING_PRESET;
                requestUpdate();
                break;
            }
            break;
        }

        case GESTURE_SWIPE_RIGHT:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void FrontLightSettingsActivity::adjustValue(int setting, int direction) {
    if (setting == SETTING_BRIGHTNESS) {
        m_brightness += direction * 5;
        if (m_brightness < 0) m_brightness = 0;
        if (m_brightness > 100) m_brightness = 100;
        if (m_frontLight && m_frontLightOn) {
            m_frontLight->setBrightness(m_brightness);
        }
    } else if (setting == SETTING_WARMTH) {
        m_warmth += direction * 5;
        if (m_warmth < 0) m_warmth = 0;
        if (m_warmth > 100) m_warmth = 100;
        if (m_frontLight && m_frontLightOn) {
            m_frontLight->setWarmth(m_warmth);
        }
    }
    requestUpdate();
}

void FrontLightSettingsActivity::applySettings() {
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        s->brightness = m_brightness;
        s->frontLightWarmth = m_warmth;
        s->frontLightOn = m_frontLightOn ? 1 : 0;
        m_settingsManager->save();
    }
}
