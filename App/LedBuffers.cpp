/*
 * LedBuffers.c
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */
#include "LedBuffers.h"

Pixel_t g_pixels[NUMPIXELS];
uint8_t g_ledBits[sizeof(g_pixels) * 8 / 2 + 1];



