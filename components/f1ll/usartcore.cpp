/*
 * ll_dmadrivenusartcore.cpp
 *
 *  Created on: Nov 4, 2019
 *      Author: abody
 */

#include "f1ll/usartcore.h"

namespace f1ll {

UsartCore::UsartCore(USART_TypeDef *usart, DMA_TypeDef *dma, uint32_t channelRx, uint32_t channelTx)
: m_usart(usart)
, m_rxDma(dma, channelRx)
, m_txDma(dma, channelTx)
{
	uint32_t status = usart->SR;
	volatile uint32_t tmpreg = usart->DR; // clearing some of the error/status bits in the USART
	(void) tmpreg;
	(void) status;

	*m_txDma.GetIfcReg() =
			m_txDma.GetGiMask();
	*m_rxDma.GetIfcReg() =
			m_rxDma.GetGiMask();

	LL_DMA_EnableIT_HT(dma, channelRx);
	LL_DMA_EnableIT_TC(dma, channelRx);
	LL_DMA_EnableIT_TE(dma, channelRx);
	LL_DMA_EnableIT_HT(dma, channelTx);
	LL_DMA_EnableIT_TC(dma, channelTx);
	LL_DMA_EnableIT_TE(dma, channelTx);
}


void UsartCore::UsartIsr()
{
	uint32_t status = m_usart->SR;
	volatile uint32_t tmpreg = m_usart->DR; // clearing some of the error/status bits in the HW
	(void) tmpreg;

	if(LL_USART_IsEnabledIT_TC(m_usart) && LL_USART_IsActiveFlag_TC(m_usart)) { // transmission complete
		LL_USART_DisableIT_TC(m_usart);
		TransmissionComplete();
	}
	if(LL_USART_IsEnabledIT_IDLE(m_usart) && (status & USART_SR_IDLE)) {
		ReceiverIdle();
	}
	if(LL_USART_IsEnabledIT_ERROR(m_usart)) {
		if(status & USART_SR_FE) {
			FramingError();
		}
		if(status & USART_SR_ORE) {
			Overrun();
		}
	}
}


void UsartCore::RxDmaIsr()
{
	if(*m_rxDma.GetIsReg() & m_rxDma.GetTcMask()) {
		*m_rxDma.GetIfcReg() = m_rxDma.GetTcMask();
		if(m_rxDma.IsEnabledIt_TC())
			RxDmaTransferComplete();
	}
	if(*m_rxDma.GetIsReg() & m_rxDma.GetHtMask()) {
		*m_rxDma.GetIfcReg() = m_rxDma.GetHtMask();
		if(m_rxDma.IsEnabledIt_HT())
			RxDmaHalfTransfer();
	}
	if(*m_rxDma.GetIsReg() & m_rxDma.GetTeMask()) {
		*m_rxDma.GetIfcReg() = m_rxDma.GetTeMask();
		if(m_rxDma.IsEnabledIt_TE())
			RxDmaError();
	}
}


void UsartCore::TxDmaIsr()
{
	if(*m_txDma.GetIsReg() & m_txDma.GetTcMask()) {	// DMA transfer complete
		*m_txDma.GetIfcReg() = m_txDma.GetTcMask();
		if(m_txDma.IsEnabledIt_TC())
			TxDmaTransferComplete();
	}
	if(*m_txDma.GetIsReg() & m_txDma.GetHtMask()) {
		*m_txDma.GetIfcReg() = m_txDma.GetHtMask();
		if(m_txDma.IsEnabledIt_HT())
			TxDmaHalfTransfer();
	}
	if(*m_txDma.GetIsReg() & m_txDma.GetTeMask()) {
		*m_txDma.GetIfcReg() = m_txDma.GetTeMask();
		if(m_txDma.IsEnabledIt_TE())
			TxDmaError();
	}
}


void UsartCore::SetupTransmit(void const *buffer, uint16_t length)
{
	LL_DMA_ConfigAddresses(m_txDma.GetDma(), m_txDma.GetChannel(), reinterpret_cast<uint32_t>(buffer),
			LL_USART_DMA_GetRegAddr(m_usart), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetDataLength(m_txDma.GetDma(), m_txDma.GetChannel(), length);
	LL_USART_EnableDMAReq_TX(m_usart);
	LL_DMA_EnableChannel(m_txDma.GetDma(), m_txDma.GetChannel());
}


void UsartCore::SetupReceive(void *buffer, uint16_t length)
{
	LL_DMA_ConfigAddresses(m_rxDma.GetDma(), m_rxDma.GetChannel(), LL_USART_DMA_GetRegAddr(m_usart),
			reinterpret_cast<uint32_t>(buffer), LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetDataLength(m_rxDma.GetDma(), m_rxDma.GetChannel(), length);
	LL_USART_EnableDMAReq_RX(m_usart);
	LL_USART_ClearFlag_ORE(m_usart);
	LL_DMA_EnableChannel(m_rxDma.GetDma(), m_rxDma.GetChannel());
}

} /* namespace f4ll */
