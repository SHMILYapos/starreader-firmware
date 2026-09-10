/**
 * StarReader Pro Firmware - Display Settings Implementation
 * 
 * 显示设置子页实现
 */

#include "display_settings_activity.h"
#include "../config.h"
#include <string.h>
#include <cstdio>

DisplaySettingsActivity::DisplaySettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                                  HalPowerManager* power, HalTouch* touch,
                                                  HalFrontLight* frontLight,
                                                  SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedItem(0) {
}

DisplaySettingsActivity::~DisplaySettingsActivity() {
}

void DisplaySettingsActivity::onEnter() {
    m_selectedItem = 0;
    requestUpdate();
}

void DisplaySettingsActivity::onExit() {
    applySettings();
}

void DisplaySettingsActivity::onResume() {
    requestUpdate();
}

void DisplaySettingsActivity::loop() {
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

void DisplaySettingsActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    drawTitleBar();
    drawSettingList();
    drawBottomHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void DisplaySettingsActivity::drawTitleBar() {
    int16_t width = m_display->getRotatedWidth();

    m_display->fillRect(0, 0, width, STATUS_BAR_HEIGHT, 0x00);

    m_display->drawString(10, 6, "< 返回", 0xFF, 1);

    const char* title = "显示设置";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 4, title, 0xFF, 2);
}

void DisplaySettingsActivity::drawSettingList() {
    int16_t width = m_display->getRotatedWidth();

    const char* itemNames[] = {
        "字体大小",
        "行间距",
        "页边距",
        "刷新模式",
        "屏幕方向",
        "自动全刷"
    };

    int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

    for (int i = 0; i < SETTING_COUNT; i++) {
        int y = startY + i * LIST_ITEM_HEIGHT;

        bool selected = (i == m_selectedItem);

        if (selected) {
            m_display->fillRect(LIST_PADDING, y, width - 2 * LIST_PADDING, LIST_ITEM_HEIGHT - 5, 0x00);
            m_display->drawString(LIST_PADDING + 15, y + 12, itemNames[i], 0xFF, 2);
            m_display->drawString(width - 120, y + 14, getSettingValueText(i), 0xFF, 1);
        } else {
            m_display->drawString(LIST_PADDING + 15, y + 12, itemNames[i], 0x00, 2);
            m_display->drawString(width - 120, y + 14, getSettingValueText(i), 0x60, 1);
        }
    }
}

void DisplaySettingsActivity::drawBottomHint() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint = "上下键: 选择  左右键: 调节  返回: 返回";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x60, 1);
}

void DisplaySettingsActivity::handleButton(ButtonState event) {
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
            // 循环切换当前设置项
            cycleSetting(m_selectedItem, 1);
            break;

        case BTN_BACK:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void DisplaySettingsActivity::handleTouch(TouchEvent event) {
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

void DisplaySettingsActivity::cycleSetting(int item, int direction) {
    if (!m_settingsManager) return;
    StarReaderSettings* s = m_settingsManager->getSettings();

    switch (item) {
        case SETTING_FONT_SIZE:
            s->fontSize = (s->fontSize + direction + 4) % 4;
            break;
        case SETTING_LINE_SPACING:
            s->lineSpacing = (s->lineSpacing + direction + 3) % 3;
            break;
        case SETTING_MARGINS:
            s->margins = (s->margins + direction * 5 + 40) % 40;
            break;
        case SETTING_REFRESH_MODE:
            s->refreshMode = (s->refreshMode + direction + 3) % 3;
            break;
        case SETTING_ORIENTATION:
            s->orientation = (s->orientation + direction + 4) % 4;
            m_display->setOrientation((HalDisplay::Orientation)s->orientation);
            break;
        case SETTING_AUTO_REFRESH:
            s->autoRefreshPages += direction * 10;
            if (s->autoRefreshPages < 10) s->autoRefreshPages = 10;
            if (s->autoRefreshPages > 200) s->autoRefreshPages = 200;
            break;
    }

    requestUpdate();
}

const char* DisplaySettingsActivity::getSettingValueText(int item) {
    if (!m_settingsManager) return "";
    StarReaderSettings* s = m_settingsManager->getSettings();
    static char buf[32];

    switch (item) {
        case SETTING_FONT_SIZE:
            switch (s->fontSize) {
                case 0: return "小";
                case 1: return "中";
                case 2: return "大";
                case 3: return "特大";
                default: return "?";
            }
        case SETTING_LINE_SPACING:
            switch (s->lineSpacing) {
                case 0: return "紧凑";
                case 1: return "标准";
                case 2: return "宽松";
                default: return "?";
            }
        case SETTING_MARGINS:
            snprintf(buf, sizeof(buf), "%d px", s->margins);
            return buf;
        case SETTING_REFRESH_MODE:
            switch (s->refreshMode) {
                case 0: return "全刷";
                case 1: return "局刷";
                case 2: return "快刷";
                default: return "?";
            }
        case SETTING_ORIENTATION:
            switch (s->orientation) {
                case 0: return "竖屏";
                case 1: return "横屏";
                case 2: return "反向竖屏";
                case 3: return "反向横屏";
                default: return "?";
            }
        case SETTING_AUTO_REFRESH:
            snprintf(buf, sizeof(buf), "每 %d 页", s->autoRefreshPages);
            return buf;
        default:
            return "";
    }
}

void DisplaySettingsActivity::applySettings() {
    if (m_settingsManager) {
        m_settingsManager->save();
    }
}
