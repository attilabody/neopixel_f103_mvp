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

#ifdef __cplusplus
struct Pixel : public Pixel_t {
	Pixel(uint8_t _r, uint8_t _g, uint8_t _b) {
		g =_g;
		r = _r;
		b = _b;
	}
};
#endif

#endif /* PIXEL_H_ */
