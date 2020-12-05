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
#include <cmsis_os.h>
#include <FreeRTOS.h>
#include <timers.h>

#include "Config.h"
#include "WS28xxStrip.h"
#include "Sparkle.h"

WS28xxStrip<NUMPIXELS, SPIBUFFER_PIXELS>	g_strip( SPI1, DMA1, LL_DMA_CHANNEL_3, { DEFAULT_COLOR } );
Sparkle	g_sparkles[NUMSPARKLES];

/* Definitions for StripFeeder */
#define STRIPFEEDER_STACK_SIZE 64
StackType_t g_stripfeeder_stack[ STRIPFEEDER_STACK_SIZE ];
StaticTask_t g_stripfeeder_control_block;

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

extern "C" void DMA1_Channel3_IRQHandler(void)
{
	LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
	g_strip.HandleSpiDmaIrq();
	LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
}

void TimerCallback(TimerHandle_t th)
{
	LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);

	for(int16_t pixel = 0; pixel < NUMSPARKLES; ++pixel) {
		if(static_cast<pixel_t*>(g_sparkles[pixel]))
			g_sparkles[pixel].Step();
		else
			StartSparkle(g_sparkles[pixel]);
	}
	g_strip.Update();
}

extern "C" void App()
{
	TimerHandle_t th = nullptr;
	StaticTimer_t tcb;

	xTaskCreateStatic(
		WS28xxStrip<NUMPIXELS, SPIBUFFER_PIXELS>::RefillTaskEntry,
		"RefillTask",
		STRIPFEEDER_STACK_SIZE,
		&g_strip,
		osPriorityRealtime,
		g_stripfeeder_stack,
		&g_stripfeeder_control_block);

	LL_SPI_Enable(SPI1);
	LL_SPI_EnableDMAReq_TX(SPI1);

	th = xTimerCreateStatic(nullptr, pdMS_TO_TICKS(20), pdTRUE, nullptr, TimerCallback, &tcb);
	xTimerStart(th, 0);

	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
