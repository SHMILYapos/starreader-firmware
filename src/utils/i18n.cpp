/**
 * StarReader Pro Firmware - 国际化双语支持实现
 * Internationalization (i18n) Implementation
 */

#include "i18n.h"

// 静态成员初始化
Language I18n::s_currentLang = LANG_CN;  // 默认中文

// 英文字符串表
const char* I18n::s_enStrings[STR_COUNT] = {
    // ===== 主菜单 =====
    "Continue Reading",     // STR_CONTINUE_READING
    "Library",              // STR_LIBRARY
    "Settings",             // STR_SETTINGS
    "About",                // STR_ABOUT
    "Sleep",                // STR_SLEEP
    "Page",                 // STR_PAGE
    "Battery",              // STR_BATTERY
    "Light",                // STR_LIGHT
    "USB",                  // STR_USB
    "Charging",             // STR_CHARGING

    // ===== 设置界面 =====
    "Settings",             // STR_SETTINGS_TITLE
    "Front Light",          // STR_FRONT_LIGHT
    "Front Light",          // STR_LIGHT_ON
    "Brightness",           // STR_BRIGHTNESS
    "Warmth",               // STR_WARMTH
    "Presets",              // STR_PRESETS
    "Reading",              // STR_PRESET_READING
    "Night",                // STR_PRESET_NIGHT
    "Day",                  // STR_PRESET_DAY

    "Display",              // STR_DISPLAY
    "Font Size",            // STR_FONT_SIZE
    "Small",                // STR_SMALL
    "Medium",               // STR_MEDIUM
    "Large",                // STR_LARGE
    "XLarge",               // STR_XLARGE
    "Orientation",          // STR_ORIENTATION
    "Portrait",             // STR_PORTRAIT
    "Landscape",            // STR_LANDSCAPE
    "Refresh Mode",         // STR_REFRESH_MODE
    "Full",                 // STR_FULL
    "Partial",              // STR_PARTIAL
    "Fast",                 // STR_FAST

    "Reader",               // STR_READER
    "Line Spacing",         // STR_LINE_SPACING
    "Tight",                // STR_TIGHT
    "Normal",               // STR_NORMAL
    "Relaxed",              // STR_RELAXED
    "Justification",        // STR_JUSTIFICATION
    "Left",                 // STR_LEFT
    "Justified",            // STR_JUSTIFIED

    "Power",                // STR_POWER
    "Sleep Timeout",        // STR_SLEEP_TIMEOUT
    "min",                  // STR_MINUTES
    "Never",                // STR_NEVER
    "Auto Refresh",         // STR_AUTO_REFRESH
    "pages",                // STR_PAGES

    "System",               // STR_SYSTEM
    "Language",              // STR_LANGUAGE
    "Version",              // STR_VERSION

    // ===== 阅读器 =====
    "Failed to load file",  // STR_FAILED_LOAD
    "Quick Settings",       // STR_QUICK_SETTINGS
    "Tap: Close",           // STR_TAP_CLOSE
    "Swipe up/down: Adjust",// STR_SWIPE_ADJUST

    // ===== 书库 =====
    "File not found",       // STR_FILE_NOT_FOUND
    "No files found",       // STR_NO_FILES
    "Folder",               // STR_FOLDER
    ".. (Up)",              // STR_UP_DIRECTORY

    // ===== 关于 =====
    "About",                // STR_ABOUT_TITLE
    "Firmware Version",     // STR_FIRMWARE_VERSION
    "Device Model",         // STR_DEVICE_MODEL
    "MCU",                  // STR_MCU
    "Resolution",            // STR_RESOLUTION
    "Author",               // STR_AUTHOR
    "License",              // STR_LICENSE
    "MIT License",          // STR_MIT
    "Community",            // STR_COMMUNITY
    "Back",                 // STR_BACK

    // ===== 提示 =====
    "Saving...",            // STR_SAVING
    "Saved",                // STR_SAVED
    "Welcome",              // STR_WELCOME
    "Boot complete.",       // STR_BOOT_COMPLETE
};

// 中文字符串表
const char* I18n::s_cnStrings[STR_COUNT] = {
    // ===== 主菜单 =====
    "继续阅读",             // STR_CONTINUE_READING
    "书库",                 // STR_LIBRARY
    "设置",                 // STR_SETTINGS
    "关于",                 // STR_ABOUT
    "休眠",                 // STR_SLEEP
    "第",                   // STR_PAGE (后面跟页码)
    "电量",                 // STR_BATTERY
    "前光",                 // STR_LIGHT
    "充电",                 // STR_USB
    "充电中",               // STR_CHARGING

    // ===== 设置界面 =====
    "设置",                 // STR_SETTINGS_TITLE
    "前光设置",             // STR_FRONT_LIGHT
    "前光开关",             // STR_LIGHT_ON
    "亮度",                 // STR_BRIGHTNESS
    "色温",                 // STR_WARMTH
    "预设模式",             // STR_PRESETS
    "阅读模式",             // STR_PRESET_READING
    "夜间模式",             // STR_PRESET_NIGHT
    "白天模式",             // STR_PRESET_DAY

    "显示设置",             // STR_DISPLAY
    "字体大小",             // STR_FONT_SIZE
    "小",                   // STR_SMALL
    "中",                   // STR_MEDIUM
    "大",                   // STR_LARGE
    "特大",                 // STR_XLARGE
    "屏幕方向",             // STR_ORIENTATION
    "竖屏",                 // STR_PORTRAIT
    "横屏",                 // STR_LANDSCAPE
    "刷新模式",             // STR_REFRESH_MODE
    "全刷",                 // STR_FULL
    "局刷",                 // STR_PARTIAL
    "快刷",                 // STR_FAST

    "阅读设置",             // STR_READER
    "行间距",               // STR_LINE_SPACING
    "紧凑",                 // STR_TIGHT
    "标准",                 // STR_NORMAL
    "宽松",                 // STR_RELAXED
    "对齐方式",             // STR_JUSTIFICATION
    "左对齐",               // STR_LEFT
    "两端对齐",             // STR_JUSTIFIED

    "电源设置",             // STR_POWER
    "休眠超时",             // STR_SLEEP_TIMEOUT
    "分钟",                 // STR_MINUTES
    "永不",                 // STR_NEVER
    "自动全刷",             // STR_AUTO_REFRESH
    "页",                   // STR_PAGES

    "系统设置",             // STR_SYSTEM
    "语言",                 // STR_LANGUAGE
    "版本",                 // STR_VERSION

    // ===== 阅读器 =====
    "文件加载失败",         // STR_FAILED_LOAD
    "快速设置",             // STR_QUICK_SETTINGS
    "点击关闭",             // STR_TAP_CLOSE
    "上下滑动调节",         // STR_SWIPE_ADJUST

    // ===== 书库 =====
    "文件不存在",           // STR_FILE_NOT_FOUND
    "暂无文件",             // STR_NO_FILES
    "文件夹",               // STR_FOLDER
    "返回上级",             // STR_UP_DIRECTORY

    // ===== 关于 =====
    "关于设备",             // STR_ABOUT_TITLE
    "固件版本",             // STR_FIRMWARE_VERSION
    "设备型号",             // STR_DEVICE_MODEL
    "主控芯片",             // STR_MCU
    "屏幕分辨率",           // STR_RESOLUTION
    "开发者",               // STR_AUTHOR
    "开源协议",             // STR_LICENSE
    "MIT 开源协议",         // STR_MIT
    "开源社区",             // STR_COMMUNITY
    "返回",                 // STR_BACK

    // ===== 提示 =====
    "保存中...",             // STR_SAVING
    "已保存",               // STR_SAVED
    "欢迎使用",             // STR_WELCOME
    "启动完成。",           // STR_BOOT_COMPLETE
};

const char* I18n::get(StringID id) {
    return get(s_currentLang, id);
}

const char* I18n::get(Language lang, StringID id) {
    if (id < 0 || id >= STR_COUNT) {
        return "";
    }
    
    if (lang == LANG_CN) {
        return s_cnStrings[id];
    } else {
        return s_enStrings[id];
    }
}
