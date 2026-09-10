/**
 * StarReader Pro Firmware - Library Activity Implementation
 * 
 * 文件浏览/书库界面实现
 */

#include "library_activity.h"
#include "txt_reader_activity.h"
#include "../config.h"
#include "../utils/i18n.h"
#include <string.h>

LibraryActivity::LibraryActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                 HalPowerManager* power, HalTouch* touch,
                                 HalFrontLight* frontLight,
                                 SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_touch(touch)
    , m_frontLight(frontLight)
    , m_settingsManager(settingsManager)
    , m_selectedItem(0)
    , m_scrollOffset(0)
    , m_needsRender(true)
    , m_loading(false) {
    strncpy(m_currentPath, "/books", sizeof(m_currentPath) - 1);
    m_currentPath[sizeof(m_currentPath) - 1] = '\0';
}

LibraryActivity::~LibraryActivity() {
}

void LibraryActivity::onEnter() {
    loadDirectory(m_currentPath);
    m_needsRender = true;
}

void LibraryActivity::onExit() {
}

void LibraryActivity::onResume() {
    m_needsRender = true;
}

void LibraryActivity::loop() {
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
}

void LibraryActivity::render() {
    if (!m_needsRender) return;

    m_display->clear(0xFF);
    
    drawStatusBar();
    drawFileList();

    m_display->refresh(HalDisplay::FULL_REFRESH);
    m_needsRender = false;
}

void LibraryActivity::drawStatusBar() {
    int16_t width = m_display->getRotatedWidth();

    // 标题栏
    m_display->fillRect(0, 0, width, 35, 0x00);
    
    // 标题：书库
    const char* title = _(STR_LIBRARY);
    int16_t titleWidth = m_display->getStringWidth(title, 2);
    m_display->drawString((width - titleWidth) / 2, 8, title, 0xFF, 2);

    // 当前路径（右上角）
    int16_t pathWidth = m_display->getStringWidth(m_currentPath, 1);
    if (pathWidth < width - 20) {
        m_display->drawString(width - pathWidth - 10, 12, m_currentPath, 0xFF, 1);
    }
}

void LibraryActivity::drawFileList() {
    if (m_itemCount == 0) {
        int16_t width = m_display->getRotatedWidth();
        int16_t height = m_display->getRotatedHeight();
        const char* msg = _(STR_NO_FILES);
        int16_t msgWidth = m_display->getStringWidth(msg, 2);
        m_display->drawString((width - msgWidth) / 2, height / 2, msg, 0x40, 2);
        return;
    }

    int visibleItems = (m_display->getRotatedHeight() - MENU_START_Y - 30) / MENU_ITEM_HEIGHT;
    
    for (int i = 0; i < visibleItems && (i + m_scrollOffset) < m_itemCount; i++) {
        int itemIdx = i + m_scrollOffset;
        int y = MENU_START_Y + i * MENU_ITEM_HEIGHT;
        
        // 高亮选中项
        if (itemIdx == m_selectedItem) {
            m_display->fillRect(5, y, m_display->getRotatedWidth() - 10, 
                               MENU_ITEM_HEIGHT - 4, 0x00);
            m_display->drawString(15, y + 8, m_items[itemIdx].name, 0xFF, 1);
            
            // 显示类型标识
            if (m_items[itemIdx].isDirectory) {
                m_display->drawString(m_display->getRotatedWidth() - 50, y + 8, 
                                     "[目录]", 0xFF, 1);
            }
        } else {
            m_display->drawString(15, y + 8, m_items[itemIdx].name, 0x00, 1);
            
            if (m_items[itemIdx].isDirectory) {
                m_display->drawString(m_display->getRotatedWidth() - 50, y + 8, 
                                     "[目录]", 0x40, 1);
            }
        }
    }

    // 滚动提示
    if (m_itemCount > visibleItems) {
        char scrollHint[32];
        snprintf(scrollHint, sizeof(scrollHint), "%d/%d", 
                m_selectedItem + 1, m_itemCount);
        int16_t hintWidth = m_display->getStringWidth(scrollHint, 1);
        m_display->drawString((m_display->getRotatedWidth() - hintWidth) / 2, 
                             m_display->getRotatedHeight() - 20, scrollHint, 0x40, 1);
    }
}

void LibraryActivity::loadDirectory(const char* path) {
    strncpy(m_currentPath, path, sizeof(m_currentPath) - 1);
    m_currentPath[sizeof(m_currentPath) - 1] = '\0';
    
    m_itemCount = 0;
    m_selectedItem = 0;
    m_scrollOffset = 0;

    // 添加上级目录
    if (strcmp(path, "/") != 0) {
        strncpy(m_items[m_itemCount].name, "..", sizeof(m_items[m_itemCount].name) - 1);
        m_items[m_itemCount].isDirectory = true;
        m_items[m_itemCount].isTxtFile = false;
        m_itemCount++;
    }

    // 列出目录内容（简化实现 - 实际应该用storage->listDirectory）
    // 这里模拟一些示例文件
    if (strcmp(path, "/books") == 0) {
        const char* sampleFiles[] = {
            "sample.txt",
            "小说.txt",
            "技术文档.txt",
            "学习笔记.txt"
        };
        
        for (int i = 0; i < 4 && m_itemCount < MAX_ITEMS; i++) {
            strncpy(m_items[m_itemCount].name, sampleFiles[i], 
                   sizeof(m_items[m_itemCount].name) - 1);
            m_items[m_itemCount].isDirectory = false;
            m_items[m_itemCount].isTxtFile = true;
            m_itemCount++;
        }
    }
}

void LibraryActivity::handleButton(ButtonState event) {
    switch (event.id) {
        case BTN_VOL_UP:
            if (m_selectedItem > 0) {
                m_selectedItem--;
                // 更新滚动偏移
                if (m_selectedItem < m_scrollOffset) {
                    m_scrollOffset = m_selectedItem;
                }
                m_needsRender = true;
            }
            break;

        case BTN_VOL_DOWN:
            if (m_selectedItem < m_itemCount - 1) {
                m_selectedItem++;
                // 更新滚动偏移
                int visibleItems = (m_display->getRotatedHeight() - MENU_START_Y - 30) / MENU_ITEM_HEIGHT;
                if (m_selectedItem >= m_scrollOffset + visibleItems) {
                    m_scrollOffset = m_selectedItem - visibleItems + 1;
                }
                m_needsRender = true;
            }
            break;

        case BTN_CONFIRM:
            openSelectedItem();
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

void LibraryActivity::handleTouch(TouchEvent event) {
    switch (event.gesture) {
        case GESTURE_TAP: {
            int y = event.endPoint.y;
            if (y >= MENU_START_Y && y < m_display->getRotatedHeight() - 30) {
                int visibleItems = (m_display->getRotatedHeight() - MENU_START_Y - 30) / MENU_ITEM_HEIGHT;
                int idx = (y - MENU_START_Y) / MENU_ITEM_HEIGHT + m_scrollOffset;
                if (idx >= 0 && idx < m_itemCount) {
                    m_selectedItem = idx;
                    m_needsRender = true;
                    // 双击打开
                    openSelectedItem();
                }
            }
            break;
        }

        case GESTURE_SWIPE_UP:
            if (m_selectedItem < m_itemCount - 1) {
                m_selectedItem++;
                int visibleItems = (m_display->getRotatedHeight() - MENU_START_Y - 30) / MENU_ITEM_HEIGHT;
                if (m_selectedItem >= m_scrollOffset + visibleItems) {
                    m_scrollOffset = m_selectedItem - visibleItems + 1;
                }
                m_needsRender = true;
            }
            break;

        case GESTURE_SWIPE_DOWN:
            if (m_selectedItem > 0) {
                m_selectedItem--;
                if (m_selectedItem < m_scrollOffset) {
                    m_scrollOffset = m_selectedItem;
                }
                m_needsRender = true;
            }
            break;

        case GESTURE_SWIPE_RIGHT:
            if (m_manager) {
                m_manager->goBack();
            }
            break;

        default:
            break;
    }
}

void LibraryActivity::openSelectedItem() {
    if (m_itemCount == 0 || m_selectedItem >= m_itemCount) return;

    FileItem* item = &m_items[m_selectedItem];

    if (item->isDirectory) {
        // 进入目录
        char newPath[128];
        if (strcmp(item->name, "..") == 0) {
            // 返回上级目录
            strncpy(newPath, m_currentPath, sizeof(newPath) - 1);
            // 去掉最后一级
            char* lastSlash = strrchr(newPath, '/');
            if (lastSlash && lastSlash != newPath) {
                *lastSlash = '\0';
            } else {
                strcpy(newPath, "/");
            }
        } else {
            // 进入子目录
            snprintf(newPath, sizeof(newPath), "%s/%s", m_currentPath, item->name);
        }
        loadDirectory(newPath);
        m_needsRender = true;
    } else if (item->isTxtFile) {
        // 打开TXT文件
        char fullPath[128];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", m_currentPath, item->name);
        
        TxtReaderActivity* reader = new TxtReaderActivity(
            m_display, m_input, m_storage, m_power,
            fullPath, m_touch, m_frontLight, m_settingsManager);
        m_manager->pushActivity(reader);
    }
}

bool LibraryActivity::isTxtFile(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return false;
    return (strcasecmp(ext, ".txt") == 0);
}
