#ifndef VHP_TRACES_H_
#define VHP_TRACES_H_

#include <inttypes.h>
#include <stdio.h>

#ifdef ESP_PLATFORM
#include <esp_log.h>
#define DEBUG_PRINTD ESP_LOGD
#else
#define DEBUG_PRINTD printf
//#define DEBUG_PRINTD //
#endif

void assertEquals(const char* expected, const char* actual, const char* failureMsg);
void assertEquals(uint32_t expected, uint32_t actual, const char* failureMsg);
void hexdump(const char* msg, const void *ptr, int buflen);

#endif /* VHP_TRACES_H_ */
