/**
 * StarReader Pro Firmware - Settings Activity Implementation
 * 
 * 设置主界面：分组列表
 */

#include "settings_activity.h"
#include "../config.h"
#include "wifi_activity.h"
#include "about_activity.h"
#include "display_settings_activity.h"
#include "frontlight_settings_activity.h"
#include "reader_settings_activity.h"
#include <string.h>
#include <cstdio>

SettingsActivity::SettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                   HalPowerManager* power, HalTouch* touch,
                                   HalFrontLight* frontLight,
                                   SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedIndex(0)
    , m_scrollOffset(0) {
}

SettingsActivity::~SettingsActivity() {
}

void SettingsActivity::onEnter() {
    m_selectedIndex = 0;
    m_scrollOffset = 0;
    requestUpdate();
}

void SettingsActivity::onExit() {
}

void SettingsActivity::onResume() {
    requestUpdate();
}

void SettingsActivity::loop() {
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

void SettingsActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    drawTitleBar();
    drawSettingsList();
    drawBottomHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void SettingsActivity::drawTitleBar() {
    int16_t width = m_display->getRotatedWidth();

    m_display->fillRect(0, 0, width, STATUS_BAR_HEIGHT, 0x00);

    // 返回箭头
    m_display->drawString(10, 6, "< 返回", 0xFF, 1);

    // 标题
    const char* title = "设置";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 4, title, 0xFF, 2);
}

void SettingsActivity::drawSettingsList() {
    int16_t width = m_display->getRotatedWidth();

    const char* groupNames[] = {
        "显示",
        "阅读",
        "前光",
        "网络",
        "系统"
    };

    const char* groupSubtitles[] = {
        "字体、刷新、方向",
        "字号、行距、排版",
        "亮度、色温、预设",
        "WiFi、传书、OTA",
        "语言、关于、升级"
    };

    int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

    for (int i = 0; i < GROUP_COUNT; i++) {
        int y = startY + i * LIST_ITEM_HEIGHT;

        bool selected = (i == m_selectedIndex);

        if (selected) {
            m_display->fillRect(LIST_PADDING, y, width - 2 * LIST_PADDING, LIST_ITEM_HEIGHT - 5, 0x00);
            m_display->drawString(LIST_PADDING + 15, y + 10, groupNames[i], 0xFF, 2);
            m_display->drawString(LIST_PADDING + 15, y + 30, groupSubtitles[i], 0xCC, 1);
            m_display->drawString(width - 40, y + 18, ">", 0xFF, 2);
        } else {
            m_display->drawString(LIST_PADDING + 15, y + 10, groupNames[i], 0x00, 2);
            m_display->drawString(LIST_PADDING + 15, y + 30, groupSubtitles[i], 0x60, 1);
            m_display->drawString(width - 40, y + 18, ">", 0x00, 2);
        }
    }
}

void SettingsActivity::drawBottomHint() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint = "上下键: 选择  确认: 进入  返回: 返回主页";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x60, 1);
}

void SettingsActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_VOL_UP:
        case BTN_LEFT:
            if (m_selectedIndex > 0) {
                m_selectedIndex--;
                requestUpdate();
            }
            break;

        case BTN_VOL_DOWN:
        case BTN_RIGHT:
            if (m_selectedIndex < GROUP_COUNT - 1) {
                m_selectedIndex++;
                requestUpdate();
            }
            break;

        case BTN_CONFIRM:
            launchGroup(m_selectedIndex);
            break;

        case BTN_BACK:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void SettingsActivity::handleTouch(TouchEvent event) {
    int16_t width = m_display->getRotatedWidth();

    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;
            int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

            for (int i = 0; i < GROUP_COUNT; i++) {
                int itemY = startY + i * LIST_ITEM_HEIGHT;
                if (y >= itemY && y < itemY + LIST_ITEM_HEIGHT - 5) {
                    m_selectedIndex = i;
                    launchGroup(i);
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

void SettingsActivity::launchGroup(int group) {
    if (!m_manager) return;

    switch (group) {
        case GROUP_DISPLAY: {
            // 显示设置子页
            DisplaySettingsActivity* displaySettings = new DisplaySettingsActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(displaySettings);
            break;
        }

        case GROUP_READER: {
            // 阅读设置子页
            ReaderSettingsActivity* readerSettings = new ReaderSettingsActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(readerSettings);
            break;
        }

        case GROUP_FRONTLIGHT: {
            // 前光设置子页
            FrontLightSettingsActivity* flSettings = new FrontLightSettingsActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(flSettings);
            break;
        }

        case GROUP_NETWORK: {
            WifiActivity* wifi = new WifiActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(wifi);
            break;
        }

        case GROUP_SYSTEM: {
            // 系统设置 - 关于页面占位
            AboutActivity* about = new AboutActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(about);
            break;
        }

        default:
            requestUpdate();
            break;
    }
}
