/**
 * StarReader Pro Firmware - Home Activity Implementation
 */

#include "home_activity.h"
#include "../config.h"
#include "txt_reader_activity.h"
#include "settings_activity.h"
#include <string.h>

HomeActivity::HomeActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power,
                           HalTouch* touch, HalFrontLight* frontLight,
                           SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedItem(0)
    , m_lastButtonTime(0)
    , m_needsRender(true)
    , m_touch(touch)
    , m_frontLight(frontLight)
    , m_settingsManager(settingsManager) {
}

HomeActivity::~HomeActivity() {
}

void HomeActivity::onEnter() {
    m_selectedItem = 0;
    m_needsRender = true;
}

void HomeActivity::onExit() {
}

void HomeActivity::onResume() {
    m_needsRender = true;
}

void HomeActivity::loop() {
    // Poll physical buttons
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
}

void HomeActivity::render() {
    if (!m_needsRender) return;

    // Clear screen
    m_display->clear(0xFF);  // White

    // Draw status bar
    drawStatusBar();

    // Draw menu
    drawMenu();

    // Draw footer hint
    drawFooter();

    // Refresh display
    m_display->refresh(HalDisplay::FULL_REFRESH);
    m_needsRender = false;
}

void HomeActivity::drawStatusBar() {
    int16_t width = m_display->getRotatedWidth();

    // Status bar background
    m_display->fillRect(0, 0, width, 30, 0x00);  // Black bar

    // Battery percentage (left)
    uint8_t batteryPct = m_power->getBatteryPercentage();
    char batteryText[16];
    snprintf(batteryText, sizeof(batteryText), "%d%%", batteryPct);
    m_display->drawString(10, 8, batteryText, 0xFF, 1);  // White text

    // Front light status (center)
    if (m_frontLight && m_frontLight->isOn()) {
        char lightText[16];
        snprintf(lightText, sizeof(lightText), "Light %d%%", m_frontLight->getBrightness());
        int16_t textWidth = m_display->getStringWidth(lightText, 1);
        m_display->drawString((width - textWidth) / 2, 8, lightText, 0xFF, 1);
    }

    // Title (right of center)
    const char* title = "StarReader Pro";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2 + 80, 5, title, 0xFF, 2);

    // USB / Magnetic status (right)
    if (m_power->isUsbConnected()) {
        m_display->drawString(width - 50, 8, "USB", 0xFF, 1);
    }
}

void HomeActivity::drawMenu() {
    int16_t width = m_display->getRotatedWidth();

    const char* menuItems[] = {
        "Continue Reading",
        "Library",
        "Settings",
        "About",
        "Sleep"
    };

    // Check if there's a last book to show "Continue Reading"
    bool hasLastBook = false;
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        hasLastBook = (s->lastBookPath[0] != '\0' && s->lastBookPage > 0);
    }

    for (int i = 0; i < MENU_COUNT; i++) {
        // Skip Continue Reading if no last book
        if (i == MENU_CONTINUE_READING && !hasLastBook) {
            continue;
        }

        int y = MENU_START_Y + i * MENU_ITEM_HEIGHT;

        // Highlight selected item
        if (i == m_selectedItem) {
            m_display->fillRect(MENU_ITEM_PADDING, y,
                              width - 2 * MENU_ITEM_PADDING,
                              MENU_ITEM_HEIGHT - 10, 0x00);  // Black background
            m_display->drawString(MENU_ITEM_PADDING + 25, y + 18,
                                menuItems[i], 0xFF, 2);  // White text

            // Show page info for continue reading
            if (i == MENU_CONTINUE_READING && m_settingsManager) {
                StarReaderSettings* s = m_settingsManager->getSettings();
                char pageText[32];
                snprintf(pageText, sizeof(pageText), "Page %d", s->lastBookPage);
                m_display->drawString(width - 120, y + 20, pageText, 0xFF, 1);
            }
        } else {
            m_display->drawString(MENU_ITEM_PADDING + 25, y + 18,
                                menuItems[i], 0x00, 2);  // Black text
        }
    }
}

void HomeActivity::drawFooter() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint = "Tap to select  Up/Down: Navigate  Right: Enter  Back: Exit";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 20, hint, 0x40, 1);
}

void HomeActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_VOL_UP:
            if (m_selectedItem > 0) {
                m_selectedItem--;
                m_needsRender = true;
            }
            break;

        case BTN_VOL_DOWN:
            if (m_selectedItem < MENU_COUNT - 1) {
                m_selectedItem++;
                m_needsRender = true;
            }
            break;

        case BTN_LEFT:
            // Adjust front light brightness down
            if (m_frontLight) {
                m_frontLight->brightnessDown(10);
                m_needsRender = true;
            }
            break;

        case BTN_RIGHT:
            // Adjust front light brightness up
            if (m_frontLight) {
                m_frontLight->brightnessUp(10);
                m_needsRender = true;
            }
            break;

        case BTN_CONFIRM:
            launchMenuItem(m_selectedItem);
            break;

        case BTN_BACK:
            // Enter deep sleep
            if (m_frontLight) m_frontLight->off();
            m_power->enterDeepSleep();
            break;

        default:
            break;
    }
}

void HomeActivity::handleTouch(TouchEvent event) {
    switch (event.gesture) {
        case GESTURE_TAP: {
            // Tap on a menu item to select and launch
            int16_t width = m_display->getRotatedWidth();
            int y = event.endPoint.y;

            for (int i = 0; i < MENU_COUNT; i++) {
                int itemY = MENU_START_Y + i * MENU_ITEM_HEIGHT;
                if (y >= itemY && y < itemY + MENU_ITEM_HEIGHT - 10) {
                    m_selectedItem = i;
                    m_needsRender = true;
                    launchMenuItem(i);
                    break;
                }
            }
            break;
        }

        case GESTURE_SWIPE_UP:
            if (m_selectedItem > 0) {
                m_selectedItem--;
                m_needsRender = true;
            }
            break;

        case GESTURE_SWIPE_DOWN:
            if (m_selectedItem < MENU_COUNT - 1) {
                m_selectedItem++;
                m_needsRender = true;
            }
            break;

        case GESTURE_LONG_PRESS:
            // Long press to toggle front light
            if (m_frontLight) {
                m_frontLight->toggle();
                m_needsRender = true;
            }
            break;

        default:
            break;
    }
}

void HomeActivity::launchMenuItem(int item) {
    if (!m_manager) return;

    switch (item) {
        case MENU_CONTINUE_READING:
        case MENU_LIBRARY: {
            // Open TXT reader
            const char* filePath = "/books/sample.txt";
            if (item == MENU_CONTINUE_READING && m_settingsManager) {
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

        case MENU_SETTINGS: {
            SettingsActivity* settings = new SettingsActivity(
                m_display, m_input, m_storage, m_power,
                m_touch, m_frontLight, m_settingsManager);
            m_manager->pushActivity(settings);
            break;
        }

        case MENU_ABOUT:
            // TODO: Show about screen
            break;

        case MENU_SLEEP:
            if (m_frontLight) m_frontLight->off();
            m_power->enterDeepSleep();
            break;
    }
}
