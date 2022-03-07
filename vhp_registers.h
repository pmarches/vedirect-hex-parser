#ifndef VHP_REGISTERS_H_
#define VHP_REGISTERS_H_

#include <inttypes.h>

enum REGISTER_ENCODING {
  UNSIGNED,
  SIGNED,
  BYTES,
};

typedef struct {
  uint32_t registerValue;
  float scale;
  uint8_t byteLen;
  REGISTER_ENCODING encoding;
  const char* unit;
  const char* desc;
} RegisterDesc;

const RegisterDesc* lookupRegister(uint16_t registerId);

typedef struct {
  const uint16_t productId;
  const char* productName;
} ProductDescription;

const ProductDescription* lookupProductId(uint16_t productId);

#endif /* VHP_REGISTERS_H_ */
