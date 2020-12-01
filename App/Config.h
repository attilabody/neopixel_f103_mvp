/*
 * Config.h
 *
 *  Created on: Dec 17, 2018
 *      Author: abody
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//#define DBG_CHOSEN_PIXEL 2

#ifndef DBG_CHOSEN_PIXEL
#	define NUMPIXELS 144
#	define NUMSPARKLES (NUMPIXELS/12+1)
#else
#	define NUMPIXELS 6
#	define NUMSPARKLES 1
#endif

#define FRAMETIME 20
#define DEFAULT_COLOR 0,0,0
#endif /* CONFIG_H_ */
