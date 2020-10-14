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
#include "main.h"


struct DisplayContent final {
	uint16_t mVerticals;
	int8_t   mTemperature;
	uint8_t  mHorizontals;
};

class Display final {
private:
    static constexpr uint32_t    csNumericSystem     = 10u;
    static constexpr uint32_t    csBsrrResetShift    = 16u;
    static constexpr uint32_t    csDigitCount        =  8u;
	static constexpr uint32_t    csValueBegin        =  0u;
	static constexpr uint32_t    csValueEnd          =  2u;
	static constexpr uint32_t    csGraphicsBegin     =  2u;
	static constexpr uint32_t    csGraphicsEnd       = csDigitCount;
	static constexpr uint32_t    csVerticalExponent  =  2u;
	static constexpr uint32_t    csHorizontalSegment = segment_d_Pin;
	static constexpr uint32_t    csSegmentMask       = segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin | segment_g_Pin;
	static constexpr uint32_t    csDigitMask         = digit0_Pin | digit1_Pin | graphics0_Pin | graphics1_Pin | graphics2_Pin | graphics3_Pin | graphics4_Pin | graphics5_Pin;
	static constexpr uint32_t    csVerticalBitMask   =  (1u << csVerticalExponent) - 1u;
	static constexpr uint32_t    csHorizontalBitMask =  1u;
	static constexpr uint32_t    csInvalidDigitBsrr  = (segment_g_Pin << csBsrrResetShift) | (segment_g_Pin ^ csSegmentMask);
	static constexpr uint32_t    csMinusSignBsrr     = csInvalidDigitBsrr;
	static constexpr uint32_t    csEmptyDigitBsrr    = csSegmentMask;

	inline static constexpr std::array<uint32_t, 1u << csVerticalExponent>  csVerticalSegments = {
	    0u,
	    segment_f_Pin,
	    segment_b_Pin,
	    segment_b_Pin | segment_f_Pin
	};

	inline static constexpr std::array<uint32_t, csNumericSystem>  csDigitSegmentsBsrr = {
        ((segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin) << csBsrrResetShift) | ((segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin) ^ csSegmentMask),
        ((segment_b_Pin | segment_c_Pin) << csBsrrResetShift) | ((segment_b_Pin | segment_c_Pin) ^ csSegmentMask),
        ((segment_a_Pin | segment_b_Pin | segment_d_Pin | segment_e_Pin | segment_g_Pin) << csBsrrResetShift) | ((segment_a_Pin | segment_b_Pin | segment_d_Pin | segment_e_Pin | segment_g_Pin) ^ csSegmentMask),
        ((segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_g_Pin) << csBsrrResetShift) | ((segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_g_Pin) ^ csSegmentMask),
        ((segment_b_Pin | segment_c_Pin | segment_f_Pin | segment_g_Pin) << csBsrrResetShift) | ((segment_b_Pin | segment_c_Pin | segment_f_Pin | segment_g_Pin) ^ csSegmentMask),
        ((segment_a_Pin | segment_c_Pin | segment_d_Pin | segment_f_Pin | segment_g_Pin) << csBsrrResetShift) | ((segment_a_Pin | segment_c_Pin | segment_d_Pin | segment_f_Pin | segment_g_Pin) ^ csSegmentMask),
        ((segment_a_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin |segment_g_Pin) << csBsrrResetShift) | ((segment_a_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin |segment_g_Pin) ^ csSegmentMask),
        ((segment_a_Pin | segment_b_Pin | segment_c_Pin) << csBsrrResetShift) | ((segment_a_Pin | segment_b_Pin | segment_c_Pin) ^ csSegmentMask),
        ((segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin | segment_g_Pin) << csBsrrResetShift) | ((segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_e_Pin | segment_f_Pin | segment_g_Pin) ^ csSegmentMask),
        ((segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_f_Pin | segment_g_Pin) << csBsrrResetShift) | ((segment_a_Pin | segment_b_Pin | segment_c_Pin | segment_d_Pin | segment_f_Pin | segment_g_Pin) ^ csSegmentMask)
	};

	inline static constexpr std::array<uint32_t, csDigitCount>  csDigitsBsrr = {
        (digit0_Pin << csBsrrResetShift) | (digit0_Pin ^ csDigitMask),
        (digit1_Pin << csBsrrResetShift) | (digit1_Pin ^ csDigitMask),
        (graphics0_Pin << csBsrrResetShift) | (graphics0_Pin ^ csDigitMask),
        (graphics1_Pin << csBsrrResetShift) | (graphics1_Pin ^ csDigitMask),
        (graphics2_Pin << csBsrrResetShift) | (graphics2_Pin ^ csDigitMask),
        (graphics3_Pin << csBsrrResetShift) | (graphics3_Pin ^ csDigitMask),
        (graphics4_Pin << csBsrrResetShift) | (graphics4_Pin ^ csDigitMask),
        (graphics5_Pin << csBsrrResetShift) | (graphics5_Pin ^ csDigitMask)
	};

	inline static GPIO_TypeDef* sSegmentPort = segment_a_GPIO_Port;
	inline static GPIO_TypeDef* sDigitPort   = digit0_GPIO_Port;

	inline static std::atomic<int32_t>  sValue;
	inline static std::atomic<uint32_t> sVerticals;
	inline static std::atomic<uint32_t> sHorizontals;
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
	}

	static void refreshNotify() noexcept {
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
	        currentSegmentBsrr = (currentSegmentPins << csBsrrResetShift) | (currentSegmentPins ^ csSegmentMask);
	    }
	    sSegmentPort->BSRR = currentSegmentBsrr;
	    sDigitPort->BSRR = csDigitsBsrr[sCurrentDigit];
	    sCurrentDigit = (sCurrentDigit + 1u) % csDigitCount;
	    sSemaphore = true;
	}

private:
	static constexpr uint32_t power(uint32_t const aExponent) noexcept {
	    uint32_t result = 1u;
	    for(uint32_t i = 0; i < aExponent; ++i) {
	        result *= csNumericSystem;
	    }
	    return result;
	}

};

#endif /* DISPLAY_H_ */
