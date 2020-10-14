/*
 * common.h
 *
 *  Created on: Oct 7, 2020
 *      Author: balazs
 */

#ifndef TICK_H_
#define TICK_H_


class Tick {
protected:
	static constexpr int32_t cTicksPerSec = 1000u;

	inline static int32_t sCountTicks = 0u;

public:
	static void tick() noexcept {
		if(++sCountTicks == cTicksPerSec) {
			sCountTicks = 0u;
		}
		else { // nothing to do
		}
	}
};


#endif /* TICK_H_ */
