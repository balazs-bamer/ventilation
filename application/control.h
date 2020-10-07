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
    static constexpr uint32_t       csMotorPin    = motor_Pin;
    inline static GPIO_TypeDef     *sMotorPort    = motor_GPIO_Port;

public:
    static void step(Temperatures const aTemperatures) {
    	HAL_IWDG_Refresh(&hiwdg);
    }
};

#endif /* CONTROL_H_ */
