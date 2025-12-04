/**
 * xl9535_relay.h
 * XL9535 I2C Relay Board Controller
 * For 8-channel relay module with XL9535 I/O expander
 */

#ifndef XL9535_RELAY_H
#define XL9535_RELAY_H

#include <Arduino.h>
#include <Wire.h>

// XL9535 Register addresses
#define XL9535_INPUT_PORT_0_REG     0x00
#define XL9535_INPUT_PORT_1_REG     0x01
#define XL9535_OUTPUT_PORT_0_REG    0x02
#define XL9535_OUTPUT_PORT_1_REG    0x03
#define XL9535_INVERSION_PORT_0_REG 0x04
#define XL9535_INVERSION_PORT_1_REG 0x05
#define XL9535_CONFIG_PORT_0_REG    0x06
#define XL9535_CONFIG_PORT_1_REG    0x07

// Default I2C address (can be changed with DIP switches)
#define XL9535_DEFAULT_ADDRESS      0x20

class XL9535Relay {
public:
    /**
     * Initialize the XL9535 relay board
     * @param address I2C address (default 0x20, can be 0x20-0x27)
     * @param sda SDA pin for I2C (default: use board default)
     * @param scl SCL pin for I2C (default: use board default)
     */
    bool begin(uint8_t address = XL9535_DEFAULT_ADDRESS, int sda = -1, int scl = -1);
    
    /**
     * Turn on a specific relay
     * @param relay Relay number (0-7 for 8-channel board)
     * @return true if successful
     */
    bool relayOn(uint8_t relay);
    
    /**
     * Turn off a specific relay
     * @param relay Relay number (0-7 for 8-channel board)
     * @return true if successful
     */
    bool relayOff(uint8_t relay);
    
    /**
     * Set relay state (on/off)
     * @param relay Relay number (0-7)
     * @param state true = ON, false = OFF
     * @return true if successful
     */
    bool setRelay(uint8_t relay, bool state);
    
    /**
     * Toggle relay state
     * @param relay Relay number (0-7)
     * @return true if successful
     */
    bool toggleRelay(uint8_t relay);
    
    /**
     * Turn on all relays
     * @return true if successful
     */
    bool allOn();
    
    /**
     * Turn off all relays
     * @return true if successful
     */
    bool allOff();
    
    /**
     * Set multiple relays at once using a bitmask
     * @param mask 8-bit mask (bit 0 = relay 0, bit 7 = relay 7)
     *             1 = ON, 0 = OFF
     * @return true if successful
     */
    bool setRelayMask(uint8_t mask);
    
    /**
     * Get current relay states as bitmask
     * @return 8-bit mask of relay states
     */
    uint8_t getRelayStates();
    
    /**
     * Check if a specific relay is on
     * @param relay Relay number (0-7)
     * @return true if relay is ON
     */
    bool isRelayOn(uint8_t relay);
    
    /**
     * Test I2C connection to the board
     * @return true if board is responding
     */
    bool testConnection();
    
private:
    uint8_t i2cAddress;
    uint8_t relayStates;  // Current state of all relays (bit 0-7)
    bool initialized;
    
    // Read from XL9535 register
    bool readRegister(uint8_t reg, uint8_t* value);
    
    // Write to XL9535 register
    bool writeRegister(uint8_t reg, uint8_t value);
    
    // Update relay states in the chip
    bool updateRelays();
};

// Global instance
extern XL9535Relay relayBoard;

// ============================================================
// C-Compatible Wrapper Functions (for use in .c files)
// ============================================================
#ifdef __cplusplus
extern "C" {
#endif

// Simple C functions that wrap the C++ class methods
void relay_on(uint8_t relay);
void relay_off(uint8_t relay);
void relay_toggle(uint8_t relay);
void relay_all_on(void);
void relay_all_off(void);
bool relay_is_on(uint8_t relay);

#ifdef __cplusplus
}
#endif

#endif // XL9535_RELAY_H