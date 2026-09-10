/**
 * StarReader Firmware - Settings Activity Implementation
 */

#include "settings_activity.h"
#include "../config.h"

SettingsActivity::SettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power)
    : Activity(display, input, storage, power)
    , m_selectedItem(0)
    , m_needsRender(true)
    , m_fontSize(1)
    , m_orientation(1)  // Landscape
    , m_refreshMode(0)  // Full
    , m_sleepTimeout(5) {
}

SettingsActivity::~SettingsActivity() {
}

void SettingsActivity::onEnter() {
    m_selectedItem = 0;
    m_needsRender = true;
}

void SettingsActivity::onExit() {
    // Save settings to SD card
    // TODO: Implement settings persistence
}

void SettingsActivity::onResume() {
    m_needsRender = true;
}

void SettingsActivity::loop() {
    m_input->update();
    ButtonState event = m_input->getLastEvent();

    if (event.event == EVENT_PRESSED) {
        handleButton(event);
    }
}

void SettingsActivity::render() {
    if (!m_needsRender) return;

    m_display->clear(0xFF);

    // Title bar
    int16_t width = m_display->getRotatedWidth();
    m_display->fillRect(0, 0, width, 35, 0x00);
    const char* title = "Settings";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 8, title, 0xFF, 2);

    drawSettings();

    // Footer
    const char* hint = "Up/Down: Select  Left/Right: Adjust  Back: Exit";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    int16_t height = m_display->getRotatedHeight();
    m_display->drawString((width - hintWidth) / 2, height - 20, hint, 0x00, 1);

    m_display->refresh(HalDisplay::FULL_REFRESH);
    m_needsRender = false;
}

void SettingsActivity::drawSettings() {
    const char* settingNames[] = {
        "Font Size",
        "Orientation",
        "Refresh Mode",
        "Sleep Timeout",
        "About"
    };

    for (int i = 0; i < SETTING_COUNT; i++) {
        int y = MENU_START_Y + i * MENU_ITEM_HEIGHT;

        // Highlight selected
        if (i == m_selectedItem) {
            m_display->fillRect(10, y, 460, MENU_ITEM_HEIGHT - 5, 0x00);
            m_display->drawString(20, y + 10, settingNames[i], 0xFF, 1);
            m_display->drawString(300, y + 10, getSettingValueText(i), 0xFF, 1);
        } else {
            m_display->drawString(20, y + 10, settingNames[i], 0x00, 1);
            m_display->drawString(300, y + 10, getSettingValueText(i), 0x00, 1);
        }
    }
}

void SettingsActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_VOL_UP:
            if (m_selectedItem > 0) {
                m_selectedItem--;
                m_needsRender = true;
            }
            break;

        case BTN_VOL_DOWN:
            if (m_selectedItem < SETTING_COUNT - 1) {
                m_selectedItem++;
                m_needsRender = true;
            }
            break;

        case BTN_LEFT:
        case BTN_RIGHT:
            cycleSetting(m_selectedItem);
            m_needsRender = true;
            break;

        case BTN_BACK:
            if (m_manager) {
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}

void SettingsActivity::cycleSetting(int item) {
    switch (item) {
        case SETTING_FONT_SIZE:
            m_fontSize = (m_fontSize % 3) + 1;  // 1, 2, 3
            break;

        case SETTING_ORIENTATION:
            m_orientation = (m_orientation + 1) % 4;
            m_display->setOrientation((HalDisplay::Orientation)m_orientation);
            break;

        case SETTING_REFRESH_MODE:
            m_refreshMode = (m_refreshMode + 1) % 3;
            break;

        case SETTING_SLEEP_TIMEOUT:
            if (m_sleepTimeoutMin >= 60) {
                m_sleepTimeoutMin = 1;
            } else {
                m_sleepTimeoutMin *= 2;
            }
            break;

        case SETTING_ABOUT:
            // TODO: Show about dialog
            break;
    }
}

const char* SettingsActivity::getSettingValueText(int item) {
    switch (item) {
        case SETTING_FONT_SIZE:
            switch (m_fontSize) {
                case 1: return "Small";
                case 2: return "Medium";
                case 3: return "Large";
                default: return "?";
            }

        case SETTING_ORIENTATION:
            switch (m_orientation) {
                case 0: return "Portrait";
                case 1: return "Landscape";
                case 2: return "Portrait Inv";
                case 3: return "Landscape Inv";
                default: return "?";
            }

        case SETTING_REFRESH_MODE:
            switch (m_refreshMode) {
                case 0: return "Full";
                case 1: return "Partial";
                case 2: return "Fast";
                default: return "?";
            }

        case SETTING_SLEEP_TIMEOUT:
            if (m_sleepTimeoutMin >= 60) {
                return "Never";
            } else {
                static char buf[16];
                snprintf(buf, sizeof(buf), "%d min", m_sleepTimeoutMin);
                return buf;
            }

        case SETTING_ABOUT:
            return "v0.1.0";

        default:
            return "";
    }
}
