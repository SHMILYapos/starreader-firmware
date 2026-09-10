/**
 * StarReader Pro Firmware - Sleep Screen Implementation
 * 
 * 睡眠屏幕/待机表盘实现
 */

#include "sleep_screen_activity.h"
#include "../config.h"
#include <string.h>
#include <cstdio>
#include <Arduino.h>

SleepScreenActivity::SleepScreenActivity(HalDisplay* display, HalInput* input, HalStorage* storage, 
                                         HalPowerManager* power, HalTouch* touch,
                                         HalFrontLight* frontLight,
                                         SettingsManager* settingsManager)
    : Activity(display, input, storage, power)
    , m_watchfaceType(0)
    , m_touched(false)
    , m_lastUpdate(0)
    , m_hour(12)
    , m_minute(0)
    , m_day(1)
    , m_month(1)
    , m_year(2026)
    , m_weekday(1) {
}

SleepScreenActivity::~SleepScreenActivity() {
}

void SleepScreenActivity::onEnter() {
    m_touched = false;
    updateTime();
    requestUpdate();
}

void SleepScreenActivity::onExit() {
}

void SleepScreenActivity::onResume() {
    requestUpdate();
}

void SleepScreenActivity::loop() {
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

    // 每分钟更新一次时间
    if (millis() - m_lastUpdate > 60000) {
        updateTime();
        requestUpdate();
    }
}

void SleepScreenActivity::render() {
    if (!needsRender()) return;

    m_display->clear(0xFF);

    switch (m_watchfaceType) {
        case WATCHFACE_CLOCK:
            drawClockWatchface();
            break;
        case WATCHFACE_BOOK:
            drawBookWatchface();
            break;
        case WATCHFACE_CALENDAR:
            drawCalendarWatchface();
            break;
    }

    drawBatteryIndicator();

    // 睡眠屏幕用局刷，减少闪烁
    m_display->refresh(HalDisplay::PARTIAL_REFRESH);
    clearRenderFlag();
}

void SleepScreenActivity::drawClockWatchface() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // 时间显示 - 大字体
    char timeText[16];
    snprintf(timeText, sizeof(timeText), "%02d:%02d", m_hour, m_minute);
    
    // 模拟大字体居中显示
    int timeX = (width - 100) / 2;
    int timeY = height / 2 - 40;
    m_display->drawString(timeX, timeY, timeText, 0x00, 4);

    // 日期显示
    char dateText[32];
    snprintf(dateText, sizeof(dateText), "%d年%d月%d日", m_year, m_month, m_day);
    int dateWidth = m_display->getStringWidth(dateText, 2);
    m_display->drawString((width - dateWidth) / 2, timeY + 60, dateText, 0x40, 2);

    // 星期显示
    const char* weekdays[] = {"日", "一", "二", "三", "四", "五", "六"};
    char weekText[16];
    snprintf(weekText, sizeof(weekText), "星期%s", weekdays[m_weekday]);
    int weekWidth = m_display->getStringWidth(weekText, 2);
    m_display->drawString((width - weekWidth) / 2, timeY + 90, weekText, 0x40, 2);
}

void SleepScreenActivity::drawBookWatchface() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // 显示当前阅读的书籍信息
    const char* bookTitle = "《星舰纪元》";
    const char* bookAuthor = "作者：佚名";
    
    // 书籍标题
    int titleWidth = m_display->getStringWidth(bookTitle, 3);
    m_display->drawString((width - titleWidth) / 2, height / 2 - 40, bookTitle, 0x00, 3);
    
    // 作者
    int authorWidth = m_display->getStringWidth(bookAuthor, 2);
    m_display->drawString((width - authorWidth) / 2, height / 2, bookAuthor, 0x40, 2);

    // 阅读进度
    const char* progress = "阅读进度: 45%";
    int progressWidth = m_display->getStringWidth(progress, 2);
    m_display->drawString((width - progressWidth) / 2, height / 2 + 40, progress, 0x60, 2);

    // 进度条
    int barX = (width - 200) / 2;
    int barY = height / 2 + 70;
    m_display->drawRect(barX, barY, 200, 8, 0x00);
    m_display->fillRect(barX + 1, barY + 1, 200 * 0.45 - 2, 6, 0x00);
}

void SleepScreenActivity::drawCalendarWatchface() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // 月历显示
    char monthText[16];
    snprintf(monthText, sizeof(monthText), "%d年%d月", m_year, m_month);
    int monthWidth = m_display->getStringWidth(monthText, 3);
    m_display->drawString((width - monthWidth) / 2, height / 2 - 60, monthText, 0x00, 3);

    // 大号日期
    char dayText[8];
    snprintf(dayText, sizeof(dayText), "%d", m_day);
    int dayWidth = m_display->getStringWidth(dayText, 5);
    m_display->drawString((width - dayWidth) / 2, height / 2 - 10, dayText, 0x00, 5);

    // 星期
    const char* weekdays[] = {"日", "一", "二", "三", "四", "五", "六"};
    char weekText[16];
    snprintf(weekText, sizeof(weekText), "星期%s", weekdays[m_weekday]);
    int weekWidth = m_display->getStringWidth(weekText, 2);
    m_display->drawString((width - weekWidth) / 2, height / 2 + 70, weekText, 0x40, 2);

    // 农历（模拟）
    const char* lunar = "农历 八月十五";
    int lunarWidth = m_display->getStringWidth(lunar, 2);
    m_display->drawString((width - lunarWidth) / 2, height / 2 + 100, lunar, 0x60, 2);
}

void SleepScreenActivity::drawBatteryIndicator() {
    int16_t width = m_display->getRotatedWidth();
    int16_t height = m_display->getRotatedHeight();

    // 右上角电量显示
    uint8_t batteryPct = m_power->getBatteryPercentage();
    char batText[16];
    snprintf(batText, sizeof(batText), "电量: %d%%", batteryPct);
    m_display->drawString(width - 80, 10, batText, 0x40, 1);
}

void SleepScreenActivity::handleButton(ButtonState event) {
    wakeUp();
}

void SleepScreenActivity::handleTouch(TouchEvent event) {
    wakeUp();
}

void SleepScreenActivity::updateTime() {
    m_lastUpdate = millis();
    // TODO: 从RTC读取真实时间
    // 这里用模拟数据
    m_minute++;
    if (m_minute >= 60) {
        m_minute = 0;
        m_hour++;
        if (m_hour >= 24) {
            m_hour = 0;
        }
    }
}

void SleepScreenActivity::wakeUp() {
    // 唤醒设备，回到主页
    if (m_manager) {
        // 返回到主页
        m_manager->goBack();
    }
}
