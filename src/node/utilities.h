//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/utilities.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 06-02-18
#ifndef UTILITIES_H
#define	UTILITIES_H

#include "types.h"

void     util_initPins();
void     util_initADC();
uint8_t  util_getWaterLevel();
void     util_runLampFSM(systemState_t* state);
void     util_initInterrupts();
void     util_initTimer23();
void     util_startTimer23();
void     util_stopTimer23();

#endif	/* UTILITIES_H */

