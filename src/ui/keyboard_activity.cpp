/**
 * StarReader Pro Firmware - Keyboard Implementation
 * 
 * 软键盘输入界面实现
 */

#include "keyboard_activity.h"
#include "../config.h"
#include <string.h>
#include <cstdio>

KeyboardActivity::KeyboardActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                   HalPowerManager* power, HalTouch* touch,
                                   HalFrontLight* frontLight,
                                   SettingsManager* settingsManager,
                                   const char* title, char* resultBuffer, int maxLen)
    : Activity(display, input, storage, power)
    , m_title(title)
    , m_resultBuffer(resultBuffer)
    , m_maxLen(maxLen)
    , m_inputLen(0)
    , m_selectedRow(1)
    , m_selectedCol(0)
    , m_currentMode(MODE_QWERTY)
    , m_passwordMode(true)
    , m_completed(false) {
}

KeyboardActivity::~KeyboardActivity() {
}

void KeyboardActivity::onEnter() {
    m_inputLen = 0;
    m_selectedRow = 1;
    m_selectedCol = 0;
    m_completed = false;
    if (m_resultBuffer) {
        m_resultBuffer[0] = '\0';
    }
    requestUpdate();
}

void KeyboardActivity::onExit() {
    if (m_resultBuffer) {
        m_resultBuffer[m_inputLen] = '\0';
    }
}

void KeyboardActivity::onResume() {
    requestUpdate();
}

void KeyboardActivity::loop() {
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

void KeyboardActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    drawTitleBar();
    drawInputBar();
    drawKeyboard();
    drawBottomHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void KeyboardActivity::drawTitleBar() {
    int16_t width = m_display->getRotatedWidth();

    m_display->fillRect(0, 0, width, STATUS_BAR_HEIGHT, 0x00);

    m_display->drawString(10, 6, "< 返回", 0xFF, 1);

    int16_t titleWidth = m_display->getStringWidth(m_title, 2);
    m_display->drawString((width - titleWidth) / 2, 4, m_title, 0xFF, 2);
}

void KeyboardActivity::drawInputBar() {
    int16_t width = m_display->getRotatedWidth();
    int y = STATUS_BAR_HEIGHT + PADDING;

    // 输入框背景
    m_display->drawRect(PADDING, y, width - 2 * PADDING, INPUT_BAR_HEIGHT, 0x00);

    // 显示输入内容（密码模式显示星号）
    if (m_inputLen > 0) {
        char displayText[64];
        if (m_passwordMode) {
            for (int i = 0; i < m_inputLen && i < 30; i++) {
                displayText[i] = '*';
            }
            displayText[m_inputLen] = '\0';
        } else {
            strncpy(displayText, m_resultBuffer, m_inputLen);
            displayText[m_inputLen] = '\0';
        }
        m_display->drawString(PADDING + 10, y + 15, displayText, 0x00, 2);
    } else {
        const char* hint = "请输入密码...";
        m_display->drawString(PADDING + 10, y + 15, hint, 0x80, 2);
    }

    // 光标
    int cursorX = PADDING + 10 + m_inputLen * 8;
    m_display->drawRect(cursorX, y + 10, 2, 20, 0x00);
}

void KeyboardActivity::drawKeyboard() {
    int16_t width = m_display->getRotatedWidth();
    int startY = STATUS_BAR_HEIGHT + PADDING + INPUT_BAR_HEIGHT + PADDING;

    const char* rows[4];
    int rowCount;

    if (m_currentMode == MODE_QWERTY) {
        memcpy(rows, m_qwertyRows, sizeof(m_qwertyRows));
        rowCount = 4;
    } else {
        memcpy(rows, m_numericRows, sizeof(m_numericRows));
        rowCount = 3;
    }

    for (int row = 0; row < rowCount; row++) {
        int rowLen = strlen(rows[row]);
        int keyW = (width - 2 * PADDING - (rowLen - 1) * KEY_GAP) / rowLen;
        int rowY = startY + row * (KEY_HEIGHT + KEY_GAP);

        for (int col = 0; col < rowLen; col++) {
            int keyX = PADDING + col * (keyW + KEY_GAP);
            bool selected = (row == m_selectedRow && col == m_selectedCol);

            if (selected) {
                m_display->fillRect(keyX, rowY, keyW, KEY_HEIGHT, 0x00);
                char keyChar[2] = {rows[row][col], '\0'};
                int charW = m_display->getStringWidth(keyChar, 2);
                m_display->drawString(keyX + (keyW - charW) / 2, rowY + 10, keyChar, 0xFF, 2);
            } else {
                m_display->drawRect(keyX, rowY, keyW, KEY_HEIGHT, 0x00);
                char keyChar[2] = {rows[row][col], '\0'};
                int charW = m_display->getStringWidth(keyChar, 1);
                m_display->drawString(keyX + (keyW - charW) / 2, rowY + 12, keyChar, 0x00, 1);
            }
        }
    }

    // 底部功能键行
    int funcY = startY + rowCount * (KEY_HEIGHT + KEY_GAP);
    int funcKeyW = (width - 2 * PADDING - 2 * KEY_GAP) / 3;

    // 模式切换
    m_display->drawRect(PADDING, funcY, funcKeyW, KEY_HEIGHT, 0x00);
    const char* modeText = m_currentMode == MODE_QWERTY ? "ABC" : "123";
    m_display->drawString(PADDING + 10, funcY + 12, modeText, 0x00, 1);

    // 删除
    m_display->drawRect(PADDING + funcKeyW + KEY_GAP, funcY, funcKeyW, KEY_HEIGHT, 0x00);
    m_display->drawString(PADDING + funcKeyW + KEY_GAP + 10, funcY + 12, "删除", 0x00, 1);

    // 确认
    m_display->drawRect(PADDING + 2 * (funcKeyW + KEY_GAP), funcY, funcKeyW, KEY_HEIGHT, 0x00);
    m_display->drawString(PADDING + 2 * (funcKeyW + KEY_GAP) + 10, funcY + 12, "确认", 0x00, 1);
}

void KeyboardActivity::drawBottomHint() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint = "方向键: 移动  确认: 输入  返回: 取消";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x60, 1);
}

void KeyboardActivity::handleButton(ButtonState event) {
    const char* rows[4];
    int rowCount;

    if (m_currentMode == MODE_QWERTY) {
        memcpy(rows, m_qwertyRows, sizeof(m_qwertyRows));
        rowCount = 4;
    } else {
        memcpy(rows, m_numericRows, sizeof(m_numericRows));
        rowCount = 3;
    }

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
            if (m_selectedCol < (int)strlen(rows[m_selectedRow]) - 1) {
                m_selectedCol++;
                requestUpdate();
            }
            break;

        case BTN_CONFIRM:
            handleKeyPress(m_selectedRow, m_selectedCol);
            break;

        case BTN_BACK:
            if (m_manager) m_manager->goBack();
            break;

        default:
            break;
    }
}

void KeyboardActivity::handleTouch(TouchEvent event) {
    int16_t width = m_display->getRotatedWidth();
    int startY = STATUS_BAR_HEIGHT + PADDING + INPUT_BAR_HEIGHT + PADDING;

    const char* rows[4];
    int rowCount;

    if (m_currentMode == MODE_QWERTY) {
        memcpy(rows, m_qwertyRows, sizeof(m_qwertyRows));
        rowCount = 4;
    } else {
        memcpy(rows, m_numericRows, sizeof(m_numericRows));
        rowCount = 3;
    }

    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;
            int x = event.endPoint.x;

            // 检查按键
            for (int row = 0; row < rowCount; row++) {
                int rowLen = strlen(rows[row]);
                int keyW = (width - 2 * PADDING - (rowLen - 1) * KEY_GAP) / rowLen;
                int rowY = startY + row * (KEY_HEIGHT + KEY_GAP);

                if (y >= rowY && y < rowY + KEY_HEIGHT) {
                    for (int col = 0; col < rowLen; col++) {
                        int keyX = PADDING + col * (keyW + KEY_GAP);
                        if (x >= keyX && x < keyX + keyW) {
                            m_selectedRow = row;
                            m_selectedCol = col;
                            handleKeyPress(row, col);
                            return;
                        }
                    }
                }
            }

            // 功能键行
            int funcY = startY + rowCount * (KEY_HEIGHT + KEY_GAP);
            int funcKeyW = (width - 2 * PADDING - 2 * KEY_GAP) / 3;

            if (y >= funcY && y < funcY + KEY_HEIGHT) {
                if (x < PADDING + funcKeyW) {
                    toggleMode();
                } else if (x < PADDING + 2 * (funcKeyW + KEY_GAP)) {
                    handleBackspace();
                } else {
                    handleEnter();
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

void KeyboardActivity::handleKeyPress(int row, int col) {
    const char* rows[4];
    int rowCount;

    if (m_currentMode == MODE_QWERTY) {
        memcpy(rows, m_qwertyRows, sizeof(m_qwertyRows));
        rowCount = 4;
    } else {
        memcpy(rows, m_numericRows, sizeof(m_numericRows));
        rowCount = 3;
    }

    if (row < 0 || row >= rowCount) return;
    if (col < 0 || col >= (int)strlen(rows[row])) return;

    if (m_inputLen < m_maxLen - 1 && m_resultBuffer) {
        m_resultBuffer[m_inputLen] = rows[row][col];
        m_inputLen++;
        m_resultBuffer[m_inputLen] = '\0';
    }

    requestUpdate();
}

void KeyboardActivity::handleBackspace() {
    if (m_inputLen > 0) {
        m_inputLen--;
        if (m_resultBuffer) {
            m_resultBuffer[m_inputLen] = '\0';
        }
    }
    requestUpdate();
}

void KeyboardActivity::handleEnter() {
    m_completed = true;
    if (m_manager) m_manager->goBack();
}

void KeyboardActivity::toggleMode() {
    m_currentMode = (m_currentMode + 1) % MODE_COUNT;
    m_selectedRow = 0;
    m_selectedCol = 0;
    requestUpdate();
}

int KeyboardActivity::getKeyWidth(const char* row) {
    int16_t width = m_display->getRotatedWidth();
    int rowLen = strlen(row);
    return (width - 2 * PADDING - (rowLen - 1) * KEY_GAP) / rowLen;
}

int KeyboardActivity::getRowsCount() {
    return (m_currentMode == MODE_QWERTY) ? 4 : 3;
}
