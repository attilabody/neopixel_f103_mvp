/*
 * WS28xxStrip.h
 *
 *  Created on: Dec 2, 2020
 *      Author: compi
 */

#ifndef WS28XXSTRIP_H_
#define WS28XXSTRIP_H_

#include <stdint.h>
#include <stm32f1xx_ll_dma.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

struct pixel_t {
	uint8_t g;
	uint8_t r;
	uint8_t b;
};

extern "C" void DMA1_Channel3_IRQHandler(void);

template <uint16_t pixels, uint8_t spi_pixels> class WS28xxStrip {
public:
	WS28xxStrip(pixel_t defcolor);

	pixel_t& operator[](int16_t index);

	friend void DMA1_Channel3_IRQHandler(void);
	void Update();

	static void RefillTaskEntry(void *param) { reinterpret_cast<WS28xxStrip<pixels, spi_pixels>*>(param)->RefillTask(); }

private:
	void SpiDmaIsr();
	void RefillTask();
	void Convert(uint8_t *src, uint8_t *dst, uint16_t src_size);
	template<typename T> T Min(T a, T b) { return a < b ? a : b; }

	TaskHandle_t	m_task_handle = nullptr;

	static constexpr const unsigned int SPI_PIXEL_SIZE = sizeof(pixel_t) * 4;
	static constexpr const unsigned int SPI_BUFFER_SIZE = spi_pixels * SPI_PIXEL_SIZE;

	pixel_t	m_pixels[pixels];
	uint8_t m_spi_buffer[2][SPI_BUFFER_SIZE];


	StaticSemaphore_t m_main_mutex_buffer;
	SemaphoreHandle_t m_main_mutex;
	//volatile uint8_t m_spi_idle = 0;
	volatile uint8_t m_buffer_in_transmit;
	volatile uint16_t m_pixels_converted = 0;
	volatile bool m_endframe = false;
	volatile bool m_endprev = false;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
template <uint16_t pixels, uint8_t spi_pixels> WS28xxStrip<pixels, spi_pixels>::WS28xxStrip(pixel_t defcolor)
: m_main_mutex(xSemaphoreCreateBinaryStatic(&m_main_mutex_buffer))
{
	for(uint16_t p=0; p < pixels; ++p)
		m_pixels[p] = defcolor;

	xSemaphoreGive(m_main_mutex);
}

//////////////////////////////////////////////////////////////////////////////
template <uint16_t pixels, uint8_t spi_pixels> pixel_t& WS28xxStrip<pixels, spi_pixels>::operator[](int16_t index)
{
	return m_pixels[index];
}

//////////////////////////////////////////////////////////////////////////////
template <uint16_t pixels, uint8_t spi_pixels> void WS28xxStrip<pixels, spi_pixels>::SpiDmaIsr()
{
	LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);

	BaseType_t woken;

	if(LL_DMA_IsActiveFlag_TE3(DMA1)) {
		LL_DMA_ClearFlag_TE3(DMA1);
	}
	else if(LL_DMA_IsActiveFlag_HT3(DMA1) || LL_DMA_IsActiveFlag_TC3(DMA1))
	{
		if(LL_DMA_IsActiveFlag_HT3(DMA1))
		{
			LL_DMA_ClearFlag_HT3(DMA1);
			m_buffer_in_transmit = 1;
			if(m_endframe)
				LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);
		}
		else if(LL_DMA_IsActiveFlag_TC3(DMA1))
		{
			LL_DMA_ClearFlag_TC3(DMA1);
			m_buffer_in_transmit = 0;
			if(m_endframe && m_endprev) {
				LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
				LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_CIRCULAR);
				woken = pdFALSE;
				xSemaphoreGiveFromISR(m_main_mutex, &woken);
				portYIELD_FROM_ISR(woken);
				m_endframe = m_endprev = false;
				return;
			}
		}

		woken = pdFALSE;

		if(m_task_handle) {
			LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_0);
			vTaskNotifyGiveFromISR(m_task_handle, &woken);
			portYIELD_FROM_ISR(woken);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
template <uint16_t pixels, uint8_t spi_pixels> void WS28xxStrip<pixels, spi_pixels>::RefillTask()
{
	m_task_handle = xTaskGetCurrentTaskHandle();

	while(true)
	{
		if( ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)) > 0)
		{
			LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
			m_endprev = m_endframe;

			uint8_t convert_now = Min((uint8_t)(pixels - m_pixels_converted), spi_pixels);

			if(convert_now)
			{
				Convert((uint8_t*)&m_pixels[m_pixels_converted],
						m_spi_buffer[m_buffer_in_transmit ^ 1],
						convert_now * sizeof(pixel_t));
				m_pixels_converted += convert_now;
			}

			if(convert_now < spi_pixels) {
				memset(m_spi_buffer[m_buffer_in_transmit ^ 1] + convert_now * SPI_PIXEL_SIZE,
						0, SPI_BUFFER_SIZE - convert_now * SPI_PIXEL_SIZE);
				m_endframe = true;
			}
			LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_13);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
template <uint16_t pixels, uint8_t spi_pixels> void WS28xxStrip<pixels, spi_pixels>::Update()
{
	xSemaphoreTake(m_main_mutex, portMAX_DELAY);

	m_pixels_converted = 0;

	Convert((uint8_t*)m_pixels, m_spi_buffer[0], spi_pixels * sizeof(pixel_t));
	m_pixels_converted += spi_pixels;
	Convert((uint8_t*)&m_pixels[m_pixels_converted], m_spi_buffer[1], spi_pixels * sizeof(pixel_t));
	m_pixels_converted += spi_pixels;

	m_buffer_in_transmit = 0;

	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3, (uint32_t)m_spi_buffer, LL_SPI_DMA_GetRegAddr(SPI1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, sizeof(m_spi_buffer));
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
	LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_3);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_3);
}

//////////////////////////////////////////////////////////////////////////////
template <uint16_t pixels, uint8_t spi_pixels> void WS28xxStrip<pixels, spi_pixels>::Convert(uint8_t *src, uint8_t *dst, uint16_t src_size)
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

#endif /* WS28XXSTRIP_H_ */
