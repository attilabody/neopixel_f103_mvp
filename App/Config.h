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
#	define FRAMETIME 20
#else
#	define NUMPIXELS 144
#	define NUMSPARKLES 1
#	define FRAMETIME 10
#endif

#define DEFAULT_COLOR 15,15,15
#define SPIBUFFER_PIXELS 4
#endif /* CONFIG_H_ */
