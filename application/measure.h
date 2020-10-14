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
#include <array>
#include <algorithm>

#include "stm32f1xx_hal.h"  // TODO remove

extern IWDG_HandleTypeDef hiwdg;
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

struct Temperatures final {
	int8_t mInner;
	int8_t mOuter;
};

/// This filter expects extreme slow temperature changes and measurements
/// transfer errors. So it considers only the majority value over a long sliding window,
/// or if there are more, their average.
class Filter final {
private:
	static constexpr int8_t cTempMinimal      = -40;
	static constexpr int8_t cTempMaximal      =  80;
	static constexpr int8_t cSampleCountsSize = cTempMaximal - cTempMinimal + 1;
	static constexpr size_t cMaxQueueSize     = std::numeric_limits<uint8_t>::max();

	std::array<uint8_t, cSampleCountsSize> mSampleCounts;
	std::array<int8_t, cMaxQueueSize> mQueue;
	size_t mQueueNextPush = 0u;
	size_t mQueueNextPop  = 0u;
	size_t mQueueSize     = 0u;

public:
	Filter() {
		std::fill(mSampleCounts.begin(), mSampleCounts.end(), 0u);
	}

	void nextSample(int8_t const aValue) {
		if(aValue >= cTempMinimal && aValue <= cTempMaximal) {
			if(mQueueSize == cMaxQueueSize) {
				--mSampleCounts[mQueue[mQueueNextPop] - cTempMinimal];
				mQueueNextPop = (mQueueNextPop + 1u) % cMaxQueueSize;
			}
			else {
				++mQueueSize;
			}
			++mSampleCounts[aValue - cTempMinimal];
			mQueue[mQueueNextPush] = aValue;
			mQueueNextPush = (mQueueNextPush + 1u) % cMaxQueueSize;
		}
		else { // nothing to do
		}
	}

	int8_t getFilteredValue() const {
		int8_t result;
		if(mQueueSize == cMaxQueueSize) {
			int32_t maxCountCount = 0;
			uint8_t maxCount = *std::max_element(mSampleCounts.begin(), mSampleCounts.end());
			int32_t sum = 0;
			for(int8_t i = 0; i < cSampleCountsSize; ++i) {
				if(mSampleCounts[i] == maxCount) {
					sum += i + cTempMinimal;
					++maxCountCount;
				}
				else { // nothing to do
				}
			}
			result = sum / maxCountCount;
		}
		else {
			result = Thermometer::cTempIllegal;
		}
		return result;
	}
};

class Measure final : public Tick {
private:
	static constexpr uint32_t cTickToHandleExt =    0u;
	static constexpr uint32_t cTickToHandleInt =  500u;

	static constexpr uint8_t  csI2c1address     =   3u;
	static constexpr uint8_t  csI2c2address     =   7u;
	static constexpr uint32_t csI2cTimeout      = 300u;

	inline static Adt7410 sThermometerExt{&hi2c1, csI2c1address, csI2cTimeout};
	inline static Mcp9808 sThermometerInt{&hi2c2, csI2c2address, csI2cTimeout};

	inline static Filter sFilterExt;
	inline static Filter sFilterInt;

	inline static Temperatures sTemperatures = { mInner: Thermometer::cTempIllegal, mOuter: Thermometer::cTempIllegal };

public:
    static void step() noexcept {    // This may last long when I2Ctransfer occurs
    	if(sCountTicks == cTickToHandleExt) {
   		    sFilterExt.nextSample(sThermometerExt.readTemperature());
   		    sTemperatures.mOuter = sFilterExt.getFilteredValue();
    	}
    	else if(sCountTicks == cTickToHandleInt) {
	 	 	sFilterInt.nextSample(sThermometerInt.readTemperature());
	 	 	sTemperatures.mInner = sFilterInt.getFilteredValue();
    	}
    	else { // nothing to do
    	}
    }

    static Temperatures const & getTemperatures() noexcept {
    	return sTemperatures;
    }

private:

};

#endif /* MEASURE_H_ */
