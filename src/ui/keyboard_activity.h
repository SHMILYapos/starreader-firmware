/**
 * StarReader Pro Firmware - Keyboard Activity
 * 
 * 软键盘输入界面，用于WiFi密码等文本输入
 * Software keyboard for text input (WiFi password, etc.)
 */

#ifndef STARREADER_UI_KEYBOARD_ACTIVITY_H
#define STARREADER_UI_KEYBOARD_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class KeyboardActivity : public Activity {
public:
    KeyboardActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                    HalPowerManager* power, HalTouch* touch = nullptr,
                    HalFrontLight* frontLight = nullptr,
                    SettingsManager* settingsManager = nullptr,
                    const char* title = "输入", char* resultBuffer = nullptr, int maxLen = 32);
    ~KeyboardActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    enum KeyboardMode {
        MODE_QWERTY = 0,
        MODE_NUMERIC,
        MODE_SYMBOLS,
        MODE_COUNT
    };

    const char* m_title;
    char* m_resultBuffer;
    int m_maxLen;
    int m_inputLen;
    
    int m_selectedRow;
    int m_selectedCol;
    int m_currentMode;
    
    bool m_passwordMode;
    bool m_completed;

    static const int STATUS_BAR_HEIGHT = 28;
    static const int INPUT_BAR_HEIGHT = 50;
    static const int KEY_HEIGHT = 40;
    static const int KEY_GAP = 2;
    static const int PADDING = 10;

    // 键盘布局
    const char* m_qwertyRows[4] = {
        "1234567890",
        "qwertyuiop",
        "asdfghjkl",
        "zxcvbnm"
    };
    
    const char* m_numericRows[3] = {
        "123",
        "456",
        "789"
    };

    void drawTitleBar();
    void drawInputBar();
    void drawKeyboard();
    void drawBottomHint();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void handleKeyPress(int row, int col);
    void handleBackspace();
    void handleEnter();
    void toggleMode();
    int getKeyWidth(const char* row);
    int getRowsCount();
};

#endif // STARREADER_UI_KEYBOARD_ACTIVITY_H
