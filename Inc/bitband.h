/*
 * bitband.h
 *
 *  Created on: Dec 14, 2018
 *      Author: abody
 */

#ifndef BITBAND_H_
#define BITBAND_H_

#include <inttypes.h>

#define RAM_BASE 0x20000000
#define RAM_BB_BASE 0x22000000
#define RAM_ResetBit_BB(VarAddr, BitNumber) (*(volatile uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 0)
#define RAM_SetBit_BB(VarAddr, BitNumber) (*(volatile uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)) = 1)
#define RAM_GetBit_BB(VarAddr, BitNumber) (*(volatile uint32_t *) (RAM_BB_BASE | ((VarAddr - RAM_BASE) << 5) | ((BitNumber) << 2)))
#define BITBAND_RAM(address, bit) ( (__IO uint32_t *) (RAM_BB_BASE + (((uint32_t)address) - RAM_BASE) * 32 + (bit) * 4))

#define IO_BASE 0x20000000
#define IO_BB_BASE 0x22000000
#define IO_ResetBit_BB(VarAddr, BitNumber) (*(volatile uint32_t *) (IO_BB_BASE | ((VarAddr - IO_BASE) << 5) | ((BitNumber) << 2)) = 0)
#define IO_SetBit_BB(VarAddr, BitNumber) (*(volatile uint32_t *) (IO_BB_BASE | ((VarAddr - IO_BASE) << 5) | ((BitNumber) << 2)) = 1)
#define IO_GetBit_BB(VarAddr, BitNumber) (*(volatile uint32_t *) (IO_BB_BASE | ((VarAddr - IO_BASE) << 5) | ((BitNumber) << 2)))
#define BITBAND_IO(address, bit) ( (__IO uint32_t *) (IO_BB_BASE + (((uint32_t)address) - IO_BASE) * 32 + (bit) * 4))

#endif /* BITBAND_H_ */
