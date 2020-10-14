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
	static constexpr int32_t       cMotorOnHoursToConsider =    2;
	static constexpr int32_t       cHoursPerDay            =   24;
	static constexpr int32_t       cSecondsPerHour         = 3600;
	static constexpr int32_t       cSecondsPerHalfAnHour   = cSecondsPerHour / 2;
	static constexpr int32_t       cDisplayOuterEndTick    =  333;
	static constexpr int8_t        cFreeze                 =    0;
	static constexpr int8_t        cHysteresis             =    2;
    static constexpr GPIO_PinState cMotorOff               = GPIO_PIN_RESET;
    static constexpr GPIO_PinState cMotorOn                = GPIO_PIN_SET;
    static constexpr uint32_t      cLedPin                 = LED_Pin;
    inline static GPIO_TypeDef    *sLedPort                = LED_GPIO_Port;
    static constexpr uint32_t      cMotorPin               = motor_Pin;
    inline static GPIO_TypeDef    *sMotorPort              = motor_GPIO_Port;

    inline static Temperatures sTemperatures;
    inline static bool         sMotorRunning;
    inline static int32_t     sCountTicksPrevious = 0;
    inline static uint32_t    sMotorOnHours       = 0u;
    inline static uint32_t    sMotorOnDays        = 0u;
    inline static int32_t     sCountSeconds       = 0;
    inline static int32_t     sCountHours         = 0;
    inline static int32_t     sMotorOnInHour      = 0;

public:
    static void initialize() noexcept {
    	setOutput(cMotorOff);
    }

    static void step(Temperatures const & aTemperatures) noexcept {
    	sTemperatures = aTemperatures;
    	if(sTemperatures.mOuter < cFreeze) {
    		setOutput(cMotorOff);
    	}
    	else if(sTemperatures.mInner >= sTemperatures.mOuter + cHysteresis){
    		setOutput(cMotorOn);
    	}
    	else if(sTemperatures.mInner <= sTemperatures.mOuter){
    		setOutput(cMotorOff);
		}
    	else { // nothing to do
    	}
    	if(sCountTicksPrevious > sCountTicks) {
    		handleSecond();
    	}
    	else { // nothing to do
    	}
    	sCountTicksPrevious = sCountTicks;
    	HAL_IWDG_Refresh(&hiwdg);
    }

    static DisplayContent getDisplayContent() noexcept {
		int8_t temperature = (sCountTicks < cDisplayOuterEndTick ? sTemperatures.mOuter : sTemperatures.mInner);
		return DisplayContent { mVerticals: static_cast<uint16_t>(sMotorOnHours), mTemperature: temperature, mHorizontals: static_cast<uint8_t>(sMotorOnDays) };
	}

private:
    static void setOutput(GPIO_PinState const aState) noexcept {
    	sMotorRunning = (aState == cMotorOn);
    	HAL_GPIO_WritePin(sMotorPort, cMotorPin, aState);
    	HAL_GPIO_WritePin(sLedPort, cLedPin, aState == cMotorOff ? cMotorOn : cMotorOff);
    }

    static void handleSecond() noexcept {
    	sMotorOnInHour += (sMotorRunning ? 1 : 0);
    	++sCountSeconds;
    	if(sCountSeconds == cSecondsPerHour) {
    		handleHour();
    	}
    	else { // nothing to do
    	}
    }

    static void handleHour() noexcept {
		++sCountHours;
		sCountSeconds = 0;
		sMotorOnHours <<= 1u;
		sMotorOnHours |= (sMotorOnInHour >= cSecondsPerHalfAnHour ? 1u : 0u );
		sMotorOnInHour = 0;
		if(sCountHours == cHoursPerDay) {
			handleDay();
		}
		else { // nothing to do
		}
	}

    static void handleDay() noexcept {
		sCountHours = 0;
		sMotorOnDays <<= 1u;
		int32_t motorOnInDay = 0;
		for(uint32_t i = 0u; i < static_cast<uint32_t>(cHoursPerDay); ++i) {
			motorOnInDay += (sMotorOnHours >> i > 0u ? 1 : 0);
		}
		sMotorOnDays |= (motorOnInDay >= cMotorOnHoursToConsider ? 1u : 0u );
	}
};

#endif /* CONTROL_H_ */
