#include "vhp_command.h"

#include <sstream>
#include <iomanip>

#include <vhp_registers.h>
#include <vhp_parser.h>
#include <vhp_traces.h>

#define TAG __FILE__

std::basic_string<unsigned char> VHPBuildCommandFromRegistersToGet(const uint16_t registerToGet, const uint8_t flag){
  std::basic_string<unsigned char> payloadBytes;
  payloadBytes.reserve(4);
  payloadBytes+=HEXCMD_GET;
  payloadBytes+=(uint8_t)  (registerToGet&0x00FF); //TODO Convert to little endian properly
  payloadBytes+=(uint8_t) ((registerToGet&0xFF00)>>8); //TODO Convert to little endian properly
  payloadBytes+=flag;
  return payloadBytes;
}

std::string VHPCommandBytesToHexString(const std::basic_string<uint8_t>& inputBytes){
  std::string hexLine=bytesToHex(inputBytes);
  hexLine[0]=':'; //Overwrite the leading HEX 0

  std::stringstream ss;
  ss << hexLine
     << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << 0+computeChecksum(inputBytes) //The 0+ here is a hack to get C++ to consider the uint8_t as integer a value, not a char.
     <<'\n';
  return ss.str();
}

std::string VHPBatchGetRegisters(const uint16_t registersToGet[], const uint8_t nbRegisters){
  std::string hexPayloadBatch;
  for(int i=0; i<nbRegisters; i++){
    std::basic_string<uint8_t> singlePayloadBytes=VHPBuildCommandFromRegistersToGet(registersToGet[i], 0);
    hexPayloadBatch+=VHPCommandBytesToHexString(singlePayloadBytes);
  }
  return hexPayloadBatch;
}

std::string bytesToHex(const std::basic_string<unsigned char>& bytes){
//  ESP_LOG_BUFFER_HEX_LEVEL(TAG, bytes.c_str(), bytes.size(), ESP_LOG_DEBUG);
  std::stringstream ss;

  for( int i(0) ; i < bytes.size(); ++i ) {
     ss << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << 0+bytes[i];
  }

  std::string hexBytes=ss.str();
  DEBUG_PRINTD(TAG, "hexBytes=%.*s", hexBytes.size(), hexBytes.c_str());
  return hexBytes;
}

void testCommandBuilder(){
  DEBUG_PRINTD(TAG, "%s\n", __FUNCTION__);
  return;

  std::basic_string<unsigned char> singleCmd=VHPBuildCommandFromRegistersToGet(VHP_REG_MODEL_NAME, 0);
  assertEquals(4, singleCmd.size(), "single command");
  hexdump("singleCmd", singleCmd.c_str(), singleCmd.size());

  const uint16_t startupRegisters[]={VHP_REG_PRODUCT_ID, VHP_REG_MODEL_NAME, VHP_REG_DEVICE_MODE};
  std::string startupCommand=VHPBatchGetRegisters(startupRegisters, sizeof(startupRegisters)/sizeof(uint16_t));
  assertEquals(12, startupCommand.size(), "3 commands issued at startup");

  const uint16_t monitoringRegisters[]={VHP_REG_PANEL_POWER, VHP_REG_CHARGER_CURRENT, VHP_REG_CHARGER_VOLTAGE, VHP_REG_CHARGER_MAX_CURRENT};
  std::string monitorCommand=VHPBatchGetRegisters(monitoringRegisters, sizeof(monitoringRegisters)/sizeof(uint16_t));
  assertEquals(16, monitorCommand.size(), "4 commands used to monitor the MPPT");

  std::basic_string<unsigned char> bytes({0x01,0x65,0xFE});
  std::string hexString=bytesToHex(bytes);

  hexdump("Hexdump of hexString.c_str", hexString.c_str(), 6);
  assertEquals("0165FE", hexString.c_str(), "bytesToHex");
}

