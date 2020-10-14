/*
 * control.h
 *
 *  Created on: Oct 7, 2020
 *      Author: balazs
 */

#ifndef CONTROL_H_
#define CONTROL_H_


#include "stm32f1xx_hal.h"
#include "tick.h"
#include "display.h"
#include "measure.h"


extern IWDG_HandleTypeDef hiwdg;

class Control final : public Tick {
private:
	static constexpr int32_t       cDisplayOuterEndTick = 333;
	static constexpr int8_t        cFreeze              = 0;
	static constexpr int8_t        cHysteresis          = 2;
    static constexpr GPIO_PinState cMotorOff            = GPIO_PIN_RESET;
    static constexpr GPIO_PinState cMotorOn             = GPIO_PIN_SET;
    static constexpr uint32_t      cLedPin              = LED_Pin;
    inline static GPIO_TypeDef    *sLedPort             = LED_GPIO_Port;
    static constexpr uint32_t      cMotorPin            = motor_Pin;
    inline static GPIO_TypeDef    *sMotorPort           = motor_GPIO_Port;

    inline static Temperatures sTemperatures;

public:
    static void initialize() noexcept {
    	setOutput(cMotorOff);
    }

    static void step(Temperatures const & aTemperatures) noexcept {
    	sTemperatures = aTemperatures;
    	if(sTemperatures.mOuter < cFreeze) {
    		setOutput(cMotorOff);
    	}
    	else if(sTemperatures.mInner > sTemperatures.mOuter + cHysteresis){
    		setOutput(cMotorOn);
    	}
    	else if(sTemperatures.mInner <= sTemperatures.mOuter){
    		setOutput(cMotorOff);
		}
    	else { // nothing to do
    	}
    	HAL_IWDG_Refresh(&hiwdg);
    }

    static DisplayContent getDisplayContent() noexcept {
		int32_t temperature = (sCountTicks < cDisplayOuterEndTick ? sTemperatures.mOuter : sTemperatures.mInner);
		return DisplayContent { mVerticals: sCountTicks, mTemperature: temperature, mHorizontals: sCountTicks >> 4u };
	}

private:
    static void setOutput(GPIO_PinState const aState) noexcept {
    	HAL_GPIO_WritePin(sMotorPort, cMotorPin, aState);
    	HAL_GPIO_WritePin(sLedPort, cLedPin, aState == cMotorOff ? cMotorOn : cMotorOff);
    }
};

#endif /* CONTROL_H_ */
