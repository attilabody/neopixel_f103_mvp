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

extern pixel_t g_pixels[NUMPIXELS];

#define SPIBUFFER_PIXEL_SIZE ( sizeof(pixel_t) * 4) // each raw pixel bit represented by 4 bits in SPI transfer
#define SPIBUFFER_SIZE (SPIBUFFER_PIXELS * SPIBUFFER_PIXEL_SIZE)

extern uint8_t g_spibuffer[2][SPIBUFFER_SIZE];

#endif /* LEDBUFFERS_H_ */
