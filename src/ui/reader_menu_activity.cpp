/**
 * StarReader Pro Firmware - Reader Menu Implementation
 * 
 * 阅读器底部弹出菜单实现
 */

#include "reader_menu_activity.h"
#include "../config.h"
#include <string.h>
#include <cstdio>

ReaderMenuActivity::ReaderMenuActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                       HalPowerManager* power, HalTouch* touch,
                                       HalFrontLight* frontLight,
                                       SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedItem(0) {
}

ReaderMenuActivity::~ReaderMenuActivity() {
}

void ReaderMenuActivity::onEnter() {
    m_selectedItem = 0;
    requestUpdate();
}

void ReaderMenuActivity::onExit() {
}

void ReaderMenuActivity::onResume() {
    requestUpdate();
}

void ReaderMenuActivity::loop() {
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

void ReaderMenuActivity::render() {
    if (!needsRender()) return;

    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // 半透明遮罩（上半部分）
    m_display->fillRect(0, 0, width, height - MENU_HEIGHT, 0x80);

    // 菜单背景（白底）
    m_display->fillRect(0, height - MENU_HEIGHT, width, MENU_HEIGHT, 0xFF);

    // 菜单顶部边框
    m_display->drawRect(0, height - MENU_HEIGHT, width, MENU_HEIGHT, 0x00);

    drawMenuItems();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void ReaderMenuActivity::drawMenuBackground() {
    // 已在render中处理
}

void ReaderMenuActivity::drawMenuItems() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* menuItems[] = {
        "目录",
        "书签",
        "阅读设置",
        "夜间模式",
        "返回主页"
    };

    int startY = height - MENU_HEIGHT + 10;

    for (int i = 0; i < MENU_COUNT; i++) {
        int y = startY + i * MENU_ITEM_HEIGHT;

        bool selected = (i == m_selectedItem);

        if (selected) {
            m_display->fillRect(10, y, width - 20, MENU_ITEM_HEIGHT - 4, 0x00);
            m_display->drawString(25, y + 12, menuItems[i], 0xFF, 2);
        } else {
            m_display->drawString(25, y + 12, menuItems[i], 0x00, 2);
        }
    }
}

void ReaderMenuActivity::handleButton(ButtonState event) {
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
            if (m_selectedItem < MENU_COUNT - 1) {
                m_selectedItem++;
                requestUpdate();
            }
            break;

        case BTN_CONFIRM:
            selectItem(m_selectedItem);
            break;

        case BTN_BACK:
            // 关闭菜单
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void ReaderMenuActivity::handleTouch(TouchEvent event) {
    int16_t height = m_display->getRotatedHeight();

    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;

            // 点击菜单外区域关闭
            if (y < height - MENU_HEIGHT) {
                if (m_manager) m_manager->goBack();
                break;
            }

            // 点击菜单项
            int startY = height - MENU_HEIGHT + 10;
            for (int i = 0; i < MENU_COUNT; i++) {
                int itemY = startY + i * MENU_ITEM_HEIGHT;
                if (y >= itemY && y < itemY + MENU_ITEM_HEIGHT - 4) {
                    m_selectedItem = i;
                    selectItem(i);
                    break;
                }
            }
            break;
        }

        case GESTURE_SWIPE_UP:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void ReaderMenuActivity::selectItem(int item) {
    switch (item) {
        case MENU_TOC:
            // 打开目录
            break;
        case MENU_BOOKMARK:
            // 打开书签
            break;
        case MENU_SETTINGS:
            // 打开阅读设置
            break;
        case MENU_NIGHT_MODE:
            // 切换夜间模式
            break;
        case MENU_RETURN_HOME:
            // 返回主页
            break;
    }
    // 选择后关闭菜单
    if (m_manager) m_manager->goBack();
}
