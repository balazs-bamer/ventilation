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
	    Measure::step();           // This may last long when I2Ctransfer occurs
	    Control::step(Measure::getTemperatures());
	    Display::setContent(Control::getDisplayContent());
	}
}
