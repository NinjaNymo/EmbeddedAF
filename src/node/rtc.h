//   _____ _____ _____
//  |   | |__   |   | | EmbeddedAF/src/node/rtc.h
//  | | | |   __| | | | Nikolai Nymo
//  |_|___|_____|_|___| 08-02-18
#ifndef RTC_H
#define	RTC_H

#include "types.h"

void rtc_init(uint8_t refreshtime);
dateTime_t rtc_getDateTime();
time_t rtc_getTime();
void rtc_setTime(uint8_t hh, uint8_t mm);

#endif	/* RTC_H */
