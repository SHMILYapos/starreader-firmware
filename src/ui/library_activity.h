/**
 * StarReader Pro Firmware - Library Activity (File Browser)
 * 
 * 文件浏览/书库界面
 * Browse SD card for TXT files
 */

#ifndef STARREADER_UI_LIBRARY_ACTIVITY_H
#define STARREADER_UI_LIBRARY_ACTIVITY_H

#include "activity.h"
#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

class LibraryActivity : public Activity {
public:
    LibraryActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                    HalPowerManager* power, HalTouch* touch = nullptr,
                    HalFrontLight* frontLight = nullptr,
                    SettingsManager* settingsManager = nullptr);
    ~LibraryActivity() override;

    void onEnter() override;
    void onExit() override;
    void onResume() override;
    void loop() override;
    void render() override;

private:
    static const int MAX_ITEMS = 20;
    static const int MENU_START_Y = 50;
    static const int MENU_ITEM_HEIGHT = 32;
    
    struct FileItem {
        char name[64];
        bool isDirectory;
        bool isTxtFile;
    };

    // Hardware references
    HalTouch* m_touch;
    HalFrontLight* m_frontLight;
    SettingsManager* m_settingsManager;

    // File browser state
    char m_currentPath[128];
    FileItem m_items[MAX_ITEMS];
    int m_itemCount;
    int m_selectedItem;
    int m_scrollOffset;
    bool m_needsRender;
    bool m_loading;

    // Methods
    void loadDirectory(const char* path);
    void drawFileList();
    void drawStatusBar();
    void handleButton(ButtonState event);
    void handleTouch(TouchEvent event);
    void openSelectedItem();
    bool isTxtFile(const char* filename);
};

#endif // STARREADER_UI_LIBRARY_ACTIVITY_H
