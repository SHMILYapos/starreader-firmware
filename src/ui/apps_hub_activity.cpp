/**
 * StarReader Pro Firmware - Apps Hub Implementation
 * 
 * 应用中心：图标宫格实现
 */

#include "apps_hub_activity.h"
#include "../config.h"
#include "wifi_activity.h"
#include "about_activity.h"
#include <string.h>
#include <cstdio>

AppsHubActivity::AppsHubActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                HalPowerManager* power, HalTouch* touch,
                                HalFrontLight* frontLight,
                                SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedRow(0)
    , m_selectedCol(0) {
}

AppsHubActivity::~AppsHubActivity() {
}

void AppsHubActivity::onEnter() {
    m_selectedRow = 0;
    m_selectedCol = 0;
    requestUpdate();
}

void AppsHubActivity::onExit() {
}

void AppsHubActivity::onResume() {
    requestUpdate();
}

void AppsHubActivity::loop() {
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

void AppsHubActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    drawTitleBar();
    drawAppGrid();
    drawBottomHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void AppsHubActivity::drawTitleBar() {
    int16_t width = m_display->getRotatedWidth();

    m_display->fillRect(0, 0, width, STATUS_BAR_HEIGHT, 0x00);

    // 返回箭头
    m_display->drawString(10, 6, "< 返回", 0xFF, 1);

    // 标题
    const char* title = "应用中心";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 4, title, 0xFF, 2);
}

void AppsHubActivity::drawAppGrid() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // 应用列表
    const char* appNames[] = {
        "阅读统计",
        "2048",
        "数独",
        "计算器",
        "时钟",
        "屏保",
        "WiFi",
        "关于"
    };

    const char* appIcons[] = {
        "[统]",
        "[20]",
        "[数]",
        "[算]",
        "[时]",
        "[屏]",
        "[网]",
        "[关]"
    };

    int gridTop = STATUS_BAR_HEIGHT + GRID_PADDING;
    int gridBottom = height - 30;
    int gridH = gridBottom - gridTop;

    int cellW = (width - 2 * GRID_PADDING - (GRID_COLS - 1) * GRID_GAP) / GRID_COLS;
    int cellH = (gridH - (GRID_ROWS - 1) * GRID_GAP) / GRID_ROWS;

    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            int idx = row * GRID_COLS + col;
            if (idx >= APP_COUNT) break;

            int x = GRID_PADDING + col * (cellW + GRID_GAP);
            int y = gridTop + row * (cellH + GRID_GAP);

            bool selected = (row == m_selectedRow && col == m_selectedCol);

            if (selected) {
                // 选中态：反白
                m_display->fillRect(x, y, cellW, cellH, 0x00);
                m_display->drawString(x + cellW/2 - 20, y + 15, appIcons[idx], 0xFF, 2);
                m_display->drawString(x + cellW/2 - 25, y + 50, appNames[idx], 0xFF, 1);
            } else {
                // 未选中：空心
                m_display->drawRect(x, y, cellW, cellH, 0x00);
                m_display->drawString(x + cellW/2 - 20, y + 15, appIcons[idx], 0x00, 2);
                m_display->drawString(x + cellW/2 - 25, y + 50, appNames[idx], 0x00, 1);
            }
        }
    }
}

void AppsHubActivity::drawBottomHint() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint = "方向键: 移动  确认: 打开  返回: 返回主页";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x60, 1);
}

void AppsHubActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_VOL_UP:
        case BTN_LEFT:
            if (m_selectedCol > 0) {
                m_selectedCol--;
                requestUpdate();
            }
            break;

        case BTN_VOL_DOWN:
        case BTN_RIGHT:
            if (m_selectedCol < GRID_COLS - 1 && getSelectedIndex() < APP_COUNT - 1) {
                m_selectedCol++;
                requestUpdate();
            }
            break;

        case BTN_CONFIRM:
            launchApp(getSelectedIndex());
            break;

        case BTN_BACK:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void AppsHubActivity::handleTouch(TouchEvent event) {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    switch (event.gesture) {
        case GESTURE_TAP: {
            int x = event.endPoint.x;
            int y = event.endPoint.y;

            int gridTop = STATUS_BAR_HEIGHT + GRID_PADDING;
            int gridBottom = height - 30;
            int gridH = gridBottom - gridTop;

            int cellW = (width - 2 * GRID_PADDING - (GRID_COLS - 1) * GRID_GAP) / GRID_COLS;
            int cellH = (gridH - (GRID_ROWS - 1) * GRID_GAP) / GRID_ROWS;

            for (int row = 0; row < GRID_ROWS; row++) {
                for (int col = 0; col < GRID_COLS; col++) {
                    int cellX = GRID_PADDING + col * (cellW + GRID_GAP);
                    int cellY = gridTop + row * (cellH + GRID_GAP);
                    if (x >= cellX && x < cellX + cellW && y >= cellY && y < cellY + cellH) {
                        int idx = row * GRID_COLS + col;
                        if (idx < APP_COUNT) {
                            m_selectedRow = row;
                            m_selectedCol = col;
                            launchApp(idx);
                        }
                        break;
                    }
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

void AppsHubActivity::launchApp(int app) {
    if (!m_manager) return;

    switch (app) {
        case APP_WIFI: {
            WifiActivity* wifi = new WifiActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(wifi);
            break;
        }

        case APP_ABOUT: {
            AboutActivity* about = new AboutActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(about);
            break;
        }

        default:
            // 其他应用待开发
            requestUpdate();
            break;
    }
}

int AppsHubActivity::getSelectedIndex() {
    return m_selectedRow * GRID_COLS + m_selectedCol;
}
