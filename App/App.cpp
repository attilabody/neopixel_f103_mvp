/*
 * App.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: abody
 */

#include <stm32f1xx_ll_dma.h>
#include <stm32f1xx_ll_gpio.h>

#include "main.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"

#include <string.h>
#include <stdlib.h>
#include "Config.h"
#include "WS28xxStrip.h"
#include "Sparkle.h"

WS28xxStrip<NUMPIXELS, SPIBUFFER_PIXELS>	g_strip( { DEFAULT_COLOR } );
Sparkle	g_sparkles[NUMSPARKLES];

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
			if(static_cast<pixel_t*>(g_sparkles[spi]) && static_cast<pixel_t*>(g_sparkles[spi]) == &g_strip[chosen])
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
	s.Start(&g_strip[ChoosePixel()], rr(32), { 255,255,255 },
			{ static_cast<uint8_t>(rr(8)+3), static_cast<uint8_t>(rr(8)+3), static_cast<uint8_t>(rr(8)+3) });
#else
	s.Start(g_pixels+ChoosePixel(), 0, { 255,255,255 }, { 1, 1, 1 });
#endif
}

extern "C" uint32_t GetTick()
{
	return g_tick;
}

extern "C" void HandleSpiDmaIrq()
{
	g_strip.SpiDmaIsr();
}

extern "C" void App()
{
	uint32_t lastTick = GetTick();

	LL_SYSTICK_EnableIT();
	LL_SPI_Enable(SPI1);
	LL_SPI_EnableDMAReq_TX(SPI1);

	while(1)
	{
		while(GetTick() - lastTick < FRAMETIME )
			DoNothing();
		lastTick += FRAMETIME;

		LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);

		for(int16_t spi = 0; spi < NUMSPARKLES; ++spi) {
			if(static_cast<pixel_t*>(g_sparkles[spi]))
				g_sparkles[spi].Step();
			else
				StartSparkle(g_sparkles[spi]);
		}

		g_strip.Update();
	}
}
