/*
 * ll_consolehandler.cpp
 *
 *  Created on: Nov 7, 2019
 *      Author: abody
 */

#include <string.h>
#include "f1ll/consolehandler.h"

namespace f1ll {

ConsoleHandler::ConsoleHandler(USART_TypeDef *usart, DMA_TypeDef *dma, uint32_t channelRx, uint32_t channelTx)
: UsartCore(usart, dma, channelRx, channelTx)
{
}

void ConsoleHandler::ReceiverIdle(void) {}
void ConsoleHandler::TransmissionComplete(void) {}
void ConsoleHandler::FramingError(void) {}
void ConsoleHandler::Overrun(void) {}
void ConsoleHandler::RxDmaTransferComplete(void) {}
void ConsoleHandler::RxDmaHalfTransfer(void) {}
void ConsoleHandler::RxDmaError(void) {}
void ConsoleHandler::TxDmaTransferComplete(void)
{
	LL_USART_EnableIT_TC(m_usart);
	LL_DMA_DisableChannel(m_txDma.GetDma(), m_txDma.GetChannel());
}
void ConsoleHandler::TxDmaHalfTransfer(void) {}
void ConsoleHandler::TxDmaError(void) {}

void ConsoleHandler::Print(char const *s)
{
	size_t len = strlen(s);
	strncpy(m_buffer, s, sizeof( m_buffer));
	SetupTransmit(m_buffer, len > sizeof(m_buffer) ? sizeof(m_buffer) : len);
}


} /* namespace f4ll */

