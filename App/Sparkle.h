/*
 * Sparkle.h
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */

#ifndef SPARKLE_H_
#define SPARKLE_H_
#include <inttypes.h>
#include "WS28xxStrip.h"

class Sparkle {
public:
	Sparkle();
	~Sparkle();
	void Start(pixel_t *target, uint8_t delay, pixel_t color, pixel_t fadeSpeed);
	bool Step();
	operator pixel_t*() { return m_target; }

private:
	pixel_t		*m_target = nullptr;
	uint8_t		m_delay;
	pixel_t		m_fadeSpeed = {1, 1, 1};
};

#endif /* SPARKLE_H_ */
