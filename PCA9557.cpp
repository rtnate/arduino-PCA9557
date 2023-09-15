#include "PCA9557.h"

void PCA9557::pinPolarity(uint8_t pin, bool shouldReverse) {
    if (pin > 7) return;
    uint8_t pol = getShadowRegister(RegisterSelection::Polarity);
    if (shouldReverse) {
        bitSet(pol, pin);
    } else {
        bitClear(pol, pin);
    }
    writePolarityRegister(pol);
}

void PCA9557::pinMode(uint8_t pin, uint8_t mode) {
    if (pin > 7) return;
    uint8_t cfg = getShadowRegister(RegisterSelection::Config);
    if (mode == OUTPUT) {
        bitClear(cfg, pin);
    } else {
        bitSet(cfg, pin);
    }
    writeConfigRegister(cfg);
}

bool PCA9557::writePin(uint8_t pin, uint8_t state) {
    if (pin > 7) return false;
    uint8_t output = getShadowRegister(RegisterSelection::Output);
    if (state == LOW) {
        bitClear(output, pin);
    } else {
        bitSet(output, pin);
    }
    writeOutputRegister(output);
}

bool PCA9557::readAllPins() {
    // Serial.print("Reading Pins, writing control register...");
    auto result = writeControlRegisterForRead(RegisterSelection::Input);
    if (result != I2CDevice::SUCCESS) { 
        // Serial.print("FAILED e: ");
        // Serial.println(result, HEX);
        return false;
    }
    // Serial.println("success");
    // Serial.print("Requesting bytes from device...");
    if (bus.requestBytes(1)) {
        uint8_t data = bus.read();
        // Serial.print("success, rx: ");
        // Serial.println(data, HEX);
        updateShadowRegister(RegisterSelection::Input, data);
        return true;
    }
    else {
        // Serial.println("FAIL.");
        return false;
    }
}

bool PCA9557::updateRegister(RegisterSelection reg, uint8_t data) {
    // Serial.print(F("Updating Ext Register: "));
    // Serial.print(static_cast<uint8_t>(reg));
    // Serial.print(F(" Value: "));
    // Serial.println(data);
    auto current = getShadowRegister(reg);
    if (data == current) return true;
    Serial.println(F("Doing data write..."));

    auto result = writeDataToRegister(reg, data);
    if (result == I2CDevice::SUCCESS) {
        // Serial.println(F("Success."));
        updateShadowRegister(reg, data);
        return true;
    }
    // Serial.println(F("fail."));
    return false;
}


