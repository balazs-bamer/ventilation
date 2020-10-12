/*
 * measure.h
 *
 *  Created on: 2020. okt. 4.
 *      Author: balazs
 */

#ifndef MEASURE_H_
#define MEASURE_H_


#include "tick.h"
#include "display.h"
#include "adt7410.h"
#include "mcp9808.h"
#include <cmath>

#include "stm32f1xx_hal.h"  // TODO remove

extern IWDG_HandleTypeDef hiwdg;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

struct Temperatures final {
	int8_t mInner;
	int8_t mOuter;
};

class Measure final : public Tick {
private:
	static constexpr uint8_t  csI2c1address     =   3u;
	static constexpr uint8_t  csI2c2address     =   7u;
	static constexpr uint32_t csI2cTimeout      = 300u;
	static constexpr uint32_t csTickToReadExt   =   0u;
	static constexpr uint32_t csTickToReadInt   = 500u;
	static constexpr float    csInvalid4display = 111.4f;

	inline static Adt7410 sThermometerExt{&hi2c1, csI2c1address, csI2cTimeout};
	inline static Mcp9808 sThermometerInt{&hi2c2, csI2c2address, csI2cTimeout};

	inline static float sLastExt = -9.0f;
	inline static float sLastInt = 99.0f;

public:
    static void step() noexcept {    // This may last long when I2Ctransfer occurs
    	if(sCountTicks == csTickToReadExt) {
   		    sLastExt = sThermometerExt.readTemperature();
    	}
    	else if(sCountTicks == csTickToReadInt) {
HAL_GPIO_WritePin (motor_GPIO_Port, motor_Pin, GPIO_PIN_SET);
	 	 	sLastInt = sThermometerInt.readTemperature();
HAL_GPIO_WritePin (motor_GPIO_Port, motor_Pin, GPIO_PIN_RESET);
    	}
    	else { // nothing to do
    	}
    }

    static Temperatures getTemperatures() noexcept {
    	return Temperatures { mInner: sLastInt, mOuter: sLastExt };
    }

    static DisplayContent getDisplayContent() noexcept {
    	int32_t temperature = sCountTicks < csTickToReadInt ? sLastExt : sLastInt;
    	return DisplayContent { mVerticals: sCountTicks, mTemperature: temperature, mHorizontals: sCountTicks >> 4u };
    }

private:

};

#endif /* MEASURE_H_ */
