/*
 * Sparkle.h
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */

#ifndef SPARKLE_H_
#define SPARKLE_H_
#include <inttypes.h>
#include "Pixel.h"

class Sparkle {
public:
	Sparkle();
	~Sparkle();
	void Start(Pixel_t *target, Pixel_t color, Pixel_t fadeSpeed);
	bool Step();
	operator Pixel_t*() { return m_target; }

private:
	Pixel_t		*m_target = nullptr;
	Pixel_t		m_fadeSpeed = {1, 1, 1};
};

#endif /* SPARKLE_H_ */
