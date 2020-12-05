/*
 * ll_dmadrivenusartcore.h
 *
 *  Created on: Nov 4, 2019
 *      Author: abody
 */

#ifndef LL_USARTCORE_H_
#define LL_USARTCORE_H_
#include <platform/usart_ll.h>

#include "f1ll/dmahelper.h"

namespace f1ll {

class UsartCore
{
public:
	static inline void HandleUsartIrq(UsartCore *_this) { _this->UsartIsr(); }
	static inline void HandleRxDmaIrq(UsartCore *_this) { _this->RxDmaIsr(); }
	static inline void HandleTxDmaIrq(UsartCore *_this) { _this->TxDmaIsr(); }

	void SetupTransmit(void const *buffer, uint16_t length);
	void SetupReceive(void *buffer, uint16_t length);

protected:
	UsartCore(USART_TypeDef *usart, DMA_TypeDef *dma, uint32_t channelRx, uint32_t streamTx);

	USART_TypeDef *m_usart;
	DmaHelper      m_rxDma;
	DmaHelper      m_txDma;

private:
	virtual void ReceiverIdle(void)                         = 0;
	virtual void TransmissionComplete(void)                 = 0;
	virtual void FramingError(void)                         = 0;
	virtual void Overrun(void)                              = 0;

	virtual void RxDmaTransferComplete(void)                = 0;
	virtual void RxDmaHalfTransfer(void)                    = 0;
	virtual void RxDmaError(void) = 0;

	virtual void TxDmaTransferComplete(void)                = 0;
	virtual void TxDmaHalfTransfer(void)                    = 0;
	virtual void TxDmaError(void) = 0;

	void UsartIsr();
	void RxDmaIsr();
	void TxDmaIsr();

};

} /* namespace f4ll */

#endif /* LL_USARTCORE_H_ */
