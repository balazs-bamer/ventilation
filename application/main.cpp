/*
 * main.cpp
 *
 *  Created on: Sep 30, 2020
 *      Author: balazs
 */

#include "display.h"
#include "measure.h"
#include "control.h"


extern "C" void systemTickUserCallback() {
	Display::refreshNotify();
	Tick::tick();
}

extern "C" void loop() {
	Control::initialize();
	while(true) {
	    Display::wait();
	    Measure::step();
	    Control::step(Measure::getTemperatures());
	    Errors errors = Measure::getErrors();
	    if(errors.mInner || errors.mOuter) {
	    	Display::setContent(errors);
	    }
	    else {
	    	Display::setContent(Control::getDisplayContent());
		}
	}
}
