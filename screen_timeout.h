/**
 * screen_timeout.h
 * Screen timeout manager for automatic brightness control
 */

#ifndef SCREEN_TIMEOUT_H
#define SCREEN_TIMEOUT_H

#include <Arduino.h>

class ScreenTimeout {
public:
    /**
     * Initialize the screen timeout manager
     * @param timeoutMs Timeout duration in milliseconds (default: 30000ms = 30s)
     * @param dimBrightness Brightness when dimmed (default: 0)
     * @param activeBrightness Brightness when active (default: 255)
     */
    void begin(uint32_t timeoutMs = 30000, uint8_t dimBrightness = 0, uint8_t activeBrightness = 255);
    
    /**
     * Update the timeout manager - call this in loop()
     */
    void update();
    
    /**
     * Reset the timeout timer (call when user interaction detected)
     */
    void resetTimeout();
    
    /**
     * Check if screen is currently dimmed
     * @return true if screen is dimmed, false if active
     */
    bool isDimmed() const;
    
    /**
     * Set the timeout duration
     * @param timeoutMs Timeout in milliseconds
     */
    void setTimeout(uint32_t timeoutMs);
    
    /**
     * Set brightness levels
     * @param dimBrightness Brightness when dimmed (0-255)
     * @param activeBrightness Brightness when active (0-255)
     */
    void setBrightnessLevels(uint8_t dimBrightness, uint8_t activeBrightness);
    
private:
    uint32_t lastActivityTime;
    uint32_t timeoutDuration;
    uint8_t dimBrightness;
    uint8_t activeBrightness;
    bool screenDimmed;
    
    void setBacklight(uint8_t brightness);
};

// Global instance
extern ScreenTimeout screenTimeout;

#endif // SCREEN_TIMEOUT_H



