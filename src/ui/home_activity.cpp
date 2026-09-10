/**
 * StarReader Pro Firmware - Home Activity Implementation
 * 
 * 主页：最近在读卡片 + 2x2网格四大入口
 */

#include "home_activity.h"
#include "../config.h"
#include "txt_reader_activity.h"
#include "settings_activity.h"
#include "library_activity.h"
#include "wifi_activity.h"
#include "../utils/i18n.h"
#include <string.h>
#include <cstdio>

HomeActivity::HomeActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power,
                           HalTouch* touch, HalFrontLight* frontLight,
                           SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedIndex(0) {
}

HomeActivity::~HomeActivity() {
}

void HomeActivity::onEnter() {
    m_selectedIndex = 0;
    requestUpdate();
}

void HomeActivity::onExit() {
}

void HomeActivity::onResume() {
    requestUpdate();
}

void HomeActivity::loop() {
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

void HomeActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    drawStatusBar();
    drawContinueCard();
    drawGridMenu();
    drawBottomHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void HomeActivity::drawStatusBar() {
    int16_t width = m_display->getRotatedWidth();

    // 状态栏黑底
    m_display->fillRect(0, 0, width, STATUS_BAR_HEIGHT, 0x00);

    // 电量（左）
    uint8_t batteryPct = m_power->getBatteryPercentage();
    char batteryText[16];
    snprintf(batteryText, sizeof(batteryText), "%d%%", batteryPct);
    m_display->drawString(10, 7, batteryText, 0xFF, 1);

    // 前光状态（中偏左）
    if (m_frontLight && m_frontLight->isOn()) {
        char lightText[32];
        snprintf(lightText, sizeof(lightText), "前光 %d%%", m_frontLight->getBrightness());
        m_display->drawString(60, 7, lightText, 0xFF, 1);
    }

    // 充电状态（右）
    if (m_power->isUsbConnected()) {
        m_display->drawString(width - 50, 7, "充电", 0xFF, 1);
    }
}

void HomeActivity::drawContinueCard() {
    int16_t width = m_display->getRotatedWidth();
    int cardX = CARD_PADDING;
    int cardY = STATUS_BAR_HEIGHT + CARD_PADDING;
    int cardW = width - 2 * CARD_PADDING;
    int cardH = CONTINUE_CARD_HEIGHT;

    // 卡片边框
    m_display->drawRect(cardX, cardY, cardW, cardH, 0x00);

    // 选中态：反白
    bool selected = (m_selectedIndex == MENU_CONTINUE);
    if (selected) {
        m_display->fillRect(cardX + 1, cardY + 1, cardW - 2, cardH - 2, 0x00);
    }

    uint8_t textColor = selected ? 0xFF : 0x00;
    uint8_t subColor = selected ? 0xCC : 0x66;

    // 封面占位（左侧方块）
    int coverX = cardX + 15;
    int coverY = cardY + 15;
    int coverW = 70;
    int coverH = 80;
    if (selected) {
        m_display->drawRect(coverX, coverY, coverW, coverH, 0xFF);
        m_display->drawString(coverX + 18, coverY + 30, "书", 0xFF, 2);
    } else {
        m_display->drawRect(coverX, coverY, coverW, coverH, 0x00);
        m_display->drawString(coverX + 18, coverY + 30, "书", 0x00, 2);
    }

    // 书名
    const char* title = "继续阅读";
    m_display->drawString(cardX + 100, cardY + 25, title, textColor, 2);

    // 进度信息
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        char progressText[64];
        snprintf(progressText, sizeof(progressText), "第 %d 页", s->lastBookPage);
        m_display->drawString(cardX + 100, cardY + 55, progressText, subColor, 1);
    }
}

void HomeActivity::drawGridMenu() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // 网格起始位置
    int gridTop = STATUS_BAR_HEIGHT + CARD_PADDING + CONTINUE_CARD_HEIGHT + CARD_PADDING + 10;
    int gridBottom = height - 30;
    int gridH = gridBottom - gridTop;

    int cellW = (width - 2 * CARD_PADDING - GRID_GAP) / GRID_COLS;
    int cellH = (gridH - GRID_GAP) / GRID_ROWS;

    // 菜单项
    const char* labels[] = {
        "书架",
        "文件",
        "应用",
        "设置"
    };
    const char* icons[] = {
        "[书]",
        "[文]",
        "[应]",
        "[设]"
    };

    // 索引映射：MENU_LIBRARY=1, MENU_FILES=2, MENU_APPS=3, MENU_SETTINGS=4
    int menuIndices[] = {
        MENU_LIBRARY,
        MENU_FILES,
        MENU_APPS,
        MENU_SETTINGS
    };

    for (int i = 0; i < 4; i++) {
        int row = i / GRID_COLS;
        int col = i % GRID_COLS;
        int x = CARD_PADDING + col * (cellW + GRID_GAP);
        int y = gridTop + row * (cellH + GRID_GAP);

        bool selected = (m_selectedIndex == menuIndices[i]);

        if (selected) {
            m_display->fillRect(x, y, cellW, cellH, 0x00);
            m_display->drawRect(x, y, cellW, cellH, 0x00);
            m_display->drawString(x + cellW/2 - 20, y + 25, icons[i], 0xFF, 2);
            m_display->drawString(x + cellW/2 - 15, y + 60, labels[i], 0xFF, 2);
        } else {
            m_display->drawRect(x, y, cellW, cellH, 0x00);
            m_display->drawString(x + cellW/2 - 20, y + 25, icons[i], 0x00, 2);
            m_display->drawString(x + cellW/2 - 15, y + 60, labels[i], 0x00, 2);
        }
    }
}

void HomeActivity::drawBottomHint() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint = "触屏: 点选  按键: 上下移动  确认: 进入  返回: 休眠";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x60, 1);
}

void HomeActivity::handleButton(ButtonState event) {
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
            if (m_selectedIndex < MENU_COUNT - 1) {
                m_selectedIndex++;
                requestUpdate();
            }
            break;

        case BTN_CONFIRM:
            launchMenuItem(m_selectedIndex);
            break;

        case BTN_BACK:
            if (m_frontLight) m_frontLight->off();
            m_power->enterDeepSleep();
            break;

        default:
            break;
    }
}

void HomeActivity::handleTouch(TouchEvent event) {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    switch (event.gesture) {
        case GESTURE_TAP: {
            int x = event.endPoint.x;
            int y = event.endPoint.y;

            // 最近在读卡片区域
            int cardY = STATUS_BAR_HEIGHT + CARD_PADDING;
            int cardH = CONTINUE_CARD_HEIGHT;
            if (y >= cardY && y < cardY + cardH) {
                m_selectedIndex = MENU_CONTINUE;
                launchMenuItem(MENU_CONTINUE);
                break;
            }

            // 网格区域
            int gridTop = STATUS_BAR_HEIGHT + CARD_PADDING + CONTINUE_CARD_HEIGHT + CARD_PADDING + 10;
            int gridBottom = height - 30;
            int gridH = gridBottom - gridTop;
            int cellW = (width - 2 * CARD_PADDING - GRID_GAP) / GRID_COLS;
            int cellH = (gridH - GRID_GAP) / GRID_ROWS;

            for (int row = 0; row < GRID_ROWS; row++) {
                for (int col = 0; col < GRID_COLS; col++) {
                    int cellX = CARD_PADDING + col * (cellW + GRID_GAP);
                    int cellY = gridTop + row * (cellH + GRID_GAP);
                    if (x >= cellX && x < cellX + cellW && y >= cellY && y < cellY + cellH) {
                        int idx = row * GRID_COLS + col;
                        int menuIndices[] = {MENU_LIBRARY, MENU_FILES, MENU_APPS, MENU_SETTINGS};
                        m_selectedIndex = menuIndices[idx];
                        launchMenuItem(m_selectedIndex);
                        break;
                    }
                }
            }
            break;
        }

        case GESTURE_SWIPE_RIGHT:
            if (m_manager) m_manager->goBack();
            break;

        case GESTURE_LONG_PRESS:
            if (m_frontLight) {
                m_frontLight->toggle();
                requestUpdate();
            }
            break;

        default:
            break;
    }
}

void HomeActivity::launchMenuItem(int item) {
    if (!m_manager) return;

    switch (item) {
        case MENU_CONTINUE: {
            const char* filePath = "/books/sample.txt";
            if (m_settingsManager) {
                StarReaderSettings* s = m_settingsManager->getSettings();
                if (s->lastBookPath[0] != '\0') {
                    filePath = s->lastBookPath;
                }
            }
            TxtReaderActivity* reader = new TxtReaderActivity(
                m_display, m_input, m_storage, m_power,
                filePath, m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(reader);
            break;
        }

        case MENU_LIBRARY: {
            LibraryActivity* library = new LibraryActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(library);
            break;
        }

        case MENU_FILES: {
            // 文件浏览器 - 暂时用Library代替
            LibraryActivity* files = new LibraryActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(files);
            break;
        }

        case MENU_APPS: {
            // Apps Hub - 暂时跳WiFi设置占位
            WifiActivity* wifi = new WifiActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(wifi);
            break;
        }

        case MENU_SETTINGS: {
            SettingsActivity* settings = new SettingsActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(settings);
            break;
        }
    }
}
