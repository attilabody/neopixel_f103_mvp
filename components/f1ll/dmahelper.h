/*
 * ll_dmahelper.h
 *
 *  Created on: Oct 25, 2019
 *      Author: abody
 */

#ifndef LL_DMAHELPER_H_
#define LL_DMAHELPER_H_

#include <inttypes.h>
#include <platform/dma_ll.h>

namespace f1ll {

class DmaHelper {
public:
	DmaHelper(DMA_TypeDef *dma, uint32_t channel);
	DmaHelper(DmaHelper const &base) = default;

	inline DMA_TypeDef* GetDma() const          { return m_dma; }
	inline uint32_t GetChannel() const          { return m_channel; }
	inline volatile uint32_t* GetIsReg() const 	{ return m_isReg; }
	inline volatile uint32_t* GetIfcReg() const { return m_ifcReg; }
	inline uint32_t GetTeMask() const           { return m_TEMasks[m_channel - 1]; }
	inline uint32_t GetHtMask() const           { return m_HTMasks[m_channel - 1]; }
	inline uint32_t GetTcMask() const           { return m_TCMasks[m_channel - 1]; }
	inline uint32_t GetGiMask() const           { return m_GIMasks[m_channel - 1]; }


	inline bool IsEnabledIt_TE()				{ return LL_DMA_IsEnabledIT_TE(m_dma, m_channel) != 0; }
	inline bool IsEnabledIt_HT()				{ return LL_DMA_IsEnabledIT_HT(m_dma, m_channel) != 0; }
	inline bool IsEnabledIt_TC()				{ return LL_DMA_IsEnabledIT_TC(m_dma, m_channel) != 0; }

private:
	DMA_TypeDef			*m_dma;
	uint32_t 			 m_channel;
	volatile uint32_t	*m_isReg;
	volatile uint32_t	*m_ifcReg;

	static const uint32_t m_TEMasks[7];
	static const uint32_t m_HTMasks[7];
	static const uint32_t m_TCMasks[7];
	static const uint32_t m_GIMasks[7];
};

} /* namespace f4ll */

#endif /* LL_DMAHELPER_H_ */
