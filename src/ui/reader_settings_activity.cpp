/**
 * StarReader Pro Firmware - Reader Settings Implementation
 * 
 * 阅读设置子页实现
 */

#include "reader_settings_activity.h"
#include "../config.h"
#include <string.h>
#include <cstdio>

ReaderSettingsActivity::ReaderSettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                               HalPowerManager* power, HalTouch* touch,
                                               HalFrontLight* frontLight,
                                               SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedItem(0) {
}

ReaderSettingsActivity::~ReaderSettingsActivity() {
}

void ReaderSettingsActivity::onEnter() {
    m_selectedItem = 0;
    requestUpdate();
}

void ReaderSettingsActivity::onExit() {
    applySettings();
}

void ReaderSettingsActivity::onResume() {
    requestUpdate();
}

void ReaderSettingsActivity::loop() {
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

void ReaderSettingsActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    drawTitleBar();
    drawSettingList();
    drawBottomHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void ReaderSettingsActivity::drawTitleBar() {
    int16_t width = m_display->getRotatedWidth();

    m_display->fillRect(0, 0, width, STATUS_BAR_HEIGHT, 0x00);

    m_display->drawString(10, 6, "< 返回", 0xFF, 1);

    const char* title = "阅读设置";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 4, title, 0xFF, 2);
}

void ReaderSettingsActivity::drawSettingList() {
    int16_t width = m_display->getRotatedWidth();

    const char* itemNames[] = {
        "翻页方式",
        "状态栏显示",
        "进度同步",
        "电量显示"
    };

    int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

    for (int i = 0; i < SETTING_COUNT; i++) {
        int y = startY + i * LIST_ITEM_HEIGHT;

        bool selected = (i == m_selectedItem);

        if (selected) {
            m_display->fillRect(LIST_PADDING, y, width - 2 * LIST_PADDING, LIST_ITEM_HEIGHT - 5, 0x00);
            m_display->drawString(LIST_PADDING + 15, y + 12, itemNames[i], 0xFF, 2);
            m_display->drawString(width - 140, y + 14, getSettingValueText(i), 0xFF, 1);
        } else {
            m_display->drawString(LIST_PADDING + 15, y + 12, itemNames[i], 0x00, 2);
            m_display->drawString(width - 140, y + 14, getSettingValueText(i), 0x60, 1);
        }
    }
}

void ReaderSettingsActivity::drawBottomHint() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint = "上下键: 选择  左右键: 调节  返回: 返回";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x60, 1);
}

void ReaderSettingsActivity::handleButton(ButtonState event) {
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
            cycleSetting(m_selectedItem, 1);
            break;

        case BTN_BACK:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void ReaderSettingsActivity::handleTouch(TouchEvent event) {
    int16_t width = m_display->getRotatedWidth();

    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;
            int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

            for (int i = 0; i < SETTING_COUNT; i++) {
                int itemY = startY + i * LIST_ITEM_HEIGHT;
                if (y >= itemY && y < itemY + LIST_ITEM_HEIGHT - 5) {
                    m_selectedItem = i;
                    cycleSetting(i, 1);
                    break;
                }
            }
            break;
        }

        case GESTURE_SWIPE_LEFT:
            cycleSetting(m_selectedItem, 1);
            break;

        case GESTURE_SWIPE_RIGHT:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void ReaderSettingsActivity::cycleSetting(int item, int direction) {
    if (!m_settingsManager) return;
    StarReaderSettings* s = m_settingsManager->getSettings();

    switch (item) {
        case SETTING_PAGE_TURN:
            s->pageTurnMode = (s->pageTurnMode + direction + 3) % 3;
            break;
        case SETTING_STATUS_BAR:
            s->statusBarMode = (s->statusBarMode + direction + 6) % 6;
            break;
        case SETTING_PROGRESS_SYNC:
            s->progressSync = (s->progressSync + direction + 2) % 2;
            break;
        case SETTING_BATTERY_DISPLAY:
            s->batteryDisplay = (s->batteryDisplay + direction + 2) % 2;
            break;
    }

    requestUpdate();
}

const char* ReaderSettingsActivity::getSettingValueText(int item) {
    if (!m_settingsManager) return "";
    StarReaderSettings* s = m_settingsManager->getSettings();
    static char buf[32];

    switch (item) {
        case SETTING_PAGE_TURN:
            switch (s->pageTurnMode) {
                case 0: return "左右翻页";
                case 1: return "上下翻页";
                case 2: return "触屏翻页";
                default: return "?";
            }
        case SETTING_STATUS_BAR:
            switch (s->statusBarMode) {
                case 0: return "隐藏";
                case 1: return "仅电量";
                case 2: return "完整信息";
                case 3: return "章节进度";
                case 4: return "全书进度";
                case 5: return "百分比";
                default: return "?";
            }
        case SETTING_PROGRESS_SYNC:
            return s->progressSync ? "开启" : "关闭";
        case SETTING_BATTERY_DISPLAY:
            return s->batteryDisplay ? "百分比" : "图标";
        default:
            return "";
    }
}

void ReaderSettingsActivity::applySettings() {
    if (m_settingsManager) {
        m_settingsManager->save();
    }
}
