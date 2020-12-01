/*
 * App.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */

#include <stm32f1xx_ll_dma.h>

#include "main.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"

#include <string.h>
#include <stdlib.h>
#include "Config.h"
#include "Pixel.h"
#include "LedBuffers.h"
#include "Sparkle.h"

Sparkle	g_sparkles[NUMSPARKLES];
volatile uint8_t g_spi_idle = 0;
uint16_t g_pixels_transmitted;
volatile uint8_t g_buffer_in_transmit;
volatile bool g_need_refill;
uint32_t g_refills = 0;
uint16_t g_pixels_converted = 0;

volatile uint32_t g_tick = 0;

#define MIN(a,b) ((a) < (b) ? (a) : (b))

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
			if(static_cast<pixel_t*>(g_sparkles[spi]) && static_cast<pixel_t*>(g_sparkles[spi]) == &g_pixels[chosen])
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
	s.Start(g_pixels+ChoosePixel(), rr(32), Pixel(255,255,255), Pixel(rr(8)+3,rr(8)+3,rr(8)+3));
}

extern "C" void HandleSystick()
{
	++ g_tick;
}

extern "C" uint32_t GetTick()
{
	return g_tick;
}

extern "C" void HandleSpiDmaIrq()
{
	static bool endframe;

	if(LL_DMA_IsActiveFlag_TC3(DMA1))
	{
		LL_DMA_ClearFlag_TC3(DMA1);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);

		g_buffer_in_transmit ^= 1;
		g_pixels_transmitted += SPIBUFFER_PIXELS;

		if(g_pixels_transmitted < NUMPIXELS)
		{
			LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)g_spibuffer[g_buffer_in_transmit]);
			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, SPIBUFFER_SIZE);
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);

			if(g_pixels_converted < NUMPIXELS)
			{
				LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
				convert((uint8_t*)&g_pixels[g_pixels_converted],
						g_spibuffer[g_buffer_in_transmit ^ 1],
						MIN(SPIBUFFER_PIXELS, NUMPIXELS - g_pixels_converted)  * sizeof(pixel_t));
				g_pixels_converted += MIN(SPIBUFFER_PIXELS, NUMPIXELS - g_pixels_converted);
				LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
				endframe = false;
			}
			else
			{
				memset(g_spibuffer[g_buffer_in_transmit ^1], 0, sizeof(g_spibuffer[g_buffer_in_transmit ^1]));
				endframe = true;
			}

		}
		else if(endframe)
		{
			LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t)g_spibuffer[g_buffer_in_transmit]);
			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, SPIBUFFER_SIZE);
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
			endframe = false;
		}
		else {
			g_spi_idle = true;
		}
	}
	else if(LL_DMA_IsActiveFlag_TE3(DMA1))
		LL_DMA_ClearFlag_TE3(DMA1);

}

extern "C" void App()
{
	uint32_t lastTick = GetTick();

	for(uint16_t px = 0; px < NUMPIXELS; ++px)
		g_pixels[px] = Pixel(DEFAULT_COLOR);

	LL_SYSTICK_EnableIT();
	LL_SPI_Enable(SPI1);
	LL_SPI_EnableDMAReq_TX(SPI1);

	while(1)
	{
		while(GetTick() - lastTick < FRAMETIME );
		lastTick += FRAMETIME;

		for(int16_t spi = 0; spi < NUMSPARKLES; ++spi) {
			if(static_cast<pixel_t*>(g_sparkles[spi]))
				g_sparkles[spi].Step();
			else
				StartSparkle(g_sparkles[spi]);
		}

		g_pixels_converted = 0;
		g_pixels_transmitted = 0;
		g_need_refill = true;

		convert((uint8_t*)g_pixels, g_spibuffer[0], SPIBUFFER_PIXELS * sizeof(pixel_t));
		g_pixels_converted += SPIBUFFER_PIXELS;
		convert((uint8_t*)&g_pixels[g_pixels_converted], g_spibuffer[1], SPIBUFFER_PIXELS * sizeof(pixel_t));
		g_pixels_converted += SPIBUFFER_PIXELS;

		g_buffer_in_transmit = 0;

		LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3, (uint32_t)g_spibuffer[0], LL_SPI_DMA_GetRegAddr(SPI1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, SPIBUFFER_SIZE);
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
		LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
		LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_3);
		g_spi_idle = false;

		while(!g_spi_idle);

	}
}
