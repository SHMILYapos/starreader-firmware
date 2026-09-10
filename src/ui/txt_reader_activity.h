/**
 * StarReader Firmware - TXT Reader Activity
 * 
 * Plain text file reader with page navigation
 */

#ifndef STARREADER_UI_TXT_READER_ACTIVITY_H
#define STARREADER_UI_TXT_READER_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/storage.h"
#include "../hal/power.h"

class TxtReaderActivity : public Activity {
public:
    TxtReaderActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power,
                      const char* filePath);
    ~TxtReaderActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

    bool preventAutoSleep() override { return true; }

private:
    char m_filePath[128];

    // Reader state
    uint32_t m_fileSize;
    uint32_t m_currentOffset;  // Current position in file
    uint32_t m_currentPage;
    uint32_t m_totalPages;

    // Page layout
    static const int PAGE_MARGIN = 20;
    static const int LINE_HEIGHT = 16;
    static const int FONT_SIZE = 1;

    // Buffer for current page
    char m_pageBuffer[2048];
    int m_pageBufferLen;

    bool m_needsRender;
    bool m_fileLoaded;

    // Methods
    bool loadFile();
    void calculatePages();
    void renderPage();
    void nextPage();
    void prevPage();
    void handleButton(ButtonState event);

    // Text wrapping helpers
    int wrapText(const char* text, int maxWidth, char* output, int maxOutputLen);
};

#endif // STARREADER_UI_TXT_READER_ACTIVITY_H
