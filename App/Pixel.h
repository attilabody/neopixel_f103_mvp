/*
 * Pixel.h
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */

#ifndef PIXEL_H_
#define PIXEL_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint8_t g;
	uint8_t r;
	uint8_t b;
} Pixel_t;

#ifdef __cplusplus
}
#endif

#endif /* PIXEL_H_ */
