#include "vhp_command.h"

#include <vhp_registers.h>
#include <vhp_parser.h>

void VHPBuildGetRegisterPayload(uint16_t registerToGet, uint8_t flag, uint8_t* payloadBytes){
  payloadBytes[0]=HEXCMD_GET;
  payloadBytes[1]=(uint8_t)  (registerToGet&0x00FF); //TODO Convert to little endian properly
  payloadBytes[2]=(uint8_t) ((registerToGet&0xFF00)>>8); //TODO Convert to little endian properly
  payloadBytes[3]=flag;
}

std::string VHPCommandGetRegister(const uint16_t registerToGet, const uint8_t flag){
  std::string payloadBytes;
  payloadBytes.reserve(4);
  payloadBytes+=HEXCMD_GET;
  payloadBytes+=(uint8_t)  (registerToGet&0x00FF); //TODO Convert to little endian properly
  payloadBytes+=(uint8_t) ((registerToGet&0xFF00)>>8); //TODO Convert to little endian properly
  payloadBytes+=flag;
  return payloadBytes;
}

std::string VHPCommandGetRegister(const uint16_t registersToGet[], const uint8_t nbRegisters){
  std::string payloadBatch;
  for(int i=0; i<nbRegisters; i++){
    payloadBatch+=VHPCommandGetRegister(registersToGet[i], 0);
  }
  return payloadBatch;
}

void assertEquals(uint32_t expected, uint32_t actual, const char* failureMsg);
void testCommandBuilder(){
  printf("%s\n", __FUNCTION__);

  std::string singleCmd=VHPCommandGetRegister(VHP_REG_MODEL_NAME, 0);
  assertEquals(4, singleCmd.size(), "single command");
  hexdump("singleCmd", singleCmd.c_str(), singleCmd.size());

  const uint16_t startupRegisters[]={VHP_REG_PRODUCT_ID, VHP_REG_MODEL_NAME, VHP_REG_DEVICE_MODE};
  std::string startupCommand=VHPCommandGetRegister(startupRegisters, sizeof(startupRegisters)/sizeof(uint16_t));
  assertEquals(12, startupCommand.size(), "3 commands issued at startup");

  const uint16_t monitoringRegisters[]={VHP_REG_PANEL_POWER, VHP_REG_CHARGER_CURRENT, VHP_REG_CHARGER_VOLTAGE, VHP_REG_CHARGER_MAX_CURRENT};
  std::string monitorCommand=VHPCommandGetRegister(monitoringRegisters, sizeof(monitoringRegisters)/sizeof(uint16_t));
  assertEquals(16, monitorCommand.size(), "4 commands used to monitor the MPPT");
}
