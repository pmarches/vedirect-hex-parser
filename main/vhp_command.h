#ifndef MAIN_VHP_COMMAND_H_
#define MAIN_VHP_COMMAND_H_

#include <string>
#include <inttypes.h>

void VHPBuildGetRegisterPayload(uint16_t registerToGet, uint8_t flag, uint8_t* payloadBytes);

std::string VHPCommandGetRegister(const uint16_t registerToGet, const uint8_t flag);
std::string VHPCommandGetRegister(const uint16_t registersToGet[], const uint8_t nbRegisters);

void testCommandBuilder();

#endif /* MAIN_VHP_COMMAND_H_ */
