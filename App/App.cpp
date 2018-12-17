/*
 * App.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */

#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"

#include <string.h>
#include <stdlib.h>
#include "Config.h"
#include "Pixel.h"
#include "LedBuffers.h"
#include "Sparkle.h"

Sparkle	g_s[NUMSPARKLES];

void convert(uint8_t *src, uint8_t *dst, uint16_t size)
{
	static uint8_t const bits[4] = { 0b10001000, 0b10001110, 0b11101000, 0b11101110 };

	while(size--) {
		uint8_t byte=*src++;
		uint8_t duo = 3;
		do {
			uint8_t mask = 3 << (duo<<1);
			*dst++ = bits[(byte & (mask))>>(duo<<1)];
		} while(duo--);
	}
}

inline uint16_t rr(uint16_t top)
{
	return rand() % top;
}

uint16_t ChoosePixel()
{
	volatile uint16_t chosen;
	uint16_t spi;

	do {
		chosen = rr(NUMPIXELS);
		for(spi=0; spi<NUMSPARKLES; ++spi) {
			if(static_cast<Pixel_t*>(g_s[spi]) && static_cast<Pixel_t*>(g_s[spi]) == &g_pixels[chosen])
				break;
		}
	} while(spi < NUMSPARKLES);
	return chosen;
}

void StartSparkle( Sparkle &s )
{
	s.Start(g_pixels+ChoosePixel(), Pixel(255,255,255), Pixel(rr(8)+3,rr(8)+3,rr(8)+3));
}

extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &hspi1) {
		g_done = true;
	}
}

extern "C" void App()
{
	g_ledBits[sizeof(g_ledBits)-1] = 0;
	memset(g_pixels, 0, sizeof(g_pixels));
	uint32_t	lastTick = HAL_GetTick() - FRAMETIME;

	while(1)
	{
		while(HAL_GetTick() - lastTick < FRAMETIME );
		lastTick += FRAMETIME;
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

		for(int16_t spi = 0; spi < NUMSPARKLES; ++spi) {
			if(static_cast<Pixel_t*>(g_s[spi]))
				g_s[spi].Step();
			else
				StartSparkle(g_s[spi]);
		}
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

		convert((uint8_t*)g_pixels, g_ledBits, sizeof(g_pixels));
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

		g_done = false;
		HAL_SPI_Transmit_DMA(&hspi1, g_ledBits, sizeof(g_ledBits));
		while(!g_done);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	}
}
