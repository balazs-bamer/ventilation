#ifndef ADT7410_H
#define ADT6410_H

#include "stm32f1xx_hal.h"
#include <limits>

class Adt7410 final {
private:
  static constexpr uint8_t  cI2cAddressHigh    = 0x48;
  static constexpr uint8_t  cI2cAddressLowMask = 0x03;
  static constexpr uint8_t  cI2cAddressShift   = 0x01;

  static constexpr uint8_t  cRegAddressTemp    = 0x00;
  static constexpr uint8_t  cTemp13lowMask     = 0xf8;
  static constexpr float    cTemp13dividor = 128.0f;

  I2C_HandleTypeDef        *mI2cHandle;
  uint8_t const             cI2cAddress;
  uint32_t const            cI2cTimeout;

public:
  Adt7410(I2C_HandleTypeDef *aI2cHandle, uint8_t const aI2cAddress, uint32_t const aI2cTimeout) noexcept
    : mI2cHandle(aI2cHandle)
    , cI2cAddress(static_cast<uint8_t>((cI2cAddressHigh | (aI2cAddress & cI2cAddressLowMask)) << cI2cAddressShift))
    , cI2cTimeout(aI2cTimeout) {
  }

  float readTemperature() noexcept {
      uint8_t temp[sizeof(uint16_t)];
      float result;
      if (HAL_I2C_Mem_Read(mI2cHandle, cI2cAddress, cRegAddressTemp, I2C_MEMADD_SIZE_8BIT, temp, sizeof(uint16_t), cI2cTimeout) == HAL_OK) {
          int16_t value = static_cast<int16_t>(static_cast<uint16_t>(temp[0]) << 8u | (temp[1] & cTemp13lowMask));
          result = static_cast<float>(value) / cTemp13dividor;
      }
      else {
          result = std::numeric_limits<float>::quiet_NaN();
      }
      return result;
  }
};

#endif
