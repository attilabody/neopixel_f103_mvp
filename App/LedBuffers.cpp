/*
 * LedBuffers.c
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */
#include "LedBuffers.h"

pixel_t g_pixels[NUMPIXELS];

uint8_t g_spibuffer[2][SPIBUFFER_SIZE];
