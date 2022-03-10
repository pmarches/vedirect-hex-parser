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

const uint16_t VHP_REG_PRODUCT_ID=0x0100;
const uint16_t VHP_REG_GROUP_ID=0x0104;
const uint16_t VHP_REG_SERIAL=0x010A;
const uint16_t VHP_REG_MODEL_NAME=0x010B;
const uint16_t VHP_REG_DEVICE_MODE=0x0200;
const uint16_t VHP_REG_DEVICE_STATE=0x0201;

const uint16_t VHP_REG_BATT_TEMP=0xEDEC;
const uint16_t VHP_REG_CHARGER_MAX_CURRENT=0xEDDF;
const uint16_t VHP_REG_SYSTEM_YIELD=0xEDDD;
const uint16_t VHP_REG_USER_YIELD=0xEDDC;
const uint16_t VHP_REG_CHARGER_INTERNAL_TEMP=0xEDDB;
const uint16_t VHP_REG_CHARGER_ERROR_CODE=0xEDDA;
const uint16_t VHP_REG_CHARGER_CURRENT=0xEDD7;
const uint16_t VHP_REG_CHARGER_VOLTAGE=0xEDD5;
const uint16_t VHP_REG_YIELD_TODAY=0xEDD3;
const uint16_t VHP_REG_MAX_POWER_TODAY=0xEDD2;
const uint16_t VHP_REG_YIELD_YESTERDAY=0xEDD1;
const uint16_t VHP_REG_MAX_POWER_YESTERDAY=0xEDD0;

const uint16_t VHP_REG_PANEL_POWER=0xEDBC;
const uint16_t VHP_REG_PANEL_VOLTAGE=0xEDBB;
const uint16_t VHP_REG_PANEL_CURRENT=0xEDBD;
const uint16_t VHP_REG_PANEL_MAX_VOLTAGE=0xEDB8;
const uint16_t VHP_REG_PANEL_TRACKER_MODE=0xEDB3;
const uint16_t VHP_REG_DAILY_HISTORY_TOTAL=0x104F;
const uint16_t VHP_REG_DAILY_HISTORY_MINUS_ZERO=0x1050;

#endif /* VHP_REGISTERS_H_ */
