/*
 * display.h
 *
 *  Created on: Sep 30, 2020
 *      Author: balazs
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_


#include <array>
#include <atomic>
#include <limits>
#include "main.h"


struct DisplayContent final {
	uint16_t mVerticals;
	int8_t   mTemperature;
	uint8_t  mHorizontals;
};

struct Errors final {
	bool mInner;
	bool mOuter;
};

class BsrrCalculation {
private:
	static constexpr uint32_t    csSegmentMask       = segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin | segment_g_Pin;
	static constexpr uint32_t    csDigitMask         = digit0_Pin | digit1_Pin | graphics0_Pin | graphics1_Pin | graphics2_Pin | graphics3_Pin | graphics4_Pin | graphics5_Pin;
    static constexpr uint32_t    csBsrrResetShift    = 16u;

protected:
	static constexpr uint32_t getBsrrSegment(uint32_t const aSegments) noexcept {
		return (aSegments << csBsrrResetShift) | (aSegments ^ csSegmentMask);
	}

	static constexpr uint32_t getBsrrDigit(uint32_t const aDigit) noexcept {
		return (aDigit << csBsrrResetShift) | (aDigit ^ csDigitMask);
	}
};

class Display final : public BsrrCalculation {
private:
    static constexpr uint32_t    csNumericSystem     = 10u;
    static constexpr uint32_t    csDigitCount        =  8u;
	static constexpr uint32_t    csValueBegin        =  0u;
	static constexpr uint32_t    csValueEnd          =  2u;
	static constexpr uint32_t    csGraphicsBegin     =  2u;
	static constexpr uint32_t    csGraphicsEnd       = csDigitCount;
	static constexpr uint32_t    csVerticalExponent  =  2u;
	static constexpr uint32_t    csHorizontalSegment = segment_d_Pin;
	static constexpr uint32_t    csVerticalBitMask   =  (1u << csVerticalExponent) - 1u;
	static constexpr uint32_t    csHorizontalBitMask =  1u;
	static constexpr uint32_t    csInvalidDigitBsrr  = getBsrrSegment(segment_g_Pin);
	static constexpr uint32_t    csErrorLocIntBsrr   = getBsrrSegment(segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin | segment_g_Pin); // Hungarian letter b
	static constexpr uint32_t    csErrorLocExtBsrr   = getBsrrSegment(segment_c_Pin | segment_d_Pin | segment_e_Pin); // Hungarian letter u
	static constexpr uint32_t    csMinusSignBsrr     = csInvalidDigitBsrr;
	static constexpr uint32_t    csEmptyDigitBsrr    = getBsrrSegment(0u);
	static constexpr uint32_t    csErrorLetterCount  =  4u;
	static constexpr int32_t     csIllegalValue      = std::numeric_limits<int32_t>::max();

	inline static constexpr std::array<uint32_t, 1u << csVerticalExponent>  csVerticalSegments = {
	    0u,
	    segment_f_Pin,
	    segment_b_Pin,
	    segment_b_Pin | segment_f_Pin
	};

	inline static constexpr std::array<uint32_t, csNumericSystem>  csDigitSegmentsBsrr = {
		getBsrrSegment(segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin),
		getBsrrSegment(segment_b_Pin | segment_c_Pin),
		getBsrrSegment(segment_a_Pin | segment_b_Pin | segment_d_Pin | segment_e_Pin | segment_g_Pin),
		getBsrrSegment(segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_g_Pin),
		getBsrrSegment(segment_b_Pin | segment_c_Pin | segment_f_Pin | segment_g_Pin),
		getBsrrSegment(segment_a_Pin | segment_c_Pin | segment_d_Pin | segment_f_Pin | segment_g_Pin),
		getBsrrSegment(segment_a_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin |segment_g_Pin),
		getBsrrSegment(segment_a_Pin | segment_b_Pin | segment_c_Pin),
		getBsrrSegment(segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin | segment_g_Pin),
		getBsrrSegment(segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_f_Pin | segment_g_Pin)
	};

	inline static constexpr std::array<uint32_t, csGraphicsEnd - csGraphicsBegin>  csErrorSegmentsBsrr = {
		getBsrrSegment(segment_c_Pin | segment_e_Pin | segment_f_Pin | segment_g_Pin),
		getBsrrSegment(segment_e_Pin),
		getBsrrSegment(segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin | segment_g_Pin),
		getBsrrSegment(segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_e_Pin | segment_f_Pin | segment_g_Pin),
		csEmptyDigitBsrr,
		csEmptyDigitBsrr
	};

	inline static constexpr std::array<uint32_t, csDigitCount>  csDigitsBsrr = {
		getBsrrDigit(digit0_Pin),
		getBsrrDigit(digit1_Pin),
		getBsrrDigit(graphics0_Pin),
		getBsrrDigit(graphics1_Pin),
		getBsrrDigit(graphics2_Pin),
		getBsrrDigit(graphics3_Pin),
		getBsrrDigit(graphics4_Pin),
		getBsrrDigit(graphics5_Pin)
	};

	inline static GPIO_TypeDef *sSegmentPort = segment_a_GPIO_Port;
	inline static GPIO_TypeDef *sDigitPort   = digit0_GPIO_Port;

	inline static std::atomic<bool>     sErrorInt     = false;
	inline static std::atomic<bool>     sErrorExt     = false;
	inline static std::atomic<int32_t>  sValue        = csIllegalValue;
	inline static std::atomic<uint32_t> sVerticals    = 0u;
	inline static std::atomic<uint32_t> sHorizontals  = 0u;
	inline static uint32_t              sCurrentDigit = 0u;
	inline static std::atomic<bool>     sSemaphore    = false;

public:
	static void wait() noexcept {
		while(!sSemaphore) { // busy wait, we have enough power
		}
		sSemaphore = false;
	}

	static void setContent(DisplayContent const aContent) noexcept {
		sValue = aContent.mTemperature;
		sVerticals = aContent.mVerticals;
		sHorizontals = aContent.mHorizontals;
		sErrorInt = false;
		sErrorExt = false;
	}

	static void setContent(Errors const aContent) noexcept {
		sErrorInt = aContent.mInner;
		sErrorExt = aContent.mOuter;
	}

	static void refreshNotify() noexcept {
		if(sErrorInt || sErrorExt) {
			refreshError();
		}
		else {
			refreshNormal();
		}
	    sCurrentDigit = (sCurrentDigit + 1u) % csDigitCount;
	    sSemaphore = true;
	}

private:
	static void refreshError() noexcept {
		uint32_t currentSegmentBsrr;
		if(sCurrentDigit == 0u && sErrorInt) {
			currentSegmentBsrr = csErrorLocIntBsrr;
		}
		else if(sCurrentDigit == 1u && sErrorExt) {
			currentSegmentBsrr = csErrorLocExtBsrr;
		}
		else if(sCurrentDigit >= csGraphicsBegin) {
			currentSegmentBsrr = csErrorSegmentsBsrr[sCurrentDigit - csGraphicsBegin];
		}
		else {
			currentSegmentBsrr = csEmptyDigitBsrr;
		}
		sSegmentPort->BSRR = currentSegmentBsrr;
	    sDigitPort->BSRR = csDigitsBsrr[sCurrentDigit];
	}

	static void refreshNormal() noexcept {
	    uint32_t currentSegmentBsrr = csEmptyDigitBsrr;
	    if(sCurrentDigit < csValueEnd) {
	        int32_t value = sValue;
	        if(value > -static_cast<int32_t>(power(csValueEnd - 1u)) &&
	                value < static_cast<int32_t>(power(csValueEnd))) {
	            uint32_t absolute;
	            uint32_t lastIndexToDisplay = csValueEnd - 1u;
	            uint32_t actualDigit;
	            if(value < 0) {
	                absolute = static_cast<uint32_t>(-value);
	                --lastIndexToDisplay;
	                if(sCurrentDigit == 0) {
	                    currentSegmentBsrr = csMinusSignBsrr;
	                }
	                else {
	                    actualDigit = sCurrentDigit - 1u;
	                }
	            }
	            else {
	                absolute = static_cast<uint32_t>(value);
	                actualDigit = sCurrentDigit;
	            }
                if(currentSegmentBsrr == csEmptyDigitBsrr) {
                    auto digit = (absolute / power(lastIndexToDisplay - actualDigit)) % csNumericSystem;
                    currentSegmentBsrr = csDigitSegmentsBsrr[digit];
                }
                else { // nothing to do, there is already a minus sign
                }
	        }
	        else {
	            currentSegmentBsrr = csInvalidDigitBsrr;
	        }
	    }
	    else {
	        auto graphicDigit = sCurrentDigit - csGraphicsBegin;
	        uint32_t currentSegmentPins = csVerticalSegments[(sVerticals >> (graphicDigit * csVerticalExponent)) & csVerticalBitMask];
	        currentSegmentPins |= (((sHorizontals >> graphicDigit) & csHorizontalBitMask) == csHorizontalBitMask ? csHorizontalSegment : 0u);
	        currentSegmentBsrr = getBsrrSegment(currentSegmentPins);
	    }
	    sSegmentPort->BSRR = currentSegmentBsrr;
	    sDigitPort->BSRR = csDigitsBsrr[sCurrentDigit];
	}

	static constexpr uint32_t power(uint32_t const aExponent) noexcept {
	    uint32_t result = 1u;
	    for(uint32_t i = 0; i < aExponent; ++i) {
	        result *= csNumericSystem;
	    }
	    return result;
	}
};

#endif /* DISPLAY_H_ */
