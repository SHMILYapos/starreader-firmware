/**
 * StarReader Pro Firmware - Settings Activity Implementation
 */

#include "settings_activity.h"
#include "../config.h"
#include <string.h>

SettingsActivity::SettingsActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                   HalPowerManager* power, HalTouch* touch,
                                   HalFrontLight* frontLight,
                                   SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedItem(0)
    , m_needsRender(true)
    , m_touch(touch)
    , m_frontLight(frontLight)
    , m_settingsManager(settingsManager)
    , m_frontLightOn(false)
    , m_brightness(50)
    , m_warmth(50)
    , m_fontSize(1)
    , m_orientation(1)
    , m_refreshMode(1)
    , m_lineSpacing(1)
    , m_justification(1)
    , m_sleepTimeoutMin(5)
    , m_autoRefreshPages(50) {
}

SettingsActivity::~SettingsActivity() {
}

void SettingsActivity::onEnter() {
    m_selectedItem = 0;
    m_needsRender = true;

    // Load current settings
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        m_frontLightOn = s->frontLightOn;
        m_brightness = s->brightness;
        m_warmth = s->frontLightWarmth;
        m_fontSize = s->fontSize;
        m_orientation = s->orientation;
        m_refreshMode = s->refreshMode;
        m_lineSpacing = s->lineSpacing;
        m_justification = s->justification;
        m_sleepTimeoutMin = s->sleepTimeoutSec / 60;
        m_autoRefreshPages = s->autoRefreshPages;
    } else if (m_frontLight) {
        m_frontLightOn = m_frontLight->isOn();
        m_brightness = m_frontLight->getBrightness();
        m_warmth = m_frontLight->getWarmth();
    }
}

void SettingsActivity::onExit() {
    // Save settings
    applySettings();
}

void SettingsActivity::onResume() {
    m_needsRender = true;
}

void SettingsActivity::loop() {
    // Handle physical buttons
    m_input->update();
    ButtonState event = m_input->getLastEvent();

    if (event.event == EVENT_PRESSED) {
        handleButton(event);
    }

    // Handle touch
    if (m_touch) {
        TouchEvent touchEvent = m_touch->getLastEvent();
        if (touchEvent.gesture != GESTURE_NONE) {
            handleTouch(touchEvent);
        }
    }
}

void SettingsActivity::render() {
    if (!m_needsRender) return;

    m_display->clear(0xFF);
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // Title bar
    m_display->fillRect(0, 0, width, 35, 0x00);
    const char* title = "Settings";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 8, title, 0xFF, 2);

    drawSettings();

    // Footer
    const char* hint = "Up/Down: Select  Left/Right: Adjust  Back: Exit";
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x00, 1);

    m_display->refresh(HalDisplay::FULL_REFRESH);
    m_needsRender = false;
}

void SettingsActivity::drawSettings() {
    const char* settingNames[] = {
        // Front Light
        "Front Light",
        "  Brightness",
        "  Warmth",
        "  Presets",
        // Display
        "Font Size",
        "Orientation",
        "Refresh Mode",
        // Reader
        "Line Spacing",
        "Justification",
        // Power
        "Sleep Timeout",
        "Auto Refresh",
        // System
        "About"
    };

    for (int i = 0; i < SETTING_COUNT; i++) {
        int y = MENU_START_Y + i * MENU_ITEM_HEIGHT;

        // Section headers (group headers)
        bool isHeader = (i == SETTING_FRONT_LIGHT_ON || i == SETTING_FONT_SIZE || 
                         i == SETTING_LINE_SPACING || i == SETTING_SLEEP_TIMEOUT);

        // Highlight selected
        if (i == m_selectedItem) {
            m_display->fillRect(5, y, 470, MENU_ITEM_HEIGHT - 4, 0x00);
            m_display->drawString(15, y + 8, settingNames[i], 0xFF, 1);
            
            // Draw slider for brightness/warmth
            if (i == SETTING_BRIGHTNESS || i == SETTING_WARMTH) {
                // Skip text value, use slider instead
            } else {
                m_display->drawString(320, y + 8, getSettingValueText(i), 0xFF, 1);
            }
        } else {
            uint8_t color = isHeader ? 0x00 : 0x40;  // Headers darker
            m_display->drawString(15, y + 8, settingNames[i], color, 1);
            
            if (i == SETTING_BRIGHTNESS || i == SETTING_WARMTH) {
                // Draw slider preview
                int sliderY = y + 14;
                int sliderX = 320;
                int sliderW = 140;
                
                // Slider background
                m_display->drawRect(sliderX, sliderY, sliderW, 8, 0x00);
                
                // Slider fill
                int value = (i == SETTING_BRIGHTNESS) ? m_brightness : m_warmth;
                int fillW = (sliderW * value) / 100;
                m_display->fillRect(sliderX + 1, sliderY + 1, fillW - 2, 6, 0x00);
            } else {
                m_display->drawString(320, y + 8, getSettingValueText(i), 0x40, 1);
            }
        }
    }
}

void SettingsActivity::drawSlider(int y, const char* label, int value, int maxValue) {
    // Reserved for future slider UI
    (void)y; (void)label; (void)value; (void)maxValue;
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
            cycleSetting(m_selectedItem, -1);
            m_needsRender = true;
            break;

        case BTN_RIGHT:
            cycleSetting(m_selectedItem, 1);
            m_needsRender = true;
            break;

        case BTN_BACK:
            if (m_manager) {
                applySettings();
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}

void SettingsActivity::handleTouch(TouchEvent event) {
    switch (event.gesture) {
        case GESTURE_TAP:
            // Tap on a setting item to select it
            if (event.endPoint.y > MENU_START_Y && event.endPoint.y < MENU_START_Y + SETTING_COUNT * MENU_ITEM_HEIGHT) {
                int idx = (event.endPoint.y - MENU_START_Y) / MENU_ITEM_HEIGHT;
                if (idx >= 0 && idx < SETTING_COUNT) {
                    m_selectedItem = idx;
                    m_needsRender = true;
                }
            }
            break;

        case GESTURE_SWIPE_DOWN:
            // Swipe down = adjust value down
            cycleSetting(m_selectedItem, -1);
            m_needsRender = true;
            break;

        case GESTURE_SWIPE_UP:
            // Swipe up = adjust value up
            cycleSetting(m_selectedItem, 1);
            m_needsRender = true;
            break;

        case GESTURE_SWIPE_RIGHT:
            // Swipe right = go back
            if (m_manager) {
                applySettings();
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}

void SettingsActivity::cycleSetting(int item, int direction) {
    switch (item) {
        case SETTING_FRONT_LIGHT_ON:
            m_frontLightOn = !m_frontLightOn;
            if (m_frontLight) {
                if (m_frontLightOn) m_frontLight->on();
                else m_frontLight->off();
            }
            break;

        case SETTING_BRIGHTNESS: {
            int newBrightness = m_brightness + direction * 5;
            if (newBrightness < 0) newBrightness = 0;
            if (newBrightness > 100) newBrightness = 100;
            m_brightness = newBrightness;
            if (m_frontLight) {
                m_frontLight->setBrightness(m_brightness);
                if (m_brightness > 0 && !m_frontLightOn) {
                    m_frontLightOn = true;
                    m_frontLight->on();
                }
            }
            break;
        }

        case SETTING_WARMTH: {
            int newWarmth = m_warmth + direction * 5;
            if (newWarmth < 0) newWarmth = 0;
            if (newWarmth > 100) newWarmth = 100;
            m_warmth = newWarmth;
            if (m_frontLight) {
                m_frontLight->setWarmth(m_warmth);
            }
            break;
        }

        case SETTING_PRESETS:
            // Cycle through presets
            if (m_frontLight) {
                static int presetIdx = 0;
                presetIdx = (presetIdx + 1) % 3;
                switch (presetIdx) {
                    case 0: m_frontLight->setPresetReading(); break;
                    case 1: m_frontLight->setPresetNight(); break;
                    case 2: m_frontLight->setPresetDay(); break;
                }
                m_brightness = m_frontLight->getBrightness();
                m_warmth = m_frontLight->getWarmth();
                m_frontLightOn = true;
            }
            break;

        case SETTING_FONT_SIZE:
            m_fontSize = (m_fontSize + direction + 4) % 4;  // 0-3
            break;

        case SETTING_ORIENTATION:
            m_orientation = (m_orientation + direction + 4) % 4;
            m_display->setOrientation((HalDisplay::Orientation)m_orientation);
            break;

        case SETTING_REFRESH_MODE:
            m_refreshMode = (m_refreshMode + direction + 3) % 3;
            break;

        case SETTING_LINE_SPACING:
            m_lineSpacing = (m_lineSpacing + direction + 3) % 3;
            break;

        case SETTING_JUSTIFICATION:
            m_justification = (m_justification + direction + 2) % 2;
            break;

        case SETTING_SLEEP_TIMEOUT:
            if (direction > 0) {
                if (m_sleepTimeoutMin >= 60) m_sleepTimeoutMin = 1;
                else m_sleepTimeoutMin *= 2;
            } else {
                if (m_sleepTimeoutMin <= 1) m_sleepTimeoutMin = 60;
                else m_sleepTimeoutMin /= 2;
            }
            break;

        case SETTING_AUTO_REFRESH:
            m_autoRefreshPages += direction * 10;
            if (m_autoRefreshPages < 10) m_autoRefreshPages = 10;
            if (m_autoRefreshPages > 200) m_autoRefreshPages = 200;
            break;

        case SETTING_ABOUT:
            // TODO: Show about dialog
            break;
    }
}

const char* SettingsActivity::getSettingValueText(int item) {
    static char buf[24];

    switch (item) {
        case SETTING_FRONT_LIGHT_ON:
            return m_frontLightOn ? "ON" : "OFF";

        case SETTING_BRIGHTNESS:
            snprintf(buf, sizeof(buf), "%d%%", m_brightness);
            return buf;

        case SETTING_WARMTH:
            snprintf(buf, sizeof(buf), "%d%%", m_warmth);
            return buf;

        case SETTING_PRESETS:
            return "Reading/Night/Day";

        case SETTING_FONT_SIZE:
            switch (m_fontSize) {
                case 0: return "Small";
                case 1: return "Medium";
                case 2: return "Large";
                case 3: return "XLarge";
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

        case SETTING_LINE_SPACING:
            switch (m_lineSpacing) {
                case 0: return "Tight";
                case 1: return "Normal";
                case 2: return "Relaxed";
                default: return "?";
            }

        case SETTING_JUSTIFICATION:
            return m_justification ? "Justified" : "Left";

        case SETTING_SLEEP_TIMEOUT:
            if (m_sleepTimeoutMin >= 60) {
                return "Never";
            } else {
                snprintf(buf, sizeof(buf), "%d min", m_sleepTimeoutMin);
                return buf;
            }

        case SETTING_AUTO_REFRESH:
            snprintf(buf, sizeof(buf), "%d pages", m_autoRefreshPages);
            return buf;

        case SETTING_ABOUT:
            return "v0.2.0 Pro";

        default:
            return "";
    }
}

void SettingsActivity::applySettings() {
    if (!m_settingsManager) return;

    StarReaderSettings* s = m_settingsManager->getSettings();
    s->frontLightOn = m_frontLightOn ? 1 : 0;
    s->brightness = m_brightness;
    s->frontLightWarmth = m_warmth;
    s->fontSize = m_fontSize;
    s->orientation = m_orientation;
    s->refreshMode = m_refreshMode;
    s->lineSpacing = m_lineSpacing;
    s->justification = m_justification;
    s->sleepTimeoutSec = m_sleepTimeoutMin * 60;
    s->autoRefreshPages = m_autoRefreshPages;

    m_settingsManager->save();
}
