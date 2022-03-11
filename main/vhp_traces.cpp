#include <vhp_traces.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TAG __FILE__

void assertEquals(const char* expected, const char* actual, const char* failureMsg){
  if(strcmp(expected, actual)!=0){
    DEBUG_PRINTD(TAG,"TEST FAILURE: %s. Was expecting %s but got %s\n", failureMsg, expected, actual);
    exit(1);
  }
}

void assertEquals(uint32_t expected, uint32_t actual, const char* failureMsg){
  if(expected!=actual){
    DEBUG_PRINTD(TAG,"TEST FAILURE: %s. Was expecting %d but got %d\n", failureMsg, expected, actual);
    exit(1);
  }
}

void hexdump(const char* msg, const void *ptr, int buflen) {
  printf("%s len=%d\n", msg, buflen);
  unsigned char *buf = (unsigned char*)ptr;
  int i, j;
  for (i=0; i<buflen; i+=16) {
    printf("%06x: ", i);
    for (j=0; j<16; j++)
      if (i+j < buflen)
        printf("%02x ", buf[i+j]);
      else
        printf("   ");
    printf(" ");
    for (j=0; j<16; j++)
      if (i+j < buflen)
        printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
    printf("\n");
  }
}

