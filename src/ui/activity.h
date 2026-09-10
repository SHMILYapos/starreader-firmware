/**
 * StarReader Pro Firmware - Activity Base Class
 * 
 * 类Android的Activity框架，支持子Activity浮层（Modal）
 * Activity-based navigation pattern (similar to Android)
 */

#ifndef STARREADER_UI_ACTIVITY_H
#define STARREADER_UI_ACTIVITY_H

#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/touch.h"
#include "../hal/frontlight.h"
#include "../hal/storage.h"
#include "../hal/power.h"
#include "../utils/settings.h"

// Forward declarations
class ActivityManager;

/**
 * 基础Activity - 所有屏幕的基类
 * Base class for all UI screens (Activities)
 */
class Activity {
public:
    Activity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power)
        : m_display(display)
        , m_input(input)
        , m_touch(nullptr)
        , m_frontLight(nullptr)
        , m_storage(storage)
        , m_power(power)
        , m_settingsManager(nullptr)
        , m_manager(nullptr)
        , m_needsRender(true)
        , m_childActivity(nullptr) {}

    virtual ~Activity() {
        if (m_childActivity) delete m_childActivity;
    }

    // ========== 生命周期 ==========
    virtual void onEnter() = 0;     // 首次进入
    virtual void onExit() = 0;      // 被覆盖退出
    virtual void onResume() = 0;    // 从子Activity返回
    virtual void loop() = 0;        // 主循环
    virtual void render() = 0;      // 渲染

    // ========== 导航 ==========
    void setActivityManager(ActivityManager* manager) { m_manager = manager; }

    // ========== 硬件注入 ==========
    void setTouch(HalTouch* touch) { m_touch = touch; }
    void setFrontLight(HalFrontLight* light) { m_frontLight = light; }
    void setSettingsManager(SettingsManager* settings) { m_settingsManager = settings; }

    // ========== 子Activity浮层（Modal） ==========
    void pushChildActivity(Activity* child) {
        if (m_childActivity) delete m_childActivity;
        m_childActivity = child;
        m_childActivity->setActivityManager(m_manager);
        injectHardware(m_childActivity);
        m_childActivity->onEnter();
    }

    void popChildActivity() {
        if (m_childActivity) {
            m_childActivity->onExit();
            delete m_childActivity;
            m_childActivity = nullptr;
            onResume();  // 子Activity关闭后父Activity恢复
        }
    }

    Activity* getChildActivity() { return m_childActivity; }

    // ========== 渲染控制 ==========
    void requestUpdate() { m_needsRender = true; }
    bool needsRender() { return m_needsRender; }

    // ========== 设置项 ==========
    virtual bool preventAutoSleep() { return false; }

protected:
    HalDisplay* m_display;
    HalInput* m_input;
    HalTouch* m_touch;
    HalFrontLight* m_frontLight;
    HalStorage* m_storage;
    HalPowerManager* m_power;
    SettingsManager* m_settingsManager;
    ActivityManager* m_manager;
    bool m_needsRender;
    Activity* m_childActivity;

    void clearRenderFlag() { m_needsRender = false; }

private:
    void injectHardware(Activity* activity) {
        if (m_touch) activity->setTouch(m_touch);
        if (m_frontLight) activity->setFrontLight(m_frontLight);
        if (m_settingsManager) activity->setSettingsManager(m_settingsManager);
    }
};

/**
 * Activity管理器 - 管理Activity栈和导航
 * Manages activity stack and navigation
 */
class ActivityManager {
public:
    ActivityManager(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power)
        : m_display(display)
        , m_input(input)
        , m_touch(nullptr)
        , m_frontLight(nullptr)
        , m_storage(storage)
        , m_power(power)
        , m_settingsManager(nullptr)
        , m_currentActivity(nullptr)
        , m_previousActivity(nullptr) {}

    ~ActivityManager() {
        if (m_currentActivity) delete m_currentActivity;
        if (m_previousActivity) delete m_previousActivity;
    }

    // Set shared hardware references
    void setTouch(HalTouch* touch) { m_touch = touch; }
    void setFrontLight(HalFrontLight* light) { m_frontLight = light; }
    void setSettingsManager(SettingsManager* settings) { m_settingsManager = settings; }

    // 替换当前Activity（顶层切换，如Home→Library→Settings）
    void startActivity(Activity* activity) {
        if (m_currentActivity) {
            m_currentActivity->onExit();
            delete m_currentActivity;
        }
        m_previousActivity = nullptr;  // 顶层切换不保留栈
        m_currentActivity = activity;
        m_currentActivity->setActivityManager(this);
        injectHardware(m_currentActivity);
        m_currentActivity->onEnter();
    }

    // 压入Activity栈（子页面，如Reader→ChapterList）
    void pushActivity(Activity* activity) {
        if (m_currentActivity) {
            m_currentActivity->onExit();
            m_previousActivity = m_currentActivity;
        }
        m_currentActivity = activity;
        m_currentActivity->setActivityManager(this);
        injectHardware(m_currentActivity);
        m_currentActivity->onEnter();
    }

    // 返回上一层
    void goBack() {
        if (m_previousActivity) {
            if (m_currentActivity) {
                m_currentActivity->onExit();
                delete m_currentActivity;
            }
            m_currentActivity = m_previousActivity;
            m_previousActivity = nullptr;
            m_currentActivity->onResume();
        }
    }

    // 主循环 - 子Activity优先处理事件
    void update() {
        if (m_currentActivity) {
            // 子Activity优先消费事件
            if (m_currentActivity->getChildActivity()) {
                m_currentActivity->getChildActivity()->loop();
            } else {
                m_currentActivity->loop();
            }
        }
    }

    // 渲染 - 先画父Activity，再画子Activity浮层
    void render() {
        if (m_currentActivity) {
            m_currentActivity->render();
            // 子Activity浮层渲染（后续可做半透明遮罩）
            if (m_currentActivity->getChildActivity()) {
                m_currentActivity->getChildActivity()->render();
            }
        }
    }

    Activity* getCurrentActivity() { return m_currentActivity; }

private:
    void injectHardware(Activity* activity) {
        if (m_touch) activity->setTouch(m_touch);
        if (m_frontLight) activity->setFrontLight(m_frontLight);
        if (m_settingsManager) activity->setSettingsManager(m_settingsManager);
    }

    HalDisplay* m_display;
    HalInput* m_input;
    HalTouch* m_touch;
    HalFrontLight* m_frontLight;
    HalStorage* m_storage;
    HalPowerManager* m_power;
    SettingsManager* m_settingsManager;
    Activity* m_currentActivity;
    Activity* m_previousActivity;
};

#endif // STARREADER_UI_ACTIVITY_H
