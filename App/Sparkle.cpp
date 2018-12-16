/*
 * Sparkle.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */

#include "Sparkle.h"

Sparkle::Sparkle()
{
}

Sparkle::~Sparkle() {
}

void Sparkle::Start(Pixel_t *target, Pixel_t color, Pixel_t fadeSpeed)
{
	m_target = target;
	*target = color;
	m_fadeSpeed = fadeSpeed;
}

bool Sparkle::Step()
{
	Pixel_t current = *m_target;
	uint8_t remaining = 3;

	if(current.r > m_fadeSpeed.r )current.r -= m_fadeSpeed.r;
	else {
		current.r = 0;
		--remaining;
	}

	if(current.g > m_fadeSpeed.g )current.g -= m_fadeSpeed.g;
	else {
		current.g = 0;
		--remaining;
	}

	if(current.b > m_fadeSpeed.b )current.b -= m_fadeSpeed.b;
	else {
		current.b = 0;
		--remaining;
	}
	*m_target = current;

	if(remaining) return true;
	m_target = nullptr;
	return false;

}
