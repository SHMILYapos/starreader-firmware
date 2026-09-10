/**
 * StarReader Pro Firmware - WiFi Activity Implementation
 * 
 * WiFi 网络配置界面实现
 */

#include "wifi_activity.h"
#include "../config.h"
#include "../utils/i18n.h"
#include <string.h>
#include <cstdio>
#include <Arduino.h>

#ifdef ARDUINO
#include <WiFi.h>
#include <WebServer.h>
#endif

// WiFi 相关字符串ID需要添加到i18n中
// 临时硬编码，后续移到i18n
#define STR_WIFI_SETUP "WiFi设置"
#define STR_WIFI_STATUS "WiFi状态"
#define STR_WIFI_SSID "网络名称"
#define STR_WIFI_PASSWORD "密码"
#define STR_WIFI_CONNECT "连接"
#define STR_WIFI_DISCONNECT "断开连接"
#define STR_WEB_SERVER "Web传书"
#define STR_WEB_SERVER_START "启动Web服务器"
#define STR_OTA_UPDATE "OTA更新"
#define STR_OTA_CHECK "检查更新"
#define STR_CONNECTING "连接中..."
#define STR_CONNECTED "已连接"
#define STR_DISCONNECTED "未连接"
#define STR_IP_ADDRESS "IP地址"

WifiActivity::WifiActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                           HalPowerManager* power, HalTouch* touch,
                           HalFrontLight* frontLight,
                           SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_touch(touch)
    , m_frontLight(frontLight)
    , m_settingsManager(settingsManager)
    , m_needsRender(true)
    , m_connecting(false)
    , m_selectedItem(0)
    , m_connectStartTime(0) {
}

WifiActivity::~WifiActivity() {
}

void WifiActivity::onEnter() {
    m_needsRender = true;
}

void WifiActivity::onExit() {
}

void WifiActivity::onResume() {
    m_needsRender = true;
}

void WifiActivity::loop() {
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

    // 连接状态检查
    if (m_connecting) {
        if (millis() - m_connectStartTime > 10000) {
            // 10秒超时
            m_connecting = false;
            m_needsRender = true;
        } else if (WiFi.status() == WL_CONNECTED) {
            m_connecting = false;
            m_needsRender = true;
        }
    }
}

void WifiActivity::render() {
    if (!m_needsRender) return;

    m_display->clear(0xFF);
    
    // 标题栏
    int16_t width = m_display->getRotatedWidth();
    m_display->fillRect(0, 0, width, 35, 0x00);
    m_display->drawString(15, 8, STR_WIFI_SETUP, 0xFF, 2);

    drawWifiMenu();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    m_needsRender = false;
}

void WifiActivity::drawWifiMenu() {
    int y = MENU_START_Y;
    int16_t width = m_display->getRotatedWidth();

    // WiFi 状态
    bool connected = (WiFi.status() == WL_CONNECTED);
    const char* status = connected ? STR_CONNECTED : STR_DISCONNECTED;
    
    m_display->drawString(20, y, STR_WIFI_STATUS, 0x00, 1);
    m_display->drawString(150, y, status, connected ? 0x00 : 0x40, 1);
    y += MENU_ITEM_HEIGHT;

    // IP 地址
    if (connected) {
        char ipBuf[32];
        snprintf(ipBuf, sizeof(ipBuf), "%s: %s", STR_IP_ADDRESS, WiFi.localIP().toString().c_str());
        m_display->drawString(20, y, ipBuf, 0x00, 1);
        y += MENU_ITEM_HEIGHT;
    }

    // 菜单选项
    const char* menuItems[] = {
        STR_WIFI_CONNECT,
        STR_WEB_SERVER_START,
        STR_OTA_CHECK,
    };

    int itemCount = sizeof(menuItems) / sizeof(menuItems[0]);

    for (int i = 0; i < itemCount; i++) {
        bool isSelected = (i == m_selectedItem);
        
        if (isSelected) {
            m_display->fillRect(5, y, width - 10, MENU_ITEM_HEIGHT - 4, 0x00);
            m_display->drawString(20, y + 8, menuItems[i], 0xFF, 1);
        } else {
            m_display->drawString(20, y + 8, menuItems[i], 0x00, 1);
        }
        y += MENU_ITEM_HEIGHT;
    }

    // 连接中提示
    if (m_connecting) {
        m_display->fillRect(0, m_display->getRotatedHeight() - 40, width, 40, 0x00);
        int16_t textWidth = m_display->getStringWidth(STR_CONNECTING, 1);
        m_display->drawString((width - textWidth) / 2, m_display->getRotatedHeight() - 28, 
                             STR_CONNECTING, 0xFF, 1);
    }

    // 底部提示
    if (!m_connecting) {
        const char* hint = "上下键: 选择  确认: 执行  返回: 退出";
        int16_t hintWidth = m_display->getStringWidth(hint, 1);
        m_display->drawString((width - hintWidth) / 2, m_display->getRotatedHeight() - 20, 
                             hint, 0x40, 1);
    }
}

void WifiActivity::handleButton(ButtonState event) {
    int menuCount = 3;

    switch (event.id) {
        case BTN_LEFT:
        case BTN_VOL_UP:
            if (m_selectedItem > 0) {
                m_selectedItem--;
                m_needsRender = true;
            }
            break;

        case BTN_RIGHT:
        case BTN_VOL_DOWN:
            if (m_selectedItem < menuCount - 1) {
                m_selectedItem++;
                m_needsRender = true;
            }
            break;

        case BTN_CONFIRM:
            switch (m_selectedItem) {
                case 0:
                    connectToWifi();
                    break;
                case 1:
                    startWebServer();
                    break;
                case 2:
                    // OTA 检查更新
                    m_needsRender = true;
                    break;
            }
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

void WifiActivity::handleTouch(TouchEvent event) {
    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;
            int menuStart = MENU_START_Y + 40;  // 跳过状态显示
            int idx = (y - menuStart) / MENU_ITEM_HEIGHT;
            if (idx >= 0 && idx < 3) {
                m_selectedItem = idx;
                m_needsRender = true;
                // 双击执行
                switch (idx) {
                    case 0: connectToWifi(); break;
                    case 1: startWebServer(); break;
                }
            }
            break;
        }

        case GESTURE_SWIPE_RIGHT:
            if (m_manager) {
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}

void WifiActivity::connectToWifi() {
#ifdef ARDUINO
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        if (strlen(s->wifiSsid) > 0) {
            m_connecting = true;
            m_connectStartTime = millis();
            
            WiFi.mode(WIFI_STA);
            WiFi.begin(s->wifiSsid, s->wifiPassword);
            
            m_needsRender = true;
        }
    }
#endif
}

void WifiActivity::startWebServer() {
#ifdef ARDUINO
    if (WiFi.status() == WL_CONNECTED) {
        // 启动Web服务器（简化版，实际需要完整实现）
        // TODO: 实现完整的Web服务器用于文件上传
        m_needsRender = true;
    }
#endif
}
