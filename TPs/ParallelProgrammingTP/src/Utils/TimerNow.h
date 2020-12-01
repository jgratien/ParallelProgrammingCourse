
/*
 * TimerNow.h
 *
 *  Created on: Nov 30, 2020
 *      Author: ouadie El farouki
 */

#ifndef SRC_UTILS_TIMER_NOW_H_
#define SRC_UTILS_TIMER_NOW_H_

#include "sys/time.h"
#include <ctime>

double now()
{
  struct timeval t;
  double f_t;
  gettimeofday(&t, NULL);
  f_t = t.tv_usec;
  f_t = f_t / ((float)1E6);
  f_t += t.tv_sec;
  return (f_t);
}
#endif /* SRC_UTILS_TIMER_NOW_H_ */
