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

volatile uint32_t	g_tick = 0;

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
			if(static_cast<Pixel_t*>(g_sparkles[spi]) && static_cast<Pixel_t*>(g_sparkles[spi]) == &g_pixels[chosen])
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
	if(LL_DMA_IsActiveFlag_TC3(DMA1)) {
		LL_DMA_ClearFlag_TC3(DMA1);
		g_spi_idle = true;
		LL_SPI_DisableDMAReq_TX(SPI1);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
	} else if(LL_DMA_IsActiveFlag_TE3(DMA1))
		LL_DMA_ClearFlag_TE3(DMA1);
}

extern "C" void App()
{
	g_spibuffer[sizeof(g_spibuffer)-1] = 0;

	for(uint16_t px = 0; px < NUMPIXELS; ++px)
		g_pixels[px] = Pixel( DEFAULT_COLOR );

	uint32_t lastTick = GetTick();

	LL_SYSTICK_EnableIT();
	LL_SPI_Enable(SPI1);

	while(1)
	{
		while(GetTick() - lastTick < FRAMETIME );
		lastTick += FRAMETIME;
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);

		for(int16_t spi = 0; spi < NUMSPARKLES; ++spi) {
			if(static_cast<Pixel_t*>(g_sparkles[spi]))
				g_sparkles[spi].Step();
			else
				StartSparkle(g_sparkles[spi]);
		}
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);

		convert((uint8_t*)g_pixels, g_spibuffer, sizeof(g_pixels));
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);

		g_spi_idle = false;
//		HAL_SPI_Transmit_DMA(&hspi1, g_ledBits, sizeof(g_ledBits));
		LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3, (uint32_t)g_spibuffer, LL_SPI_DMA_GetRegAddr(SPI1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, sizeof(g_spibuffer));
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
		LL_SPI_EnableDMAReq_TX(SPI1);
		LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
		LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_3);

		while(!g_spi_idle);
		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
	}
}
