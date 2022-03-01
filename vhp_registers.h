#ifndef VHP_REGISTERS_H_
#define VHP_REGISTERS_H_

enum REGISTER_TYPES {
  un8,
  un16,
  sn16,
  un32,
  sn32,
};

typedef struct {
  uint32_t registerValue;
  float scale;
  REGISTER_TYPES byteLen;
  const char* unit;
  const char* desc;
} RegisterDesc;

RegisterDesc registerDescriptions[]={
    {0x0200, 1, un8, "Device mode"},
    {0x0201, 1, un8, "Device state"},
    {0x0202, 1, un32, "Remote control used"},
    {0x0205, 1, un8, "Device off reason1"},
    {0x0207, 1, un32, "Device off reason2"},

    //Battery settings registers
    {0xEDFF, 1   , un8 , "0=off, 1=on", "Batterysafe mode"},
    {0xEDFE, 1   , un8 , "0=off, 1=on", "Adaptive mode"},
    {0xEDFD, 1   , un8 , "0=off, 1=on", "Automatic equalisation mode"},
    {0xEDFC, 0.01, un16, "0=off, 1..250", "Battery bulk time limit"},
    {0xEDFB, 0.01, un16, "hours", "Battery absorption time limit"},
    {0xEDF7, 0.01, un16, "hours", "Battery absorption voltage"},
    {0xEDF6, 0.01, un16, "V", "Battery float voltage"},
    {0xEDF4, 0.01, un16, "V", "Battery equalisation voltage"},
    {0xEDF2, 0.01, sn16, "V", "Battery temp. compensation"},
    {0xEDF1, 1   , un8 , "mV/K", "Battery type "},
    {0xEDF0, 0.10, un16, "0xFF = user", "Battery maximum current "},
    {0xEDEF, 1   , un8 , "A", "Battery voltage"},
    {0xEDEC, 0.01, un16, "V", "Battery temperature"},
    {0xEDEA, 1   , un8 , "K, 0xFFFF=N/A", "Battery voltage setting"},
    {0xEDE8, 1   , un8 , "V", "BMS present"},
    {0xEDE7, 0.10, un16, "0=no, 1=yes", "Tail current"},
    {0xEDE6, 0.10, un16, "A, 0xFFFF=use max", "Low temperature charge current"},
    {0xEDE5, 1   , un8 , "0=no, 1=yes", "Auto equalise stop on voltage"},
    {0xEDE4, 1   , un8 , "% (of 0xEDF0)", "Equalisation current level"},
    {0xEDE3, 0.01, un16, "hours", "Equalisation duration"},
    {0xED2E, 0.01, un16, "V", "Re-bulk voltage offset"},
    {0xEDE0, 0.01, sn16, "°C", "Battery low temperature level"},
    {0xEDCA, 0.01, un16, "V", "Voltage compensation"},

    //Charger regsiters
    {0xEDEC, 0.01, un16, "K", "Battery temperature"},
    {0xEDDF, 0.1 , un16, "A", "Charger maximum current"},
    {0xEDDD, 0.01, un32, "kWh", "System yield"},
    {0xEDDC, 0.01, un32, "kWh", "User yield (resettable)"},
    {0xEDDB, 0.01, sn16, "°C", "Charger internal temperature"},
    {0xEDDA, 1   , un8 , "-", "Charger error code"},
    {0xEDD7, 0.1 , un16, "A", "Charger current"},
    {0xEDD5, 0.01, un16, "V", "Charger voltage"},
    {0xEDD4, 1   , un8 , "-", "Additional charger state info"},
    {0xEDD3, 0.01, un16, "kWh", "Yield today"},
    {0xEDD2, 1   , un16, "W", "Maximum power today"},
    {0xEDD1, 0.01, un16, "kWh", "Yield yesterday"},
    {0xEDD0, 1   , un16, "W", "Maximum power yesterday"},
    {0xEDCE, 1   , un16, "-", "Voltage settings range"},
    {0xEDCD, 1   , un8 , "-", "History version"},
    {0xEDCC, 1   , un8 , "-", "Streetlight version"},
    {0x2211, 0.01, un16, "V", "Adjustable voltage minimum"},
    {0x2212, 0.01, un16, "V", "Adjustable voltage maximum"},

    //Solar panel registers
    {0xEDBC, 0.01, un32, "W", "Panel power"},
    {0xEDBB, 0.01, un16, "V", "Panel voltage"},
    {0xEDBD, 0.1 , un16, "A", "Panel current"},
    {0xEDB8, 0.01, un16, "V", "Panel maximum voltage"},
    {0xEDB3, 1   , un8 , "-", "Tracker mode"},

    //Load output registers
    {0xEDAD, 0.1 , un16, "A ", "Load current "},
    {0xEDAC, 0.01, un8 , "V ", "Load offset voltage "},
    {0xEDAB, 1   , un8 , "- ", "Load output control "},
    {0xEDA9, 0.01, un16, "V ", "Load output voltage "},
    {0xEDA8, 1   , un8 , "- ", "Load output state "},
    {0xED9D, 0.01, un16, "V ", "Load switch high level "},
    {0xED9C, 0.01, un16, "V ", "Load switch low level "},
    {0xED91, 1   , un8 , "- ", "Load output off reason"},
    {0xED90, 1   , un16, "minute", "Load AES timer "},

    //Relay settings registers
    {0xEDD9, 1   , un8 , "- ", "Relay operation mode "},
    {0x0350, 0.01, un16, "V ", "Relay battery low voltage set "},
    {0x0351, 0.01, un16, "V ", "Relay battery low voltage clear "},
    {0x0352, 0.01, un16, "V ", "Relay battery high voltage set "},
    {0x0353, 0.01, un16, "V ", "Relay battery high voltage clear"},
    {0xEDBA, 0.01, un16, "V ", "Relay panel high voltage set "},
    {0xEDB9, 0.01, un16, "V ", "Relay panel high voltage clear "},
    {0x100A, 1   , un16, "minute", "Relay minimum enabled time "},

    //Lighting controller timer
    {0xEDA0, 1   , un32, "-", "Timer events 0"},
    {0xEDA1, 1   , un32, "-", "Timer events 1"},
    {0xEDA2, 1   , un32, "-", "Timer events 2"},
    {0xEDA3, 1   , un32, "-", "Timer events 3"},
    {0xEDA4, 1   , un32, "-", "Timer events 4"},
    {0xEDA5, 1   , un32, "-", "Timer events 5"},
    {0xEDA7, 1   , sn16, "min ", "Mid-point shift (*1) "},
    {0xED9B, 1   , un8 , "s ", "Gradual dim speed (*2) "},
    {0xED9A, 0.01, un16, "V ", "Panel voltage night (*3)"},
    {0xED99, 0.01, un16, "V ", "Panel voltage day (*3) "},
    {0xED96, 1   , un16, "min ", "Sunset delay (*4) "},
    {0xED97, 1   , un16, "min ", "Sunrise delay (*4) "},
    {0xED90, 1   , un16, "min ", "AES Timer "},
    {0x2030, 1   , un8 , "0=dark, 1=light ", "Solar activity "},
    {0x2031, 1   , un16, "min, 0=mid-night", "Time-of-day (*5) "},

    //VE.Direct port functions
    {0xED9E, 1, un8, "", "TX Port operation mode"},
    {0xED98, 1, un8, "", "RX Port operation mode"},

    //History data
//    {0x1030, 1, 0, "", "Clear history"},
//    {0x104F, 1, 34, "", "Total history"},
//    {0x1050, 1, 34, "", "Daily history (0x1050=today, 0x1051=yesterday, ...)"},
    //up to 0x106E


    //Pluggable display settings
    {0x0400, 1, un8, "", "Display backlight mode (0 = keypress, 1 = on, 2 = auto)"},
    {0x0401, 1, un8, "", "Display backlight intensity (0 = always off, 1 = on)   "},
    {0x0402, 1, un8, "", "Display scroll text speed (1 = slow, 5 = fast)         "},
    {0x0403, 1, un8, "", "Display setup lock (0 = unlocked, 1 = locked) (*2)     "},
    {0x0404, 1, un8, "", "Display temperature unit (0 = Celsius, 1 = Fahrenheit) "},


    //Remote control registers - firmware v1.29 or higher
    {0x2000, 1    , un8 , "-    ", "Charge algorithm version (*1,*8)  "},
    {0x2001, 0.01 , un16, "V    ", "Charge voltage set-point (*2)     "},
    {0x2002, 0.01 , un16, "V    ", "Battery voltage sense (*3)        "},
    {0x2003, 0.01 , sn16, "°C   ", "Battery temperature sense (*3)    "},
    {0x2004, 1    , un8 , "-    ", "Remote command                    "},
    {0x2007, 1    , un32, "ms   ", "Charge state elapsed time (*4, *8)"},
    {0x2008, 0.01 , un16, "hours", "Absorption time (*4, *8)          "},
    {0x2009, 1    , un8 , "-    ", "Error code (*8)                   "},
    {0x200A, 0.001, sn32, "A    ", "Battery charge current (*8)       "},
    {0x200B, 0.01 , un16, "V    ", "Battery idle voltage (*8)         "},
    {0x200C, 1    , un8 , "-    ", "Device state (*8)                 "},
    {0x200D, 1    , un8 , "-    ", "Network info (*7)                 "},
    {0x200E, 1    , un8 , "-    ", "Network mode                      "},
    {0x200F, 1    , un8 , "-    ", "Network status register           "},
    {0x2013, 0.001, sn32, "A    ", "Total charge current (*8)         "},
    {0x2014, 1    , un8 , "%    ", "Charge current percentage (*5)    "},
    {0x2015, 0.1  , un16, "A    ", "Charge current limit (*6)         "},
    {0x2018, 1    , un8 , "-    ", "Manual equalisation pending (*8)  "},
    {0x2027, 0.01 , un32, "W    ", "Total DC input power (*8)         "},

};


#endif /* VHP_REGISTERS_H_ */
