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
#include "Pixel.h"
#include "LedBuffers.h"
#include "Sparkle.h"

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

extern "C" void App()
{
	g_ledBits[sizeof(g_ledBits)-1] = 0;
	memset(g_pixels, 0, sizeof(g_pixels));


	while(1)
	{
		Sparkle	s;
		Pixel_t color = {0xff, 0xff, 0xff};
		Pixel_t fadeSpeed = {1,2,3};

		for(uint16_t idx=0; idx < NUMPIXELS; idx++)
		{
			s.Start(&g_pixels[idx], color, fadeSpeed);
			convert((uint8_t*)g_pixels, g_ledBits, sizeof(g_pixels));
			HAL_SPI_Transmit_DMA(&hspi1, g_ledBits, sizeof(g_ledBits));
			while(!g_done);
			HAL_Delay(1);

			do {
				s.Step();
				convert((uint8_t*)g_pixels, g_ledBits, sizeof(g_pixels));
				HAL_SPI_Transmit_DMA(&hspi1, g_ledBits, sizeof(g_ledBits));
				while(!g_done);
				HAL_Delay(1);
			} while( static_cast<Pixel_t*>( s ));
		}
	}
}
