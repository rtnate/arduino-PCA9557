#ifndef PCA9557_LIB_H_
#define PCA9557_LIB_H_

#include <I2CDevice.h>

class PCA9557: public HasI2CDevice {
    static constexpr uint8_t I2C_ADDR_DEFAULT = 0x18;
    static constexpr uint8_t INVALID_DATA = 0xFF;
public:
    enum class RegisterSelection: uint8_t {
        Input = 0x0,
        Output = 0x1,
        Polarity = 0x2,
        Config = 0x3
    };

    PCA9557(TwoWire& tw = Wire, uint8_t address = I2C_ADDR_DEFAULT):
        HasI2CDevice(tw, address) {
        updateShadowRegister(RegisterSelection::Input, 0);
        updateShadowRegister(RegisterSelection::Output, 0);
        updateShadowRegister(RegisterSelection::Polarity, 0xF0);
        updateShadowRegister(RegisterSelection::Config, 0xFF);
    };

    void pinPolarity(uint8_t pin, bool shouldReverse);

    void pinMode(uint8_t pin, uint8_t mode);

    bool writePin(uint8_t pin, uint8_t state);

    uint8_t readPin(uint8_t pin) {
        if (pin > 7) return INVALID_DATA;
        if (readAllPins()) {
            return getCurrentPinState(pin);
        }
        return INVALID_DATA;
    }

    uint8_t getInputRegister() const {
        return getShadowRegister(RegisterSelection::Input);
    }

    bool readAllPins();

    bool writeConfigRegister(uint8_t data) {
        return updateRegister(RegisterSelection::Config, data);
    }

    bool writeOutputRegister(uint8_t data) {
        return updateRegister(RegisterSelection::Output, data);
    }

    bool writePolarityRegister(uint8_t data) {
        return updateRegister(RegisterSelection::Polarity, data);
    }

    inline uint16_t getWireHardwareAddress() const {
        return bus.getWireHardwareAddress();
    }
    
    void initializeAsOutputs(uint8_t initialOutputState = 0x0) {
        writeOutputRegister(initialOutputState);
        writePolarityRegister(0x0);
        writeConfigRegister(0x0);
    }
protected:
    bool updateRegister(RegisterSelection reg, uint8_t data);

    uint8_t getCurrentPinState(uint8_t pin) {
        auto ip = getShadowRegister(RegisterSelection::Input);
        uint8_t result = (ip & (1 << pin));
        return result == 0 ? LOW : HIGH;
    }

    uint8_t writeControlRegisterForRead(RegisterSelection reg) {
        bus.beginTransmission();
        writeControlRegister(reg);
        return bus.endTransmission(false);
    }

    void writeControlRegister(RegisterSelection reg) {
        bus.write(static_cast<uint8_t>(reg));
    }

    void updateShadowRegister(RegisterSelection reg, uint8_t value) {
        m_shadow_reg[static_cast<uint8_t>(reg)] = value;
    }

    uint8_t getShadowRegister(RegisterSelection reg) const {
        return m_shadow_reg[static_cast<uint8_t>(reg)];
    }

private:
    uint8_t m_shadow_reg[4];

    uint8_t writeDataToRegister(RegisterSelection reg, uint8_t data) {
        bus.beginTransmission();
        writeControlRegister(reg);
        bus.write(data);
        return bus.endTransmission();
    }
};

#endif /* PCA9557_LIB_H_ */
