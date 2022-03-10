#ifndef MAIN_VHP_COMMAND_H_
#define MAIN_VHP_COMMAND_H_

#include <string>
#include <inttypes.h>

std::string bytesToHex(const std::basic_string<unsigned char>& bytes);
std::basic_string<unsigned char> VHPCommandGetRegister(const uint16_t registerToGet, const uint8_t flag);
std::basic_string<unsigned char> VHPCommandGetRegister(const uint16_t registersToGet[], const uint8_t nbRegisters);

void testCommandBuilder();

#endif /* MAIN_VHP_COMMAND_H_ */
