/**
 * StarReader Firmware - Activity Base Class
 * 
 * Activity-based navigation pattern (similar to Android)
 * Each screen is an Activity with well-defined lifecycle
 */

#ifndef STARREADER_UI_ACTIVITY_H
#define STARREADER_UI_ACTIVITY_H

#include "../hal/display.h"
#include "../hal/input.h"
#include "../hal/storage.h"
#include "../hal/power.h"

// Forward declarations
class ActivityManager;

/**
 * Base class for all UI screens (Activities)
 */
class Activity {
public:
    Activity(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power)
        : m_display(display)
        , m_input(input)
        , m_storage(storage)
        , m_power(power)
        , m_manager(nullptr)
        , m_initialized(false) {}

    virtual ~Activity() {}

    // Lifecycle methods
    virtual void onEnter() = 0;     // Called when activity becomes active
    virtual void onExit() = 0;      // Called when activity is paused/stopped
    virtual void onResume() = 0;    // Called when returning to this activity
    virtual void loop() = 0;        // Called frequently for updates
    virtual void render() = 0;      // Called to draw the UI

    // Navigation
    void setActivityManager(ActivityManager* manager) { m_manager = manager; }

    // Settings
    virtual bool preventAutoSleep() { return false; }

protected:
    HalDisplay* m_display;
    HalInput* m_input;
    HalStorage* m_storage;
    HalPowerManager* m_power;
    ActivityManager* m_manager;
    bool m_initialized;
};

/**
 * Manages activity stack and navigation
 */
class ActivityManager {
public:
    ActivityManager(HalDisplay* display, HalInput* input, HalStorage* storage, HalPowerManager* power)
        : m_display(display)
        , m_input(input)
        , m_storage(storage)
        , m_power(power)
        , m_currentActivity(nullptr)
        , m_previousActivity(nullptr) {}

    ~ActivityManager() {
        if (m_currentActivity) delete m_currentActivity;
        if (m_previousActivity) delete m_previousActivity;
    }

    // Start a new activity (replaces current)
    void startActivity(Activity* activity) {
        if (m_currentActivity) {
            m_currentActivity->onExit();
            delete m_currentActivity;
        }

        m_currentActivity = activity;
        m_currentActivity->setActivityManager(this);
        m_currentActivity->onEnter();
    }

    // Go back to previous activity
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

    // Push activity onto stack (keep previous)
    void pushActivity(Activity* activity) {
        if (m_currentActivity) {
            m_currentActivity->onExit();
            m_previousActivity = m_currentActivity;
        }
        m_currentActivity = activity;
        m_currentActivity->setActivityManager(this);
        m_currentActivity->onEnter();
    }

    // Main loop
    void update() {
        if (m_currentActivity) {
            m_currentActivity->loop();
        }
    }

    void render() {
        if (m_currentActivity) {
            m_currentActivity->render();
        }
    }

    Activity* getCurrentActivity() { return m_currentActivity; }

private:
    HalDisplay* m_display;
    HalInput* m_input;
    HalStorage* m_storage;
    HalPowerManager* m_power;
    Activity* m_currentActivity;
    Activity* m_previousActivity;
};

#endif // STARREADER_UI_ACTIVITY_H
