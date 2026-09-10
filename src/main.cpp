/**
 * StarReader Pro Firmware - Main Entry Point
 * 
 * An open-source e-reader firmware for Xteink X4 Pro
 * 
 * Hardware: ESP32-C3 + 4.3" E-Ink (800x480) + Touch + Front Light
 * 
 * Author: StarReader Community
 * License: MIT
 */

#include <Arduino.h>
#include "config.h"
#include "hal/display.h"
#include "hal/input.h"
#include "hal/touch.h"
#include "hal/frontlight.h"
#include "hal/storage.h"
#include "hal/power.h"
#include "ui/activity.h"
#include "ui/home_activity.h"
#include "utils/settings.h"

// Global objects
static HalDisplay* display = nullptr;
static HalInput* input = nullptr;
static HalTouch* touch = nullptr;
static HalFrontLight* frontLight = nullptr;
static HalStorage* storage = nullptr;
static HalPowerManager* powerManager = nullptr;
static ActivityManager* activityManager = nullptr;
static SettingsManager* settingsManager = nullptr;

// Timing
static uint32_t lastActivityTime = 0;
static uint32_t loopCount = 0;

// Forward declarations
void initializeHardware();
void createActivities();
void checkAutoSleep();

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    delay(100);

    Serial.println();
    Serial.println("===================================");
    Serial.println("  StarReader Pro Firmware v0.2.0  ");
    Serial.println("  for Xteink X4 Pro               ");
    Serial.println("===================================");
    Serial.println();

    // Initialize hardware
    initializeHardware();

    // Create activity manager and start home activity
    createActivities();

    lastActivityTime = millis();
    loopCount = 0;

    Serial.println("Boot complete.");
}

void loop() {
    // Update touch input
    if (touch) {
        touch->update();
    }

    // Update activity
    if (activityManager) {
        activityManager->update();
        activityManager->render();
    }

    // Check for auto sleep
    checkAutoSleep();

    // Small delay to prevent watchdog issues
    delay(10);

    loopCount++;
}

void initializeHardware() {
    Serial.println("Initializing hardware...");

    // Display
    display = new HalDisplay();
    if (!display->begin()) {
        Serial.println("ERROR: Display initialization failed!");
    } else {
        Serial.println("Display initialized: 800x480");
    }

    // Physical buttons
    input = new HalInput();
    if (!input->begin()) {
        Serial.println("ERROR: Input initialization failed!");
    } else {
        Serial.println("Physical buttons initialized");
    }

    // Touchscreen (X4 Pro)
    touch = new HalTouch();
    if (!touch->begin()) {
        Serial.println("WARNING: Touchscreen not detected");
    } else {
        Serial.println("Touchscreen initialized (GT911)");
    }

    // Front light (X4 Pro)
    frontLight = new HalFrontLight();
    if (!frontLight->begin()) {
        Serial.println("WARNING: Front light initialization failed");
    } else {
        Serial.println("Front light initialized (dual-tone)");
        Serial.printf("  Brightness: %d%%, Warmth: %d%%\n",
                     frontLight->getBrightness(),
                     frontLight->getWarmth());
    }

    // Storage (SD card)
    storage = new HalStorage();
    if (!storage->begin()) {
        Serial.println("WARNING: SD card not found - running in limited mode");
    } else {
        Serial.println("SD card mounted");
        Serial.printf("Card size: %llu MB\n", storage->cardSizeMB());
    }

    // Power management
    powerManager = new HalPowerManager();
    if (!powerManager->begin()) {
        Serial.println("ERROR: Power manager initialization failed!");
    } else {
        Serial.println("Power manager initialized");
        Serial.printf("Battery: %.2fV (%d%%)\n",
                     powerManager->getBatteryVoltage(),
                     powerManager->getBatteryPercentage());
    }

    // Settings
    settingsManager = new SettingsManager(storage);
    if (settingsManager->load()) {
        Serial.println("Settings loaded from SD card");

        // Apply saved settings
        HalDisplay::Orientation orient = (HalDisplay::Orientation)settingsManager->getSettings()->orientation;
        display->setOrientation(orient);

        frontLight->setBrightness(settingsManager->getSettings()->brightness);
        frontLight->setWarmth(settingsManager->getSettings()->frontLightWarmth);
    } else {
        Serial.println("Using default settings");
        settingsManager->save();
    }

    Serial.println("Hardware initialization complete.");
}

void createActivities() {
    Serial.println("Creating activities...");

    activityManager = new ActivityManager(display, input, storage, powerManager);
    
    // Inject shared hardware references
    activityManager->setTouch(touch);
    activityManager->setFrontLight(frontLight);
    activityManager->setSettingsManager(settingsManager);

    // Start with home activity
    HomeActivity* home = new HomeActivity(display, input, storage, powerManager,
                                          touch, frontLight, settingsManager);
    activityManager->startActivity(home);

    Serial.println("Home activity started.");
}

void checkAutoSleep() {
    if (!powerManager || !activityManager) return;

    // Check if current activity prevents sleep
    Activity* current = activityManager->getCurrentActivity();
    if (current && current->preventAutoSleep()) {
        lastActivityTime = millis();
        return;
    }

    // Check for button activity
    input->update();
    ButtonState event = input->getLastEvent();

    if (event.event != EVENT_NONE) {
        lastActivityTime = millis();
    }

    // Check for touch activity
    if (touch) {
        TouchEvent touchEvent = touch->getLastEvent();
        if (touchEvent.gesture != GESTURE_NONE) {
            lastActivityTime = millis();
        }
    }

    // Check timeout
    uint32_t timeout = settingsManager ?
        settingsManager->getSettings()->sleepTimeoutSec * 1000 :
        AUTO_SLEEP_TIMEOUT_MS;

    if (millis() - lastActivityTime > timeout) {
        Serial.println("Auto-sleep timeout reached. Entering deep sleep...");

        // Save state
        if (settingsManager) {
            settingsManager->save();
        }

        // Turn off front light before sleep
        if (frontLight) {
            frontLight->off();
        }

        // Enter deep sleep
        powerManager->enterDeepSleep();
    }
}
