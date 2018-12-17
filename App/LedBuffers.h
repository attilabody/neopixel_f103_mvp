/*
 * LedBuffers.h
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */

#ifndef LEDBUFFERS_H_
#define LEDBUFFERS_H_

#include "Config.h"
#include "Pixel.h"


#ifdef __cplusplus
extern "C" {
#endif

extern Pixel_t g_pixels[NUMPIXELS];
uint8_t g_ledBits[sizeof(g_pixels) * 8 / 2 + 1];

#ifdef __cplusplus
}
#endif

#endif /* LEDBUFFERS_H_ */
