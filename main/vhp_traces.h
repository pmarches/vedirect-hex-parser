#ifndef VHP_TRACES_H_
#define VHP_TRACES_H_

#include <inttypes.h>
#include <stdio.h>

#if 1
#define DEBUG printf
#else
#define DEBUG //
#endif

void assertEquals(const char* expected, const char* actual, const char* failureMsg);
void assertEquals(uint32_t expected, uint32_t actual, const char* failureMsg);
void hexdump(const char* msg, const void *ptr, int buflen);

#endif /* VHP_TRACES_H_ */
