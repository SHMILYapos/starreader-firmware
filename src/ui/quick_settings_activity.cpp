/**
 * StarReader Pro Firmware - Quick Settings Panel Implementation
 * 
 * 顶部下拉快捷面板实现
 */

#include "quick_settings_activity.h"
#include "../config.h"
#include <string.h>
#include <cstdio>

QuickSettingsActivity::QuickSettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                             HalPowerManager* power, HalTouch* touch,
                                             HalFrontLight* frontLight,
                                             SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedSlider(0)
    , m_brightness(50)
    , m_warmth(50)
    , m_frontLightOn(false) {
}

QuickSettingsActivity::~QuickSettingsActivity() {
}

void QuickSettingsActivity::onEnter() {
    // 读取当前前光状态
    if (m_frontLight) {
        m_frontLightOn = m_frontLight->isOn();
        m_brightness = m_frontLight->getBrightness();
        m_warmth = m_frontLight->getWarmth();
    }
    requestUpdate();
}

void QuickSettingsActivity::onExit() {
    // 保存设置
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        s->brightness = m_brightness;
        s->frontLightWarmth = m_warmth;
        s->frontLightOn = m_frontLightOn ? 1 : 0;
        m_settingsManager->save();
    }
}

void QuickSettingsActivity::onResume() {
    requestUpdate();
}

void QuickSettingsActivity::loop() {
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

void QuickSettingsActivity::render() {
    if (!needsRender()) return;

    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // 半透明遮罩效果（用深灰表示，墨水屏没有真透明）
    m_display->fillRect(0, PANEL_HEIGHT, width, height - PANEL_HEIGHT, 0x80);

    // 面板背景（白底）
    m_display->fillRect(0, 0, width, PANEL_HEIGHT, 0xFF);

    // 面板顶部边框
    m_display->drawRect(0, 0, width, PANEL_HEIGHT, 0x00);

    int y = 10;

    // 前光开关
    drawToggleSwitch(y, "前光", m_frontLightOn);
    y += SLIDER_HEIGHT;

    // 亮度滑块
    drawSlider(y, "亮度", m_brightness, 100);
    y += SLIDER_HEIGHT;

    // 色温滑块
    drawSlider(y, "色温", m_warmth, 100);
    y += SLIDER_HEIGHT;

    // 关闭提示
    drawCloseHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void QuickSettingsActivity::drawPanelBackground() {
    // 已在render中处理
}

void QuickSettingsActivity::drawToggleSwitch(int y, const char* label, bool on) {
    int16_t width = m_display->getRotatedWidth();

    // 标签
    m_display->drawString(SLIDER_PADDING, y + 15, label, 0x00, 2);

    // 开关背景
    int switchX = width - 80;
    int switchY = y + 12;
    int switchW = 60;
    int switchH = 25;

    m_display->drawRect(switchX, switchY, switchW, switchH, 0x00);

    if (on) {
        // 开：填充
        m_display->fillRect(switchX + 1, switchY + 1, switchW - 2, switchH - 2, 0x00);
        m_display->drawString(switchX + 15, switchY + 5, "ON", 0xFF, 1);
    } else {
        // 关：空心
        m_display->drawString(switchX + 12, switchY + 5, "OFF", 0x00, 1);
    }
}

void QuickSettingsActivity::drawSlider(int y, const char* label, int value, int maxValue) {
    int16_t width = m_display->getRotatedWidth();

    // 标签
    m_display->drawString(SLIDER_PADDING, y + 15, label, 0x00, 2);

    // 数值
    char valueText[16];
    snprintf(valueText, sizeof(valueText), "%d%%", value);
    m_display->drawString(width - 60, y + 15, valueText, 0x00, 2);

    // 滑块轨道
    int trackX = SLIDER_PADDING + 80;
    int trackY = y + 20;
    int trackW = width - trackX - 80;
    int trackH = 12;

    m_display->drawRect(trackX, trackY, trackW, trackH, 0x00);

    // 滑块填充
    int fillW = (trackW * value) / maxValue;
    if (fillW > 0) {
        m_display->fillRect(trackX + 1, trackY + 1, fillW - 2, trackH - 2, 0x00);
    }

    // 选中指示
    bool isSelected = (m_selectedSlider == (y == SLIDER_PADDING + SLIDER_HEIGHT ? SLIDER_BRIGHTNESS : SLIDER_WARMTH));
    if (isSelected) {
        m_display->drawString(SLIDER_PADDING, y - 2, ">", 0x00, 1);
    }
}

void QuickSettingsActivity::drawCloseHint() {
    int16_t width = m_display->getRotatedWidth();
    const char* hint = "点击任意处关闭";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, PANEL_HEIGHT - 20, hint, 0x60, 1);
}

void QuickSettingsActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_VOL_UP:
        case BTN_LEFT:
            adjustSlider(m_selectedSlider, -5);
            break;

        case BTN_VOL_DOWN:
        case BTN_RIGHT:
            adjustSlider(m_selectedSlider, 5);
            break;

        case BTN_CONFIRM:
            // 切换前光开关
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

        case BTN_BACK:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void QuickSettingsActivity::handleTouch(TouchEvent event) {
    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;
            
            // 点击面板外区域关闭
            if (y > PANEL_HEIGHT) {
                if (m_manager) m_manager->goBack();
                break;
            }

            // 点击前光开关行
            if (y < SLIDER_HEIGHT) {
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

            // 点击亮度滑块
            if (y < SLIDER_HEIGHT * 2) {
                m_selectedSlider = SLIDER_BRIGHTNESS;
                // 计算点击位置对应的亮度值
                int16_t width = m_display->getRotatedWidth();
                int trackX = SLIDER_PADDING + 80;
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

            // 点击色温滑块
            if (y < SLIDER_HEIGHT * 3) {
                m_selectedSlider = SLIDER_WARMTH;
                int16_t width = m_display->getRotatedWidth();
                int trackX = SLIDER_PADDING + 80;
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
            break;
        }

        case GESTURE_SWIPE_DOWN:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void QuickSettingsActivity::adjustSlider(int slider, int direction) {
    if (slider == SLIDER_BRIGHTNESS) {
        m_brightness += direction;
        if (m_brightness < 0) m_brightness = 0;
        if (m_brightness > 100) m_brightness = 100;
        if (m_frontLight && m_frontLightOn) {
            m_frontLight->setBrightness(m_brightness);
        }
    } else {
        m_warmth += direction;
        if (m_warmth < 0) m_warmth = 0;
        if (m_warmth > 100) m_warmth = 100;
        if (m_frontLight && m_frontLightOn) {
            m_frontLight->setWarmth(m_warmth);
        }
    }
    requestUpdate();
}
