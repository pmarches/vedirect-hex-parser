#ifndef MAIN_VHP_COMMAND_H_
#define MAIN_VHP_COMMAND_H_

#include <string>
#include <inttypes.h>

std::string bytesToHex(const std::basic_string<unsigned char>& bytes);
std::string VHPCommandBytesToHexString(const std::basic_string<uint8_t>& inputBytes);

std::basic_string<unsigned char> VHPBuildCommandFromRegistersToGet(const uint16_t registerToGet, const uint8_t flag);
std::string VHPBatchGetRegisters(const uint16_t registersToGet[], const uint8_t nbRegisters);

void testCommandBuilder();

#endif /* MAIN_VHP_COMMAND_H_ */
