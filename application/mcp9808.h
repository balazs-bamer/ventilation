#ifndef MCP9808_H
#define MCP9808_H

#include "stm32f1xx_hal.h"
#include "thermometer.h"
#include <limits>

class Mcp9808 final : public Thermometer {
private:
    static constexpr uint8_t  cI2cAddressHigh    = 0x18u;
    static constexpr uint8_t  cI2cAddressLowMask = 0x07u;
    static constexpr uint8_t  cI2cAddressShift   = 0x01u;

    static constexpr uint8_t  cRegAddressTemp    = 0x05u;
    static constexpr uint8_t  cTempShift         = 0x03u;
    static constexpr uint8_t  cTempHighMask      = 0x1fu;
    static constexpr uint16_t cTempPreShift      =    3u;
    static constexpr int16_t  cTempPostDivisor   =  128;
    static constexpr int16_t  cTempPositiveBias  = static_cast<int16_t>(cTempPostDivisor / 2);
    static constexpr int16_t  cTempNegativeBias  = -cTempPositiveBias;

    I2C_HandleTypeDef*        mI2cHandle;
    uint8_t const             cI2cAddress;
    uint32_t const            cI2cTimeout;

public:
    Mcp9808(I2C_HandleTypeDef* aI2cHandle, uint8_t const aI2cAddress, uint32_t const aI2cTimeout) noexcept
      : mI2cHandle(aI2cHandle)
      , cI2cAddress(static_cast<uint8_t>((cI2cAddressHigh | (aI2cAddress & cI2cAddressLowMask)) << cI2cAddressShift))
      , cI2cTimeout(aI2cTimeout) {
    }

    int8_t readTemperature() noexcept {  // first use blocking transfer, then we will see if DMA is needed
        uint8_t temp[sizeof(uint16_t)];
        int8_t result;
        if (HAL_I2C_Mem_Read(mI2cHandle, cI2cAddress, cRegAddressTemp, I2C_MEMADD_SIZE_8BIT, temp, sizeof(uint16_t), cI2cTimeout) == HAL_OK) {
            int16_t value = static_cast<int16_t>((static_cast<uint16_t>(temp[0] & cTempHighMask) << 8u | temp[1]) << cTempPreShift);
            value += (value < 0 ? cTempNegativeBias : cTempPositiveBias);
            result = static_cast<int8_t>(value / cTempPostDivisor);
        }
        else {
            result = cTempIllegal;
        }
        return result;
    }
};

#endif
