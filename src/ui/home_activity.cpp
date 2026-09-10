/**
 * StarReader Firmware - Home Activity Implementation
 */

#include "home_activity.h"
#include "../config.h"
#include "txt_reader_activity.h"
#include "settings_activity.h"

HomeActivity::HomeActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power)
    : Activity(display, input, storage, power)
    , m_selectedItem(0)
    , m_lastButtonTime(0)
    , m_needsRender(true) {
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
    // Poll input
    m_input->update();
    ButtonState event = m_input->getLastEvent();

    if (event.event == EVENT_PRESSED) {
        handleButton(event);
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

    // Refresh display
    m_display->refresh(HalDisplay::FULL_REFRESH);
    m_needsRender = false;
}

void HomeActivity::drawStatusBar() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // Status bar background
    m_display->fillRect(0, 0, width, 30, 0x00);  // Black bar

    // Battery percentage
    uint8_t batteryPct = m_power->getBatteryPercentage();
    char batteryText[16];
    snprintf(batteryText, sizeof(batteryText), "BAT: %d%%", batteryPct);
    m_display->drawString(10, 8, batteryText, 0xFF, 1);  // White text

    // USB status
    if (m_power->isUsbConnected()) {
        m_display->drawString(width - 60, 8, "USB", 0xFF, 1);
    }

    // Title
    const char* title = "StarReader";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 5, title, 0xFF, 2);
}

void HomeActivity::drawMenu() {
    int16_t width = m_display->getRotatedWidth();

    const char* menuItems[] = {
        "Library",
        "Settings",
        "About",
        "Sleep"
    };

    for (int i = 0; i < MENU_COUNT; i++) {
        int y = MENU_START_Y + i * MENU_ITEM_HEIGHT;

        // Highlight selected item
        if (i == m_selectedItem) {
            m_display->fillRect(MENU_ITEM_PADDING, y,
                              width - 2 * MENU_ITEM_PADDING,
                              MENU_ITEM_HEIGHT - 10, 0x00);  // Black background
            m_display->drawString(MENU_ITEM_PADDING + 20, y + 15,
                                menuItems[i], 0xFF, 2);  // White text
        } else {
            m_display->drawString(MENU_ITEM_PADDING + 20, y + 15,
                                menuItems[i], 0x00, 2);  // Black text
        }
    }

    // Footer hint
    const char* hint = "Left/Right: Navigate  Confirm: Select  Back: Exit";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    int16_t height = m_display->getRotatedHeight();
    m_display->drawString((width - hintWidth) / 2, height - 25, hint, 0x00, 1);
}

void HomeActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_LEFT:
            if (m_selectedItem > 0) {
                m_selectedItem--;
                m_needsRender = true;
            }
            break;

        case BTN_RIGHT:
            if (m_selectedItem < MENU_COUNT - 1) {
                m_selectedItem++;
                m_needsRender = true;
            }
            break;

        case BTN_CONFIRM:
            switch (m_selectedItem) {
                case MENU_LIBRARY:
                    // TODO: Open library browser
                    // For now, open a sample TXT reader
                    if (m_manager) {
                        TxtReaderActivity* reader = new TxtReaderActivity(
                            m_display, m_input, m_storage, m_power,
                            "/books/sample.txt");
                        m_manager->pushActivity(reader);
                    }
                    break;

                case MENU_SETTINGS:
                    if (m_manager) {
                        SettingsActivity* settings = new SettingsActivity(
                            m_display, m_input, m_storage, m_power);
                        m_manager->pushActivity(settings);
                    }
                    break;

                case MENU_ABOUT:
                    // TODO: Show about screen
                    break;

                case MENU_SLEEP:
                    // Enter deep sleep
                    m_power->enterDeepSleep();
                    break;
            }
            break;

        case BTN_BACK:
            // Exit / sleep
            m_power->enterDeepSleep();
            break;

        default:
            break;
    }
}
