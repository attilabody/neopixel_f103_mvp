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

void Sparkle::Start(pixel_t *target, uint8_t delay, pixel_t color, pixel_t fadeSpeed)
{
	m_target = target;
	*target = color;
	m_delay = delay;
	m_fadeSpeed = fadeSpeed;
}

bool Sparkle::Step()
{
	if(m_delay) {
		--m_delay;
		return true;
	}

	pixel_t current = *m_target;
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
