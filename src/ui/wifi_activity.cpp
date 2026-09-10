/**
 * StarReader Pro Firmware - WiFi Activity Implementation
 * 
 * WiFi 网络设置界面实现
 */

#include "wifi_activity.h"
#include "../config.h"
#include <string.h>
#include <cstdio>
#include <Arduino.h>

#ifdef ARDUINO
#include <WiFi.h>
#include <WebServer.h>
#endif

WifiActivity::WifiActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                           HalPowerManager* power, HalTouch* touch,
                           HalFrontLight* frontLight,
                           SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_selectedItem(0)
    , m_selectedNetwork(0)
    , m_viewMode(VIEW_MAIN)
    , m_wifiEnabled(false)
    , m_connecting(false)
    , m_scanning(false)
    , m_connectStartTime(0)
    , m_scanStartTime(0)
    , m_networkCount(0) {
    memset(m_currentSsid, 0, sizeof(m_currentSsid));
    memset(m_currentIp, 0, sizeof(m_currentIp));
    memset(m_networkSsids, 0, sizeof(m_networkSsids));
    memset(m_networkRssi, 0, sizeof(m_networkRssi));
}

WifiActivity::~WifiActivity() {
}

void WifiActivity::onEnter() {
    m_selectedItem = 0;
    m_viewMode = VIEW_MAIN;
    requestUpdate();
}

void WifiActivity::onExit() {
}

void WifiActivity::onResume() {
    requestUpdate();
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

    // 扫描完成检查
    if (m_scanning) {
        updateScanResults();
    }

    // 连接状态检查
    if (m_connecting) {
#ifdef ARDUINO
        if (millis() - m_connectStartTime > 10000) {
            m_connecting = false;
            requestUpdate();
        } else if (WiFi.status() == WL_CONNECTED) {
            m_connecting = false;
            strncpy(m_currentSsid, WiFi.SSID().c_str(), sizeof(m_currentSsid) - 1);
            strncpy(m_currentIp, WiFi.localIP().toString().c_str(), sizeof(m_currentIp) - 1);
            m_viewMode = VIEW_MAIN;
            requestUpdate();
        }
#endif
    }
}

void WifiActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    drawTitleBar();
    
    if (m_viewMode == VIEW_MAIN) {
        drawMainMenu();
    } else if (m_viewMode == VIEW_SCANNING) {
        drawScanningView();
    } else if (m_viewMode == VIEW_NETWORK_LIST) {
        drawNetworkList();
    }
    
    drawBottomHint();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    clearRenderFlag();
}

void WifiActivity::drawTitleBar() {
    int16_t width = m_display->getRotatedWidth();

    m_display->fillRect(0, 0, width, STATUS_BAR_HEIGHT, 0x00);

    // 返回箭头
    m_display->drawString(10, 6, "< 返回", 0xFF, 1);

    // 标题
    const char* title = "WiFi 设置";
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 4, title, 0xFF, 2);
}

void WifiActivity::drawMainMenu() {
    int16_t width = m_display->getRotatedWidth();

    const char* itemNames[] = {
        "WiFi 开关",
        "扫描网络",
        "当前连接",
        "Web 传书",
        "OTA 更新"
    };

    const char* itemSubtitles[] = {
        "开启 / 关闭 WiFi",
        "搜索附近可用网络",
        "查看当前连接状态",
        "浏览器上传书籍",
        "检查固件更新"
    };

    // 获取当前状态
#ifdef ARDUINO
    bool connected = (WiFi.status() == WL_CONNECTED);
#else
    bool connected = false;
#endif

    const char* statusValues[] = {
        m_wifiEnabled ? "ON" : "OFF",
        "扫描",
        connected ? m_currentSsid : "未连接",
        "启动",
        "检查"
    };

    int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        int y = startY + i * LIST_ITEM_HEIGHT;

        bool selected = (i == m_selectedItem);

        if (selected) {
            m_display->fillRect(LIST_PADDING, y, width - 2 * LIST_PADDING, LIST_ITEM_HEIGHT - 5, 0x00);
            m_display->drawString(LIST_PADDING + 15, y + 8, itemNames[i], 0xFF, 2);
            m_display->drawString(LIST_PADDING + 15, y + 28, itemSubtitles[i], 0xCC, 1);
            m_display->drawString(width - 100, y + 15, statusValues[i], 0xFF, 1);
        } else {
            m_display->drawString(LIST_PADDING + 15, y + 8, itemNames[i], 0x00, 2);
            m_display->drawString(LIST_PADDING + 15, y + 28, itemSubtitles[i], 0x60, 1);
            m_display->drawString(width - 100, y + 15, statusValues[i], 0x60, 1);
        }
    }
}

void WifiActivity::drawScanningView() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // 扫描中动画文字
    const char* text = "正在扫描网络...";
    int16_t textWidth = m_display->getStringWidth(text, 2);
    m_display->drawString((width - textWidth) / 2, height / 2 - 20, text, 0x00, 2);

    // 进度条
    int barX = 50;
    int barY = height / 2 + 10;
    int barW = width - 100;
    int barH = 10;

    m_display->drawRect(barX, barY, barW, barH, 0x00);
    
    uint32_t elapsed = millis() - m_scanStartTime;
    int fillW = (barW * elapsed) / 3000; // 3秒扫描时间
    if (fillW > barW) fillW = barW;
    
    if (fillW > 0) {
        m_display->fillRect(barX + 1, barY + 1, fillW - 2, barH - 2, 0x00);
    }
}

void WifiActivity::drawNetworkList() {
    int16_t width = m_display->getRotatedWidth();

    int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

    if (m_networkCount == 0) {
        const char* text = "未找到可用网络";
        int16_t textWidth = m_display->getStringWidth(text, 2);
        m_display->drawString((width - textWidth) / 2, 100, text, 0x00, 2);
        return;
    }

    for (int i = 0; i < m_networkCount && i < MAX_NETWORKS; i++) {
        int y = startY + i * LIST_ITEM_HEIGHT;

        bool selected = (i == m_selectedNetwork);

        if (selected) {
            m_display->fillRect(LIST_PADDING, y, width - 2 * LIST_PADDING, LIST_ITEM_HEIGHT - 5, 0x00);
            m_display->drawString(LIST_PADDING + 15, y + 10, m_networkSsids[i], 0xFF, 2);
            
            // 信号强度显示
            char rssiText[16];
            snprintf(rssiText, sizeof(rssiText), "信号: %d", m_networkRssi[i]);
            m_display->drawString(width - 100, y + 15, rssiText, 0xFF, 1);
        } else {
            m_display->drawString(LIST_PADDING + 15, y + 10, m_networkSsids[i], 0x00, 2);
            
            char rssiText[16];
            snprintf(rssiText, sizeof(rssiText), "信号: %d", m_networkRssi[i]);
            m_display->drawString(width - 100, y + 15, rssiText, 0x60, 1);
        }
    }
}

void WifiActivity::drawBottomHint() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    const char* hint;
    if (m_viewMode == VIEW_MAIN) {
        hint = "上下键: 选择  确认: 执行  返回: 返回";
    } else if (m_viewMode == VIEW_NETWORK_LIST) {
        hint = "上下键: 选择  确认: 连接  返回: 返回";
    } else {
        hint = "扫描中...";
    }
    
    int16_t hintWidth = m_display->getStringWidth(hint, 1);
    m_display->drawString((width - hintWidth) / 2, height - 18, hint, 0x60, 1);
}

void WifiActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_VOL_UP:
        case BTN_LEFT:
            if (m_viewMode == VIEW_MAIN) {
                if (m_selectedItem > 0) {
                    m_selectedItem--;
                    requestUpdate();
                }
            } else if (m_viewMode == VIEW_NETWORK_LIST) {
                if (m_selectedNetwork > 0) {
                    m_selectedNetwork--;
                    requestUpdate();
                }
            }
            break;

        case BTN_VOL_DOWN:
        case BTN_RIGHT:
            if (m_viewMode == VIEW_MAIN) {
                if (m_selectedItem < MENU_ITEM_COUNT - 1) {
                    m_selectedItem++;
                    requestUpdate();
                }
            } else if (m_viewMode == VIEW_NETWORK_LIST) {
                if (m_selectedNetwork < m_networkCount - 1) {
                    m_selectedNetwork++;
                    requestUpdate();
                }
            }
            break;

        case BTN_CONFIRM:
            if (m_viewMode == VIEW_MAIN) {
                switch (m_selectedItem) {
                    case MENU_WIFI_TOGGLE:
                        toggleWifi();
                        break;
                    case MENU_SCAN:
                        scanNetworks();
                        break;
                    case MENU_CONNECTED:
                        // 显示当前连接详情
                        requestUpdate();
                        break;
                    case MENU_WEBSERVER:
                        startWebServer();
                        break;
                    case MENU_OTA:
                        checkOta();
                        break;
                }
            } else if (m_viewMode == VIEW_NETWORK_LIST) {
                connectToNetwork(m_selectedNetwork);
            }
            break;

        case BTN_BACK:
            if (m_viewMode == VIEW_NETWORK_LIST) {
                m_viewMode = VIEW_MAIN;
                requestUpdate();
            } else if (m_manager) {
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}

void WifiActivity::handleTouch(TouchEvent event) {
    int16_t width = m_display->getRotatedWidth();

    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;
            int startY = STATUS_BAR_HEIGHT + LIST_PADDING;

            if (m_viewMode == VIEW_MAIN) {
                for (int i = 0; i < MENU_ITEM_COUNT; i++) {
                    int itemY = startY + i * LIST_ITEM_HEIGHT;
                    if (y >= itemY && y < itemY + LIST_ITEM_HEIGHT - 5) {
                        m_selectedItem = i;
                        switch (i) {
                            case MENU_WIFI_TOGGLE: toggleWifi(); break;
                            case MENU_SCAN: scanNetworks(); break;
                            case MENU_WEBSERVER: startWebServer(); break;
                            case MENU_OTA: checkOta(); break;
                        }
                        break;
                    }
                }
            } else if (m_viewMode == VIEW_NETWORK_LIST) {
                for (int i = 0; i < m_networkCount; i++) {
                    int itemY = startY + i * LIST_ITEM_HEIGHT;
                    if (y >= itemY && y < itemY + LIST_ITEM_HEIGHT - 5) {
                        m_selectedNetwork = i;
                        connectToNetwork(i);
                        break;
                    }
                }
            }
            break;
        }

        case GESTURE_SWIPE_RIGHT:
            if (m_viewMode == VIEW_NETWORK_LIST) {
                m_viewMode = VIEW_MAIN;
                requestUpdate();
            } else if (m_manager) {
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}

void WifiActivity::toggleWifi() {
#ifdef ARDUINO
    m_wifiEnabled = !m_wifiEnabled;
    if (m_wifiEnabled) {
        WiFi.mode(WIFI_STA);
    } else {
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
    }
#endif
    requestUpdate();
}

void WifiActivity::scanNetworks() {
#ifdef ARDUINO
    if (!m_wifiEnabled) {
        m_wifiEnabled = true;
        WiFi.mode(WIFI_STA);
    }
    
    m_viewMode = VIEW_SCANNING;
    m_scanning = true;
    m_scanStartTime = millis();
    m_networkCount = 0;
    
    WiFi.scanNetworks(true); // 异步扫描
#endif
    requestUpdate();
}

void WifiActivity::updateScanResults() {
#ifdef ARDUINO
    int n = WiFi.scanComplete();
    if (n >= 0) {
        m_scanning = false;
        m_networkCount = (n < MAX_NETWORKS) ? n : MAX_NETWORKS;
        
        for (int i = 0; i < m_networkCount; i++) {
            String ssid = WiFi.SSID(i);
            strncpy(m_networkSsids[i], ssid.c_str(), sizeof(m_networkSsids[0]) - 1);
            m_networkRssi[i] = WiFi.RSSI(i);
        }
        
        WiFi.scanDelete();
        m_viewMode = VIEW_NETWORK_LIST;
        requestUpdate();
    }
#endif
}

void WifiActivity::connectToNetwork(int index) {
#ifdef ARDUINO
    if (index < 0 || index >= m_networkCount) return;
    
    m_connecting = true;
    m_connectStartTime = millis();
    
    WiFi.begin(m_networkSsids[index], ""); // TODO: 密码输入
    
    // 保存到设置
    if (m_settingsManager) {
        StarReaderSettings* s = m_settingsManager->getSettings();
        strncpy(s->wifiSsid, m_networkSsids[index], sizeof(s->wifiSsid) - 1);
        m_settingsManager->save();
    }
#endif
    requestUpdate();
}

void WifiActivity::startWebServer() {
#ifdef ARDUINO
    if (WiFi.status() == WL_CONNECTED) {
        // TODO: 启动Web服务器
    }
#endif
    requestUpdate();
}

void WifiActivity::checkOta() {
    // TODO: OTA检查更新
    requestUpdate();
}
