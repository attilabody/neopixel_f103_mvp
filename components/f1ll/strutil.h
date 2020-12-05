/*
 * strutil.h
 *
 *  Created on: Feb 11, 2017
 *      Author: compi
 */

#ifndef _STM32PLUS_STRUTIL_H_
#define _STM32PLUS_STRUTIL_H_

#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
size_t strcpy_ex(char *dst, char const *src);
size_t uitodec(char* buffer, uint32_t data);
size_t uitohex(char* buffer, uint32_t data, uint8_t chars);
size_t itodec(char* buffer, int data);
size_t itohex(char* buffer, int data);
void strrev(char *first, char *last);
char tochr(const uint8_t in, const uint8_t upper);

#ifdef __cplusplus
}
#endif

#endif /* _STM32PLUS_STRUTIL_H_ */
