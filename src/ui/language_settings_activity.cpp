/**
 * StarReader Pro Firmware - Language Settings Implementation
 * 
 * 语言设置界面实现
 */

#include "language_settings_activity.h"
#include "../config.h"
#include <string.h>
#include <cstdio>

LanguageSettingsActivity::LanguageSettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                                   HalPowerManager* power, HalTouch* touch,
                                                   HalFrontLight* frontLight,
                                                   SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedItem(0) {
}

LanguageSettingsActivity::~LanguageSettingsActivity() {
}

void LanguageSettingsActivity::onEnter() {
    // 读取当前语言设置
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        m_selectedItem = s->language;
    }
    requestUpdate();
}

void LanguageSettingsActivity::onExit() {
    // 保存语言设置
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        s->language = m_selectedItem;
        m_settingsManager->save();
    }
}

void LanguageSettingsActivity::onResume() {
    requestUpdate();
}

void LanguageSettingsActivity::loop() {
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

void LanguageSettingsActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    drawTitleBar();
    drawLanguageList();
    drawBottomHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void LanguageSettingsActivity::drawTitleBar() {
    int16_t width = m_display->getRotatedWidth();

    m_display->fillRect(0, 0, width, STATUS_BAR_HEIGHT, 0x00);

    m_display->drawString(10, 6, "< 返回", 0xFF, 1);

    const char* title = "语言设置 / Language";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 4, title, 0xFF, 2);
}

void LanguageSettingsActivity::drawLanguageList() {
    int16_t width = m_display->getRotatedWidth();

    const char* langNames[] = {
        "简体中文",
        "English"
    };

    const char* langDescs[] = {
        "Chinese Simplified",
        "英文"
    };

    int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

    for (int i = 0; i < LANG_COUNT; i++) {
        int y = startY + i * LIST_ITEM_HEIGHT;

        bool selected = (i == m_selectedItem);

        if (selected) {
            m_display->fillRect(LIST_PADDING, y, width - 2 * LIST_PADDING, LIST_ITEM_HEIGHT - 10, 0x00);
            m_display->drawString(LIST_PADDING + 20, y + 12, langNames[i], 0xFF, 2);
            m_display->drawString(LIST_PADDING + 20, y + 35, langDescs[i], 0xCC, 1);
            
            // 选中标记
            m_display->drawString(width - 60, y + 20, "✓", 0xFF, 2);
        } else {
            m_display->drawString(LIST_PADDING + 20, y + 12, langNames[i], 0x00, 2);
            m_display->drawString(LIST_PADDING + 20, y + 35, langDescs[i], 0x60, 1);
        }
    }
}

void LanguageSettingsActivity::drawBottomHint() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint = "上下键: 选择  确认: 切换  返回: 返回";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x60, 1);
}

void LanguageSettingsActivity::handleButton(ButtonState event) {
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
            if (m_selectedItem < LANG_COUNT - 1) {
                m_selectedItem++;
                requestUpdate();
            }
            break;

        case BTN_CONFIRM:
            applyLanguage(m_selectedItem);
            break;

        case BTN_BACK:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void LanguageSettingsActivity::handleTouch(TouchEvent event) {
    int16_t width = m_display->getRotatedWidth();

    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;
            int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

            for (int i = 0; i < LANG_COUNT; i++) {
                int itemY = startY + i * LIST_ITEM_HEIGHT;
                if (y >= itemY && y < itemY + LIST_ITEM_HEIGHT - 10) {
                    m_selectedItem = i;
                    applyLanguage(i);
                    break;
                }
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

void LanguageSettingsActivity::applyLanguage(int lang) {
    m_selectedItem = lang;
    
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        s->language = lang;
        m_settingsManager->save();
    }
    
    requestUpdate();
}
