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


extern IWDG_HandleTypeDef hiwdg;

struct Temperatures final {
	int8_t mInner;
	int8_t mOuter;
};

class Measure final : public Tick {
public:
    static void step() {
    }

    static Temperatures getTemperatures() {
    	// TODO implement
    	return Temperatures { mInner: 13, mOuter: -7 };
    }

    static DisplayContent getDisplayContent() {
    	// TODO implement
    	return DisplayContent { mVerticals: sCountTicks >> 8u, mTemperature: sCountTicks >> 8u, mHorizontals: sCountTicks >> 10u };
    }

private:

};

#endif /* MEASURE_H_ */
