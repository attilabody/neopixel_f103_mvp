/*
q * ll_dmahelper.cpp
 *
 *  Created on: Oct 25, 2019
 *      Author: abody
 */

#include "f1ll/dmahelper.h"

namespace f1ll {

const uint32_t DmaHelper::m_TEMasks[7]  = {DMA_ISR_TEIF1,  DMA_ISR_TEIF2,  DMA_ISR_TEIF3,  DMA_ISR_TEIF4,  DMA_ISR_TEIF5,  DMA_ISR_TEIF6,  DMA_ISR_TEIF7};
const uint32_t DmaHelper::m_HTMasks[7]  = {DMA_ISR_HTIF1,  DMA_ISR_HTIF2,  DMA_ISR_HTIF3,  DMA_ISR_HTIF4,  DMA_ISR_HTIF5,  DMA_ISR_HTIF6,  DMA_ISR_HTIF7};
const uint32_t DmaHelper::m_TCMasks[7]  = {DMA_ISR_TCIF1,  DMA_ISR_TCIF2,  DMA_ISR_TCIF3,  DMA_ISR_TCIF4,  DMA_ISR_TCIF5,  DMA_ISR_TCIF6,  DMA_ISR_TCIF7};
const uint32_t DmaHelper::m_GIMasks[7]  = {DMA_ISR_GIF1,   DMA_ISR_GIF2,   DMA_ISR_GIF3,   DMA_ISR_GIF4,   DMA_ISR_GIF5,   DMA_ISR_GIF6,   DMA_ISR_GIF7};

DmaHelper::DmaHelper(DMA_TypeDef *dma, uint32_t channel)
: m_dma(dma)
, m_channel(channel)
#ifdef DMA2
, m_isReg(dma == DMA1 ? &DMA1->ISR : &DMA2->ISR)
, m_ifcReg(dma == DMA1 ? &DMA1->IFCR : &DMA2->IFCR)
#else
, m_isReg(&DMA1->ISR)
, m_ifcReg(&DMA1->IFCR)
#endif
{
}

} /* namespace f4ll */
