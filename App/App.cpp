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
volatile uint8_t g_done = 0;

void convert(uint8_t *src, uint8_t *dst, uint16_t size)
{
	static uint8_t const bits[4] = { 0b10001000, 0b10001110, 0b11101000, 0b11101110 };

	while(size--) {
		uint8_t byte=*src++;
		for(int8_t shift = 6; shift >= 0; shift -= 2) {
			uint8_t mask = 3 << shift;
			*dst++ = bits[ (byte & mask) >> shift ];
		}
	}
}

inline uint16_t rr(uint16_t top)
{
	return rand() % top;
}

uint16_t ChoosePixel()
{
#ifndef DBG_CHOSEN_PIXEL
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
#else
	return DBG_CHOSEN_PIXEL;
#endif	//	DBG_CHOSEN_PIXEL
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
	g_spibuffer[sizeof(g_spibuffer)-1] = 0;

	for(uint16_t px = 0; px < NUMPIXELS; ++px)
		g_pixels[px] = Pixel( DEFAULT_COLOR );

	uint32_t lastTick = HAL_GetTick();

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

		convert((uint8_t*)g_pixels, g_spibuffer, sizeof(g_pixels));
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

		g_done = false;
		HAL_SPI_Transmit_DMA(&hspi1, g_spibuffer, sizeof(g_spibuffer));
		while(!g_done);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	}
}
