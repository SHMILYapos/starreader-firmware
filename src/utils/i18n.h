/**
 * StarReader Pro Firmware - 国际化双语支持
 * Internationalization (i18n) - Bilingual Chinese/English support
 * 
 * 支持中文(简体)和英文双语界面
 * Language: 0=English, 1=中文
 */

#ifndef STARREADER_UTILS_I18N_H
#define STARREADER_UTILS_I18N_H

#include <stdint.h>

// 语言枚举
enum Language {
    LANG_EN = 0,    // English
    LANG_CN = 1     // 中文
};

// 字符串ID枚举 - 所有界面文本
enum StringID {
    // ===== 主菜单 =====
    STR_CONTINUE_READING = 0,   // 继续阅读 / Continue Reading
    STR_LIBRARY,                // 书库 / Library
    STR_SETTINGS,               // 设置 / Settings
    STR_ABOUT,                  // 关于 / About
    STR_SLEEP,                  // 休眠 / Sleep
    STR_PAGE,                   // 第 X 页 / Page X
    STR_BATTERY,                // 电量 / Battery
    STR_LIGHT,                  // 前光 / Light
    STR_USB,                    // 充电 / USB
    STR_CHARGING,               // 充电中 / Charging

    // ===== 设置界面 =====
    STR_SETTINGS_TITLE,         // 设置 / Settings
    STR_FRONT_LIGHT,            // 前光设置 / Front Light
    STR_LIGHT_ON,               // 前光开关 / Front Light
    STR_BRIGHTNESS,             // 亮度 / Brightness
    STR_WARMTH,                 // 色温 / Warmth
    STR_PRESETS,                // 预设 / Presets
    STR_PRESET_READING,         // 阅读模式 / Reading
    STR_PRESET_NIGHT,           // 夜间模式 / Night
    STR_PRESET_DAY,             // 白天模式 / Day

    STR_DISPLAY,                // 显示设置 / Display
    STR_FONT_SIZE,              // 字体大小 / Font Size
    STR_SMALL,                  // 小 / Small
    STR_MEDIUM,                 // 中 / Medium
    STR_LARGE,                  // 大 / Large
    STR_XLARGE,                 // 特大 / XLarge
    STR_ORIENTATION,            // 屏幕方向 / Orientation
    STR_PORTRAIT,               // 竖屏 / Portrait
    STR_LANDSCAPE,              // 横屏 / Landscape
    STR_REFRESH_MODE,           // 刷新模式 / Refresh Mode
    STR_FULL,                   // 全刷 / Full
    STR_PARTIAL,                // 局刷 / Partial
    STR_FAST,                   // 快刷 / Fast

    STR_READER,                 // 阅读设置 / Reader
    STR_LINE_SPACING,           // 行间距 / Line Spacing
    STR_TIGHT,                  // 紧凑 / Tight
    STR_NORMAL,                 // 标准 / Normal
    STR_RELAXED,                // 宽松 / Relaxed
    STR_JUSTIFICATION,          // 对齐方式 / Justification
    STR_LEFT,                   // 左对齐 / Left
    STR_JUSTIFIED,              // 两端对齐 / Justified

    STR_POWER,                  // 电源设置 / Power
    STR_SLEEP_TIMEOUT,          // 休眠超时 / Sleep Timeout
    STR_MINUTES,                // 分钟 / min
    STR_NEVER,                  // 永不 / Never
    STR_AUTO_REFRESH,           // 自动全刷 / Auto Refresh
    STR_PAGES,                  // 页 / pages

    STR_SYSTEM,                 // 系统设置 / System
    STR_LANGUAGE,               // 语言 / Language
    STR_VERSION,                // 版本 / Version

    // ===== 阅读器 =====
    STR_FAILED_LOAD,            // 文件加载失败 / Failed to load file
    STR_QUICK_SETTINGS,         // 快速设置 / Quick Settings
    STR_TAP_CLOSE,              // 点击关闭 / Tap: Close
    STR_SWIPE_ADJUST,           // 滑动调节 / Swipe up/down: Adjust

    // ===== 书库 =====
    STR_FILE_NOT_FOUND,         // 文件不存在 / File not found
    STR_NO_FILES,               // 暂无文件 / No files found
    STR_FOLDER,                 // 文件夹 / Folder
    STR_UP_DIRECTORY,           // 返回上级 / .. (Up)

    // ===== 关于 =====
    STR_ABOUT_TITLE,            // 关于设备 / About
    STR_FIRMWARE_VERSION,       // 固件版本 / Firmware Version
    STR_DEVICE_MODEL,           // 设备型号 / Device Model
    STR_MCU,                    // 主控芯片 / MCU
    STR_RESOLUTION,             // 屏幕分辨率 / Resolution
    STR_AUTHOR,                 // 开发者 / Author
    STR_LICENSE,                // 开源协议 / License
    STR_MIT,                    // MIT License
    STR_COMMUNITY,              // 社区 / Community
    STR_BACK,                   // 返回 / Back

    // ===== 提示 =====
    STR_SAVING,                 // 保存中... / Saving...
    STR_SAVED,                  // 已保存 / Saved
    STR_WELCOME,                // 欢迎使用 / Welcome
    STR_BOOT_COMPLETE,          // 启动完成 / Boot complete

    STR_COUNT
};

// 国际化管理器
class I18n {
public:
    // 设置当前语言
    static void setLanguage(Language lang) { s_currentLang = lang; }
    
    // 获取当前语言
    static Language getLanguage() { return s_currentLang; }
    
    // 获取字符串（根据当前语言）
    static const char* get(StringID id);
    
    // 获取指定语言的字符串
    static const char* get(Language lang, StringID id);
    
    // 切换语言
    static void toggleLanguage() {
        s_currentLang = (s_currentLang == LANG_EN) ? LANG_CN : LANG_EN;
    }

private:
    static Language s_currentLang;
    
    // 英文字符串表
    static const char* s_enStrings[STR_COUNT];
    
    // 中文字符串表
    static const char* s_cnStrings[STR_COUNT];
};

// 便捷宏 - 获取当前语言字符串
#define _(id) I18n::get(id)

#endif // STARREADER_UTILS_I18N_H
