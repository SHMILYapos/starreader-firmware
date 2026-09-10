/**
 * StarReader Pro Firmware - TXT Reader Activity Implementation
 */

#include "txt_reader_activity.h"
#include "../config.h"
#include <string.h>

TxtReaderActivity::TxtReaderActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power,
                                     const char* filePath, HalTouch* touch,
                                     HalFrontLight* frontLight,
                                     SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_touch(touch)
    , m_frontLight(frontLight)
    , m_settingsManager(settingsManager)
    , m_fileSize(0)
    , m_currentOffset(0)
    , m_currentPage(0)
    , m_totalPages(0)
    , m_fontSizeIdx(1)
    , m_lineSpacing(1)
    , m_margin(20)
    , m_justification(1)
    , m_pageBufferLen(0)
    , m_needsRender(true)
    , m_fileLoaded(false)
    , m_showStatusBar(true)
    , m_statusBarHideTime(0)
    , m_showQuickSettings(false)
    , m_quickSettingsItem(0) {
    strncpy(m_filePath, filePath, sizeof(m_filePath) - 1);
    m_filePath[sizeof(m_filePath) - 1] = '\0';
}

TxtReaderActivity::~TxtReaderActivity() {
}

void TxtReaderActivity::onEnter() {
    // Load reader settings
    applyReaderSettings();

    if (!m_fileLoaded) {
        loadFile();
    }
    m_needsRender = true;
}

void TxtReaderActivity::onExit() {
    // Save reading position to settings
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        strncpy(s->lastBookPath, m_filePath, sizeof(s->lastBookPath) - 1);
        s->lastBookPosition = m_currentOffset;
        s->lastBookPage = m_currentPage;
        m_settingsManager->save();
    }
}

void TxtReaderActivity::onResume() {
    m_needsRender = true;
}

void TxtReaderActivity::loop() {
    // Handle physical buttons
    m_input->update();
    ButtonState event = m_input->getLastEvent();

    if (event.event == EVENT_PRESSED) {
        handleButton(event);
    }

    // Handle touch gestures
    if (m_touch) {
        TouchEvent touchEvent = m_touch->getLastEvent();
        if (touchEvent.gesture != GESTURE_NONE) {
            handleTouch(touchEvent);
        }
    }

    // Auto-hide status bar
    if (m_showStatusBar && !m_showQuickSettings) {
        if (millis() - m_statusBarHideTime > 3000) {
            m_showStatusBar = false;
            m_needsRender = true;
        }
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

        if (m_showStatusBar) {
            renderStatusBar();
        }

        if (m_showQuickSettings) {
            renderQuickSettings();
        }
    }

    // Use partial refresh for reading (faster)
    HalDisplay::RefreshMode mode = m_showQuickSettings ? HalDisplay::FULL_REFRESH : HalDisplay::PARTIAL_REFRESH;
    m_display->refresh(mode);
    m_needsRender = false;
}

void TxtReaderActivity::applyReaderSettings() {
    if (!m_settingsManager) return;

    StarReaderSettings* s = m_settingsManager->getSettings();
    m_fontSizeIdx = s->fontSize;
    m_lineSpacing = s->lineSpacing;
    m_margin = s->margins;
    m_justification = s->justification;
}

bool TxtReaderActivity::loadFile() {
    if (!m_storage->fileExists(m_filePath)) {
        m_fileLoaded = false;
        return false;
    }

    m_fileSize = m_storage->getFileSize(m_filePath);
    m_currentOffset = 0;
    m_currentPage = 0;

    // Try to restore last position
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        if (strstr(s->lastBookPath, m_filePath) != NULL && s->lastBookPage > 0) {
            m_currentPage = s->lastBookPage;
            m_currentOffset = s->lastBookPosition;
        }
    }

    calculatePages();
    m_fileLoaded = true;
    return true;
}

void TxtReaderActivity::calculatePages() {
    int16_t width = m_display->getRotatedWidth();

    int charsPerLine = (width - 2 * m_margin) / getCharWidth(m_fontSizeIdx);
    int linesPerPage = (m_display->getRotatedHeight() - 2 * m_margin - 30) / getLineHeight();
    int charsPerPage = charsPerLine * linesPerPage;

    if (charsPerPage > 0) {
        m_totalPages = m_fileSize / charsPerPage;
        if (m_fileSize % charsPerPage > 0) m_totalPages++;
    }
}

void TxtReaderActivity::renderPage() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // Read page content from file
    size_t bytesRead = 0;
    int charsPerLine = (width - 2 * m_margin) / getCharWidth(m_fontSizeIdx);
    int linesPerPage = (height - 2 * m_margin - 30) / getLineHeight();
    int charsToRead = charsPerLine * linesPerPage;

    if (charsToRead >= (int)sizeof(m_pageBuffer)) {
        charsToRead = sizeof(m_pageBuffer) - 1;
    }

    m_storage->readFile(m_filePath, m_currentOffset, (uint8_t*)m_pageBuffer,
                        charsToRead, &bytesRead);
    m_pageBufferLen = bytesRead;
    m_pageBuffer[bytesRead] = '\0';

    // Wrap text
    int wrappedLen = wrapText(m_pageBuffer, width - 2 * m_margin,
                             m_pageBuffer, sizeof(m_pageBuffer) - 1);

    // Render text
    int y = m_margin + 25;  // Below status bar
    int x = m_margin;
    int line = 0;

    for (int i = 0; i < wrappedLen && m_pageBuffer[i] != '\0'; i++) {
        if (m_pageBuffer[i] == '\n') {
            line++;
            y += getLineHeight();
            x = m_margin;
            continue;
        }

        m_display->drawChar(x, y, m_pageBuffer[i], 0x00, m_fontSizeIdx + 1);
        x += getCharWidth(m_fontSizeIdx);
    }
}

void TxtReaderActivity::renderStatusBar() {
    int16_t width = m_display->getRotatedWidth();

    // Status bar background
    m_display->fillRect(0, 0, width, 25, 0x00);

    // Page info
    char statusText[64];
    snprintf(statusText, sizeof(statusText), "%d / %d", m_currentPage + 1, m_totalPages);
    m_display->drawString(10, 6, statusText, 0xFF, 1);

    // Front light indicator (if on)
    if (m_frontLight && m_frontLight->isOn()) {
        const char* lightIcon = "L:";
        m_display->drawString(width / 2 - 20, 6, lightIcon, 0xFF, 1);
    }

    // Battery
    uint8_t batteryPct = m_power->getBatteryPercentage();
    char batText[16];
    snprintf(batText, sizeof(batText), "%d%%", batteryPct);
    m_display->drawString(width - 40, 6, batText, 0xFF, 1);
}

void TxtReaderActivity::renderQuickSettings() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // Darken background
    // (In real implementation, would draw overlay)

    // Panel background
    int panelY = height / 4;
    int panelH = height / 2;
    m_display->fillRect(0, panelY, width, panelH, 0xFF);
    m_display->drawRect(0, panelY, width, panelH, 0x00);

    // Title
    const char* title = "Quick Settings";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, panelY + 10, title, 0x00, 2);

    // Brightness slider
    int y = panelY + 50;
    m_display->drawString(30, y, "Brightness", 0x00, 1);
    
    int sliderX = 150;
    int sliderW = width - 180;
    int sliderY = y + 5;
    m_display->drawRect(sliderX, sliderY, sliderW, 10, 0x00);
    
    if (m_frontLight) {
        int bright = m_frontLight->getBrightness();
        int fillW = (sliderW * bright) / 100;
        m_display->fillRect(sliderX + 1, sliderY + 1, fillW - 2, 8, 0x00);
        
        char valBuf[8];
        snprintf(valBuf, sizeof(valBuf), "%d%%", bright);
        m_display->drawString(sliderX + sliderW + 10, y, valBuf, 0x00, 1);
    }

    // Warmth slider
    y += 40;
    m_display->drawString(30, y, "Warmth", 0x00, 1);
    
    sliderY = y + 5;
    m_display->drawRect(sliderX, sliderY, sliderW, 10, 0x00);
    
    if (m_frontLight) {
        int warmth = m_frontLight->getWarmth();
        int fillW = (sliderW * warmth) / 100;
        m_display->fillRect(sliderX + 1, sliderY + 1, fillW - 2, 8, 0x00);
        
        char valBuf[8];
        snprintf(valBuf, sizeof(valBuf), "%d%%", warmth);
        m_display->drawString(sliderX + sliderW + 10, y, valBuf, 0x00, 1);
    }

    // Toggle hint
    const char* hint = "Swipe up/down: Adjust  Tap: Close";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, panelY + panelH - 25, hint, 0x40, 1);
}

void TxtReaderActivity::nextPage() {
    if (m_currentPage < m_totalPages - 1) {
        m_currentPage++;
        int charsPerLine = (m_display->getRotatedWidth() - 2 * m_margin) / getCharWidth(m_fontSizeIdx);
        int linesPerPage = (m_display->getRotatedHeight() - 2 * m_margin - 30) / getLineHeight();
        m_currentOffset = m_currentPage * charsPerLine * linesPerPage;
        m_needsRender = true;
    }
}

void TxtReaderActivity::prevPage() {
    if (m_currentPage > 0) {
        m_currentPage--;
        int charsPerLine = (m_display->getRotatedWidth() - 2 * m_margin) / getCharWidth(m_fontSizeIdx);
        int linesPerPage = (m_display->getRotatedHeight() - 2 * m_margin - 30) / getLineHeight();
        m_currentOffset = m_currentPage * charsPerLine * linesPerPage;
        m_needsRender = true;
    }
}

void TxtReaderActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_RIGHT:
        case BTN_VOL_UP:
            if (m_showQuickSettings) {
                // Adjust value up
                if (m_quickSettingsItem == 0 && m_frontLight) {
                    m_frontLight->brightnessUp(5);
                } else if (m_quickSettingsItem == 1 && m_frontLight) {
                    m_frontLight->warmthUp(5);
                }
            } else {
                nextPage();
            }
            m_needsRender = true;
            break;

        case BTN_LEFT:
        case BTN_VOL_DOWN:
            if (m_showQuickSettings) {
                // Adjust value down
                if (m_quickSettingsItem == 0 && m_frontLight) {
                    m_frontLight->brightnessDown(5);
                } else if (m_quickSettingsItem == 1 && m_frontLight) {
                    m_frontLight->warmthDown(5);
                }
            } else {
                prevPage();
            }
            m_needsRender = true;
            break;

        case BTN_CONFIRM:
            if (m_showQuickSettings) {
                m_showQuickSettings = false;
            } else {
                // Toggle status bar
                m_showStatusBar = !m_showStatusBar;
                m_statusBarHideTime = millis();
            }
            m_needsRender = true;
            break;

        case BTN_BACK:
            if (m_showQuickSettings) {
                m_showQuickSettings = false;
            } else if (m_manager) {
                m_manager->goBack();
            }
            m_needsRender = true;
            break;

        default:
            break;
    }
}

void TxtReaderActivity::handleTouch(TouchEvent event) {
    switch (event.gesture) {
        case GESTURE_TAP:
            if (m_showQuickSettings) {
                // Tap outside panel to close
                int16_t height = m_display->getRotatedHeight();
                if (event.endPoint.y < height / 4 || event.endPoint.y > height * 3 / 4) {
                    m_showQuickSettings = false;
                    m_needsRender = true;
                }
            } else {
                // Tap to show/hide status bar
                m_showStatusBar = !m_showStatusBar;
                m_statusBarHideTime = millis();
                m_needsRender = true;
            }
            break;

        case GESTURE_SWIPE_LEFT:
            if (!m_showQuickSettings) {
                nextPage();
            }
            break;

        case GESTURE_SWIPE_RIGHT:
            if (!m_showQuickSettings) {
                prevPage();
            }
            break;

        case GESTURE_SWIPE_DOWN:
            // Pull down to show quick settings
            if (!m_showQuickSettings) {
                m_showQuickSettings = true;
                m_quickSettingsItem = 0;
            } else if (m_quickSettingsItem > 0) {
                m_quickSettingsItem--;
            }
            m_needsRender = true;
            break;

        case GESTURE_SWIPE_UP:
            if (m_showQuickSettings) {
                m_quickSettingsItem++;
                if (m_quickSettingsItem > 1) {
                    m_showQuickSettings = false;
                }
            }
            m_needsRender = true;
            break;

        case GESTURE_LONG_PRESS:
            // Long press to toggle front light
            if (m_frontLight && !m_showQuickSettings) {
                m_frontLight->toggle();
                m_showStatusBar = true;
                m_statusBarHideTime = millis();
                m_needsRender = true;
            }
            break;

        default:
            break;
    }
}

int TxtReaderActivity::getCharWidth(int fontIdx) {
    // Font sizes: 0=small, 1=medium, 2=large, 3=xlarge
    // Approximate character widths in pixels
    switch (fontIdx) {
        case 0: return 5;   // Small
        case 1: return 8;   // Medium
        case 2: return 12;  // Large
        case 3: return 16;  // XLarge
        default: return 8;
    }
}

int TxtReaderActivity::getLineHeight() {
    int baseHeight = getCharWidth(m_fontSizeIdx) + 2;
    // Line spacing: 0=tight, 1=normal, 2=relaxed
    switch (m_lineSpacing) {
        case 0: return baseHeight;          // Tight
        case 1: return baseHeight + 4;      // Normal
        case 2: return baseHeight + 8;      // Relaxed
        default: return baseHeight + 4;
    }
}

int TxtReaderActivity::wrapText(const char* text, int maxWidth, char* output, int maxOutputLen) {
    int outIdx = 0;
    int lineWidth = 0;
    int charWidth = getCharWidth(m_fontSizeIdx);

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
