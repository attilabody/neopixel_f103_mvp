/*
 * ll_consolehandler.h
 *
 *  Created on: Nov 7, 2019
 *      Author: abody
 */

#ifndef LL_CONSOLEHANDLER_H_
#define LL_CONSOLEHANDLER_H_

#include "f1ll/usartcore.h"
#include "singleton.h"


namespace f1ll {

class ConsoleHandler: public UsartCore, public Singleton<ConsoleHandler>
{
	friend class Singleton<ConsoleHandler>;

public:
	void Print(char const *s);

private:
	ConsoleHandler(USART_TypeDef *usart, DMA_TypeDef *dma, uint32_t channelRx, uint32_t channelTx);

	// LL_UsartCore pure virtual function implementations
	virtual void ReceiverIdle(void);
	virtual void TransmissionComplete(void);
	virtual void FramingError(void);
	virtual void Overrun(void);
	virtual void RxDmaTransferComplete(void);
	virtual void RxDmaHalfTransfer(void);
	virtual void RxDmaError(void);
	virtual void TxDmaTransferComplete(void);
	virtual void TxDmaHalfTransfer(void);
	virtual void TxDmaError(void);

	char		m_buffer[128];
	uint16_t	m_used = 0;
};

} /* namespace f4ll */

#endif /* LL_CONSOLEHANDLER_H_ */
