/**
 * StarReader Firmware - TXT Reader Activity Implementation
 */

#include "txt_reader_activity.h"
#include "../config.h"
#include <string.h>

TxtReaderActivity::TxtReaderActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power,
                                     const char* filePath)
    : Activity(display, input, storage, power)
    , m_fileSize(0)
    , m_currentOffset(0)
    , m_currentPage(0)
    , m_totalPages(0)
    , m_pageBufferLen(0)
    , m_needsRender(true)
    , m_fileLoaded(false) {
    strncpy(m_filePath, filePath, sizeof(m_filePath) - 1);
    m_filePath[sizeof(m_filePath) - 1] = '\0';
}

TxtReaderActivity::~TxtReaderActivity() {
}

void TxtReaderActivity::onEnter() {
    if (!m_fileLoaded) {
        loadFile();
    }
    m_needsRender = true;
}

void TxtReaderActivity::onExit() {
    // Save reading position
    // TODO: Save to settings
}

void TxtReaderActivity::onResume() {
    m_needsRender = true;
}

void TxtReaderActivity::loop() {
    m_input->update();
    ButtonState event = m_input->getLastEvent();

    if (event.event == EVENT_PRESSED) {
        handleButton(event);
    }
}

void TxtReaderActivity::render() {
    if (!m_needsRender) return;

    m_display->clear(0xFF);

    if (!m_fileLoaded) {
        const char* msg = "Failed to load file";
        int16_t width = m_display->getRotatedWidth();
        int16_t height = m_display->getRotatedHeight();
        int16_t msgWidth = m_display->getStringWidth(msg, 2);
        m_display->drawString((width - msgWidth) / 2, height / 2, msg, 0x00, 2);
    } else {
        renderPage();
    }

    m_display->refresh(HalDisplay::FULL_REFRESH);
    m_needsRender = false;
}

bool TxtReaderActivity::loadFile() {
    if (!m_storage->fileExists(m_filePath)) {
        m_fileLoaded = false;
        return false;
    }

    m_fileSize = m_storage->getFileSize(m_filePath);
    m_currentOffset = 0;
    m_currentPage = 0;

    calculatePages();
    m_fileLoaded = true;
    return true;
}

void TxtReaderActivity::calculatePages() {
    // Estimate characters per page
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    int charsPerLine = (width - 2 * PAGE_MARGIN) / (6 * FONT_SIZE);  // ~6px per char
    int linesPerPage = (height - 2 * PAGE_MARGIN - 30) / LINE_HEIGHT;  // 30px for status bar
    int charsPerPage = charsPerLine * linesPerPage;

    m_totalPages = m_fileSize / charsPerPage;
    if (m_fileSize % charsPerPage > 0) m_totalPages++;
}

void TxtReaderActivity::renderPage() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // Status bar
    m_display->fillRect(0, 0, width, 25, 0x00);
    char statusText[64];
    snprintf(statusText, sizeof(statusText), "Page %d / %d", m_currentPage + 1, m_totalPages);
    m_display->drawString(10, 6, statusText, 0xFF, 1);

    // Battery
    uint8_t batteryPct = m_power->getBatteryPercentage();
    char batText[16];
    snprintf(batText, sizeof(batText), "%d%%", batteryPct);
    m_display->drawString(width - 40, 6, batText, 0xFF, 1);

    // Read page content from file
    size_t bytesRead = 0;
    // Note: In a real implementation, we'd read from the file at m_currentOffset
    // For now, use a placeholder
    memset(m_pageBuffer, 0, sizeof(m_pageBuffer));

    // Wrap text
    int wrappedLen = wrapText(m_pageBuffer, width - 2 * PAGE_MARGIN,
                             m_pageBuffer, sizeof(m_pageBuffer) - 1);

    // Render text
    int y = PAGE_MARGIN + 25;  // Below status bar
    int x = PAGE_MARGIN;
    int line = 0;

    for (int i = 0; i < wrappedLen && m_pageBuffer[i] != '\0'; i++) {
        if (m_pageBuffer[i] == '\n') {
            line++;
            y += LINE_HEIGHT;
            x = PAGE_MARGIN;
            continue;
        }

        m_display->drawChar(x, y, m_pageBuffer[i], 0x00, FONT_SIZE);
        x += 6 * FONT_SIZE;
    }

    // Footer hint
    const char* hint = "Left/Right: Page  Back: Exit";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 20, hint, 0x00, 1);
}

void TxtReaderActivity::nextPage() {
    if (m_currentPage < m_totalPages - 1) {
        m_currentPage++;
        // Calculate new offset
        int charsPerLine = (m_display->getRotatedWidth() - 2 * PAGE_MARGIN) / 6;
        int linesPerPage = (m_display->getRotatedHeight() - 2 * PAGE_MARGIN - 30) / LINE_HEIGHT;
        m_currentOffset = m_currentPage * charsPerLine * linesPerPage;
        m_needsRender = true;
    }
}

void TxtReaderActivity::prevPage() {
    if (m_currentPage > 0) {
        m_currentPage--;
        int charsPerLine = (m_display->getRotatedWidth() - 2 * PAGE_MARGIN) / 6;
        int linesPerPage = (m_display->getRotatedHeight() - 2 * PAGE_MARGIN - 30) / LINE_HEIGHT;
        m_currentOffset = m_currentPage * charsPerLine * linesPerPage;
        m_needsRender = true;
    }
}

void TxtReaderActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_RIGHT:
        case BTN_VOL_UP:
            nextPage();
            break;

        case BTN_LEFT:
        case BTN_VOL_DOWN:
            prevPage();
            break;

        case BTN_BACK:
        case BTN_CONFIRM:
            if (m_manager) {
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}

int TxtReaderActivity::wrapText(const char* text, int maxWidth, char* output, int maxOutputLen) {
    // Simple text wrapping implementation
    int outIdx = 0;
    int lineWidth = 0;
    int charWidth = 6 * FONT_SIZE;

    for (int i = 0; text[i] != '\0' && outIdx < maxOutputLen - 1; i++) {
        if (text[i] == '\n') {
            output[outIdx++] = '\n';
            lineWidth = 0;
            continue;
        }

        if (lineWidth + charWidth > maxWidth) {
            output[outIdx++] = '\n';
            lineWidth = 0;
        }

        output[outIdx++] = text[i];
        lineWidth += charWidth;
    }

    output[outIdx] = '\0';
    return outIdx;
}
