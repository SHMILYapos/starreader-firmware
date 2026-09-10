/**
 * StarReader Pro Firmware - TXT Reader Activity
 * 
 * Plain text file reader with page navigation
 * Supports touch gestures (X4 Pro)
 */

#ifndef STARREADER_UI_TXT_READER_ACTIVITY_H
#define STARREADER_UI_TXT_READER_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class TxtReaderActivity : public Activity {
public:
    TxtReaderActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power,
                      const char* filePath, HalTouch* touch = nullptr,
                      HalFrontLight* frontLight = nullptr,
                      SettingsManager* settingsManager = nullptr);
    ~TxtReaderActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

    bool preventAutoSleep() override { return true; }

private:
    char m_filePath[128];

    // Hardware references
    HalTouch* m_touch;
    HalFrontLight* m_frontLight;
    SettingsManager* m_settingsManager;

    // Reader state
    uint32_t m_fileSize;
    uint32_t m_currentOffset;  // Current position in file
    uint32_t m_currentPage;
    uint32_t m_totalPages;

    // Page layout settings
    uint8_t m_fontSizeIdx;
    uint8_t m_lineSpacing;
    uint8_t m_margin;
    uint8_t m_justification;

    // Buffer for current page
    char m_pageBuffer[4096];
    int m_pageBufferLen;

    bool m_needsRender;
    bool m_fileLoaded;
    bool m_showStatusBar;
    uint32_t m_statusBarHideTime;

    // Quick settings panel
    bool m_showQuickSettings;
    int m_quickSettingsItem;

    // Methods
    bool loadFile();
    void calculatePages();
    void renderPage();
    void renderStatusBar();
    void renderQuickSettings();
    void nextPage();
    void prevPage();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void applyReaderSettings();

    // Text wrapping helpers
    int getCharWidth(int fontIdx);
    int getLineHeight();
    int wrapText(const char* text, int maxWidth, char* output, int maxOutputLen);
};

#endif // STARREADER_UI_TXT_READER_ACTIVITY_H
