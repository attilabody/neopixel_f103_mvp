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
volatile uint8_t g_buffer_in_transmit;
volatile uint16_t g_pixels_converted = 0;

volatile uint32_t g_tick = 0;

#define MIN(a,b) ((a) < (b) ? (a) : (b))

void convert(uint8_t *src, uint8_t *dst, uint16_t src_size)
{
	static uint16_t const bits[16] = { // due to LE-ness the bit order is 1 0 3 2
			0b1000100010001000, 0b1000111010001000, 0b1110100010001000, 0b1110111010001000,
			0b1000100010001110, 0b1000111010001110, 0b1110100010001110, 0b1110111010001110,
			0b1000100011101000, 0b1000111011101000, 0b1110100011101000, 0b1110111011101000,
			0b1000100011101110, 0b1000111011101110, 0b1110100011101110, 0b1110111011101110
	};

	uint16_t *dstptr = (uint16_t*)dst;
	while(src_size--) {
		uint8_t tmp =*src++;
		*dstptr++ = bits[tmp >> 4];
		*dstptr++ = bits[tmp & 0x0f];
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
#ifndef DBG_CHOSEN_PIXEL
	s.Start(g_pixels+ChoosePixel(), rr(32), Pixel(255,255,255), Pixel(rr(8)+3,rr(8)+3,rr(8)+3));
#else
	s.Start(g_pixels+ChoosePixel(), 0, Pixel(255,255,255), Pixel(1, 1, 1));
#endif
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
	static bool endframe = false;
	static bool endprev = false;

	if(LL_DMA_IsActiveFlag_TE3(DMA1)) {
		LL_DMA_ClearFlag_TE3(DMA1);
	}
	else if(LL_DMA_IsActiveFlag_HT3(DMA1) || LL_DMA_IsActiveFlag_TC3(DMA1))
	{
		if(LL_DMA_IsActiveFlag_HT3(DMA1))
		{
			LL_DMA_ClearFlag_HT3(DMA1);
			g_buffer_in_transmit = 1;
			if(endframe)
				LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);
		}
		else if(LL_DMA_IsActiveFlag_TC3(DMA1))
		{
			LL_DMA_ClearFlag_TC3(DMA1);
			g_buffer_in_transmit = 0;
			if(endframe && endprev) {
				LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
				LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_CIRCULAR);
				g_spi_idle = true;
				endframe = endprev = false;
				return;
			}
		}

		endprev = endframe;

		uint8_t convert_now = MIN(NUMPIXELS - g_pixels_converted, SPIBUFFER_PIXELS);

		if(convert_now)
		{
			LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
			convert((uint8_t*)&g_pixels[g_pixels_converted],
					g_spibuffer[g_buffer_in_transmit ^ 1],
					convert_now * sizeof(pixel_t));
			LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
			g_pixels_converted += convert_now;
		}

		if(convert_now < SPIBUFFER_PIXELS) {
			memset(g_spibuffer[g_buffer_in_transmit ^ 1] + convert_now * SPIBUFFER_PIXEL_SIZE,
					0, SPIBUFFER_SIZE - convert_now * SPIBUFFER_PIXEL_SIZE);
			endframe = true;
		}
	}
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

		convert((uint8_t*)g_pixels, g_spibuffer[0], SPIBUFFER_PIXELS * sizeof(pixel_t));
		g_pixels_converted += SPIBUFFER_PIXELS;
		convert((uint8_t*)&g_pixels[g_pixels_converted], g_spibuffer[1], SPIBUFFER_PIXELS * sizeof(pixel_t));
		g_pixels_converted += SPIBUFFER_PIXELS;

		g_buffer_in_transmit = 0;
		g_spi_idle = false;

		LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3, (uint32_t)g_spibuffer, LL_SPI_DMA_GetRegAddr(SPI1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, sizeof(g_spibuffer));
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
		LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_3);
		LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
		LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_3);

		while(!g_spi_idle);

	}
}
