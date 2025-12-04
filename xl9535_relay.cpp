/**
 * xl9535_relay.cpp
 * Implementation of XL9535 I2C Relay Board Controller
 */

#include "xl9535_relay.h"

// Global instance
XL9535Relay relayBoard;

bool XL9535Relay::begin(uint8_t address, int sda, int scl) {
    i2cAddress = address;
    relayStates = 0x00;  // Start with all relays OFF
    initialized = false;
    
    // Initialize I2C with custom pins if provided
    if (sda != -1 && scl != -1) {
        Wire.begin(sda, scl);
        Serial.print("I2C initialized on SDA=");
        Serial.print(sda);
        Serial.print(", SCL=");
        Serial.println(scl);
    } else {
        // I2C already initialized by display system
        Serial.println("Using existing I2C initialization");
    }
    
    // Test connection
    if (!testConnection()) {
        Serial.print("ERROR: XL9535 relay board not found at address 0x");
        Serial.println(i2cAddress, HEX);
        return false;
    }
    
    // Configure Port 0 (relays 0-7) as outputs
    // 0 = output, 1 = input
    if (!writeRegister(XL9535_CONFIG_PORT_0_REG, 0x00)) {
        Serial.println("ERROR: Failed to configure XL9535 Port 0");
        return false;
    }
    
    // Turn off all relays initially
    if (!allOff()) {
        Serial.println("ERROR: Failed to initialize relay states");
        return false;
    }
    
    initialized = true;
    Serial.println("XL9535 relay board initialized successfully!");
    Serial.print("I2C Address: 0x");
    Serial.println(i2cAddress, HEX);
    
    return true;
}

bool XL9535Relay::relayOn(uint8_t relay) {
    return setRelay(relay, true);
}

bool XL9535Relay::relayOff(uint8_t relay) {
    return setRelay(relay, false);
}

bool XL9535Relay::setRelay(uint8_t relay, bool state) {
    if (relay > 7) {
        Serial.print("ERROR: Invalid relay number: ");
        Serial.println(relay);
        return false;
    }
    
    if (state) {
        // Turn ON: set bit to 1
        relayStates |= (1 << relay);
    } else {
        // Turn OFF: set bit to 0
        relayStates &= ~(1 << relay);
    }
    
    bool success = updateRelays();
    
    if (success) {
        Serial.print("Relay ");
        Serial.print(relay);
        Serial.print(" turned ");
        Serial.println(state ? "ON" : "OFF");
    }
    
    return success;
}

bool XL9535Relay::toggleRelay(uint8_t relay) {
    if (relay > 7) {
        Serial.print("ERROR: Invalid relay number: ");
        Serial.println(relay);
        return false;
    }
    
    // Toggle the bit
    relayStates ^= (1 << relay);
    
    bool success = updateRelays();
    
    if (success) {
        Serial.print("Relay ");
        Serial.print(relay);
        Serial.println(" toggled");
    }
    
    return success;
}

bool XL9535Relay::allOn() {
    relayStates = 0xFF;  // All 8 bits set to 1
    bool success = updateRelays();
    
    if (success) {
        Serial.println("All relays turned ON");
    }
    
    return success;
}

bool XL9535Relay::allOff() {
    relayStates = 0x00;  // All 8 bits set to 0
    bool success = updateRelays();
    
    if (success) {
        Serial.println("All relays turned OFF");
    }
    
    return success;
}

bool XL9535Relay::setRelayMask(uint8_t mask) {
    relayStates = mask;
    bool success = updateRelays();
    
    if (success) {
        Serial.print("Relays set to mask: 0b");
        Serial.println(mask, BIN);
    }
    
    return success;
}

uint8_t XL9535Relay::getRelayStates() {
    return relayStates;
}

bool XL9535Relay::isRelayOn(uint8_t relay) {
    if (relay > 7) {
        return false;
    }
    
    return (relayStates & (1 << relay)) != 0;
}

bool XL9535Relay::testConnection() {
    Wire.beginTransmission(i2cAddress);
    uint8_t error = Wire.endTransmission();
    
    return (error == 0);
}

bool XL9535Relay::readRegister(uint8_t reg, uint8_t* value) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    uint8_t error = Wire.endTransmission(false);
    
    if (error != 0) {
        return false;
    }
    
    Wire.requestFrom(i2cAddress, (uint8_t)1);
    
    if (Wire.available()) {
        *value = Wire.read();
        return true;
    }
    
    return false;
}

bool XL9535Relay::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(i2cAddress);
    Wire.write(reg);
    Wire.write(value);
    uint8_t error = Wire.endTransmission();
    
    return (error == 0);
}

bool XL9535Relay::updateRelays() {
    // Write relay states to Output Port 0 register
    return writeRegister(XL9535_OUTPUT_PORT_0_REG, relayStates);
}

// ============================================================
// C-Compatible Wrapper Functions
// ============================================================
extern "C" {

void relay_on(uint8_t relay) {
    relayBoard.relayOn(relay);
}

void relay_off(uint8_t relay) {
    relayBoard.relayOff(relay);
}

void relay_toggle(uint8_t relay) {
    relayBoard.toggleRelay(relay);
}

void relay_all_on(void) {
    relayBoard.allOn();
}

void relay_all_off(void) {
    relayBoard.allOff();
}

bool relay_is_on(uint8_t relay) {
    return relayBoard.isRelayOn(relay);
}

void relay_set_mask(uint8_t mask) {
    relayBoard.setRelayMask(mask);
}

uint8_t relay_get_states(void) {
    return relayBoard.getRelayStates();
}

} // extern "C"