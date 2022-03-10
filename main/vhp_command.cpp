#include "vhp_command.h"

#include <sstream>
#include <iomanip>

#include <vhp_registers.h>
#include <vhp_parser.h>
#include <vhp_traces.h>

std::basic_string<unsigned char> VHPCommandGetRegister(const uint16_t registerToGet, const uint8_t flag){
  std::basic_string<unsigned char> payloadBytes;
  payloadBytes.reserve(4);
  payloadBytes+=HEXCMD_GET;
  payloadBytes+=(uint8_t)  (registerToGet&0x00FF); //TODO Convert to little endian properly
  payloadBytes+=(uint8_t) ((registerToGet&0xFF00)>>8); //TODO Convert to little endian properly
  payloadBytes+=flag;
  return payloadBytes;
}

std::basic_string<unsigned char> VHPCommandGetRegister(const uint16_t registersToGet[], const uint8_t nbRegisters){
  std::basic_string<unsigned char> payloadBatch;
  for(int i=0; i<nbRegisters; i++){
    payloadBatch+=VHPCommandGetRegister(registersToGet[i], 0);
  }
  return payloadBatch;
}

std::string bytesToHex(const std::basic_string<unsigned char>& bytes){
  std::stringstream ss;
  ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase;

  for( int i(0) ; i < bytes.size(); ++i ) {
     ss << 0+bytes[i];
  }

  return ss.str();
}

void testCommandBuilder(){
  printf("%s\n", __FUNCTION__);

  std::basic_string<unsigned char> singleCmd=VHPCommandGetRegister(VHP_REG_MODEL_NAME, 0);
  assertEquals(4, singleCmd.size(), "single command");
  hexdump("singleCmd", singleCmd.c_str(), singleCmd.size());

  const uint16_t startupRegisters[]={VHP_REG_PRODUCT_ID, VHP_REG_MODEL_NAME, VHP_REG_DEVICE_MODE};
  std::basic_string<unsigned char> startupCommand=VHPCommandGetRegister(startupRegisters, sizeof(startupRegisters)/sizeof(uint16_t));
  assertEquals(12, startupCommand.size(), "3 commands issued at startup");

  const uint16_t monitoringRegisters[]={VHP_REG_PANEL_POWER, VHP_REG_CHARGER_CURRENT, VHP_REG_CHARGER_VOLTAGE, VHP_REG_CHARGER_MAX_CURRENT};
  std::basic_string<unsigned char> monitorCommand=VHPCommandGetRegister(monitoringRegisters, sizeof(monitoringRegisters)/sizeof(uint16_t));
  assertEquals(16, monitorCommand.size(), "4 commands used to monitor the MPPT");

  std::basic_string<unsigned char> bytes({0x01,0x65,0xFE});
  std::string hexString=bytesToHex(bytes);

  hexdump("Hexdump of hexString.c_str", hexString.c_str(), 6);
  assertEquals("0165FE", hexString.c_str(), "bytesToHex");
}

