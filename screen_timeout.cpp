/**
 * screen_timeout.cpp
 * Implementation of screen timeout manager
 */

#include "screen_timeout.h"
#include "ui.h"


// Global instance
ScreenTimeout screenTimeout;

void ScreenTimeout::begin(uint32_t timeoutMs, uint8_t dimBright, uint8_t activeBright) {
    timeoutDuration = timeoutMs;
    dimBrightness = dimBright;
    activeBrightness = activeBright;
    screenDimmed = false;
    lastActivityTime = millis();
    
    // Set initial brightness to active level
    setBacklight(activeBrightness);
}

void ScreenTimeout::update() {
    uint32_t currentTime = millis();
    
    // Check if timeout has occurred
    if (!screenDimmed && (currentTime - lastActivityTime >= timeoutDuration)) {
        // Dim the screen
        screenDimmed = true;
        setBacklight(dimBrightness);
        Serial.println("Screen dimmed due to inactivity");

        //Vracamo se na meni obavezno!
        _ui_screen_change(&ui_meni, LV_SCR_LOAD_ANIM_NONE, 5, 0, &ui_meni_screen_init);
    }
}

void ScreenTimeout::resetTimeout() {
    lastActivityTime = millis();
    
    // If screen was dimmed, wake it up
    if (screenDimmed) {
        screenDimmed = false;
        setBacklight(activeBrightness);
        Serial.println("Screen woken up");
    }
}

bool ScreenTimeout::isDimmed() const {
    return screenDimmed;
}

void ScreenTimeout::setTimeout(uint32_t timeoutMs) {
    timeoutDuration = timeoutMs;
}

void ScreenTimeout::setBrightnessLevels(uint8_t dimBright, uint8_t activeBright) {
    dimBrightness = dimBright;
    activeBrightness = activeBright;
    
    // Update current brightness if needed
    if (screenDimmed) {
        setBacklight(dimBrightness);
    } else {
        setBacklight(activeBrightness);
    }
}

void ScreenTimeout::setBacklight(uint8_t brightness) {
    // The backlight pin is GPIO 2 (as defined in gfx_conf.h)
    // ESP32 PWM setup for backlight control
    
    // Use LEDC (LED Control) peripheral for PWM
    static bool pwmInitialized = false;
    const int pwmChannel = 0;
    const int pwmFreq = 5000;  // 5 kHz
    const int pwmResolution = 8;  // 8-bit resolution (0-255)
    const int backlightPin = 2;
    
    if (!pwmInitialized) {
        ledcSetup(pwmChannel, pwmFreq, pwmResolution);
        ledcAttachPin(backlightPin, pwmChannel);
        pwmInitialized = true;
    }
    
    // Set the PWM duty cycle (brightness)
    ledcWrite(pwmChannel, brightness);
}



