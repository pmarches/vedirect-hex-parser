#ifndef VHP_REGISTERS_H_
#define VHP_REGISTERS_H_

enum REGISTER_ENCODING {
  UNSIGNED,
  SIGNED,
  CHAR,
};

typedef struct {
  uint32_t registerValue;
  float scale;
  uint8_t byteLen;
  REGISTER_ENCODING encoding;
  const char* unit;
  const char* desc;
} RegisterDesc;

const RegisterDesc registerDescriptions[]={
    {0x0200, 1, 1, UNSIGNED, "Device mode"},
    {0x0201, 1, 1, UNSIGNED, "Device state"},
    {0x0202, 1, 4, UNSIGNED, "Remote control used"},
    {0x0205, 1, 1, UNSIGNED, "Device off reason1"},
    {0x0207, 1, 4, UNSIGNED, "Device off reason2"},

    //Battery settings registers
    {0xEDFF, 1   , 1, UNSIGNED , "0=off, 1=on", "Batterysafe mode"},
    {0xEDFE, 1   , 1, UNSIGNED , "0=off, 1=on", "Adaptive mode"},
    {0xEDFD, 1   , 1, UNSIGNED , "0=off, 1=on", "Automatic equalisation mode"},
    {0xEDFC, 0.01, 2, UNSIGNED, "0=off, 1..250", "Battery bulk time limit"},
    {0xEDFB, 0.01, 2, UNSIGNED, "hours", "Battery absorption time limit"},
    {0xEDF7, 0.01, 2, UNSIGNED, "hours", "Battery absorption voltage"},
    {0xEDF6, 0.01, 2, UNSIGNED, "V", "Battery float voltage"},
    {0xEDF4, 0.01, 2, UNSIGNED, "V", "Battery equalisation voltage"},
    {0xEDF2, 0.01, 2, SIGNED, "V", "Battery temp. compensation"},
    {0xEDF1, 1   , 1, UNSIGNED , "mV/K", "Battery type "},
    {0xEDF0, 0.10, 2, UNSIGNED, "0xFF = user", "Battery maximum current "},
    {0xEDEF, 1   , 1, UNSIGNED , "A", "Battery voltage"},
    {0xEDEC, 0.01, 2, UNSIGNED, "V", "Battery temperature"},
    {0xEDEA, 1   , 1, UNSIGNED , "K, 0xFFFF=N/A", "Battery voltage setting"},
    {0xEDE8, 1   , 1, UNSIGNED , "V", "BMS present"},
    {0xEDE7, 0.10, 2, UNSIGNED, "0=no, 1=yes", "Tail current"},
    {0xEDE6, 0.10, 2, UNSIGNED, "A, 0xFFFF=use max", "Low temperature charge current"},
    {0xEDE5, 1   , 1, UNSIGNED , "0=no, 1=yes", "Auto equalise stop on voltage"},
    {0xEDE4, 1   , 1, UNSIGNED , "% (of 0xEDF0)", "Equalisation current level"},
    {0xEDE3, 0.01, 2, UNSIGNED, "hours", "Equalisation duration"},
    {0xED2E, 0.01, 2, UNSIGNED, "V", "Re-bulk voltage offset"},
    {0xEDE0, 0.01, 2, SIGNED, "°C", "Battery low temperature level"},
    {0xEDCA, 0.01, 2, UNSIGNED, "V", "Voltage compensation"},

    //Charger regsiters
    {0xEDEC, 0.01, 2, UNSIGNED, "K", "Battery temperature"},
    {0xEDDF, 0.1 , 2, UNSIGNED, "A", "Charger maximum current"},
    {0xEDDD, 0.01, 4, UNSIGNED, "kWh", "System yield"},
    {0xEDDC, 0.01, 4, UNSIGNED, "kWh", "User yield (resettable)"},
    {0xEDDB, 0.01, 2, SIGNED, "°C", "Charger internal temperature"},
    {0xEDDA, 1   , 1, UNSIGNED , "-", "Charger error code"},
    {0xEDD7, 0.1 , 2, UNSIGNED, "A", "Charger current"},
    {0xEDD5, 0.01, 2, UNSIGNED, "V", "Charger voltage"},
    {0xEDD4, 1   , 1, UNSIGNED , "-", "Additional charger state info"},
    {0xEDD3, 0.01, 2, UNSIGNED, "kWh", "Yield today"},
    {0xEDD2, 1   , 2, UNSIGNED, "W", "Maximum power today"},
    {0xEDD1, 0.01, 2, UNSIGNED, "kWh", "Yield yesterday"},
    {0xEDD0, 1   , 2, UNSIGNED, "W", "Maximum power yesterday"},
    {0xEDCE, 1   , 2, UNSIGNED, "-", "Voltage settings range"},
    {0xEDCD, 1   , 1, UNSIGNED , "-", "History version"},
    {0xEDCC, 1   , 1, UNSIGNED , "-", "Streetlight version"},
    {0x2211, 0.01, 2, UNSIGNED, "V", "Adjustable voltage minimum"},
    {0x2212, 0.01, 2, UNSIGNED, "V", "Adjustable voltage maximum"},

    //Solar panel registers
    {0xEDBC, 0.01, 4, UNSIGNED, "W", "Panel power"},
    {0xEDBB, 0.01, 2, UNSIGNED, "V", "Panel voltage"},
    {0xEDBD, 0.1 , 2, UNSIGNED, "A", "Panel current"},
    {0xEDB8, 0.01, 2, UNSIGNED, "V", "Panel maximum voltage"},
    {0xEDB3, 1   , 1, UNSIGNED , "-", "Tracker mode"},

    //Load output registers
    {0xEDAD, 0.1 , 2, UNSIGNED, "A ", "Load current "},
    {0xEDAC, 0.01, 1, UNSIGNED , "V ", "Load offset voltage "},
    {0xEDAB, 1   , 1, UNSIGNED , "- ", "Load output control "},
    {0xEDA9, 0.01, 2, UNSIGNED, "V ", "Load output voltage "},
    {0xEDA8, 1   , 1, UNSIGNED , "- ", "Load output state "},
    {0xED9D, 0.01, 2, UNSIGNED, "V ", "Load switch high level "},
    {0xED9C, 0.01, 2, UNSIGNED, "V ", "Load switch low level "},
    {0xED91, 1   , 1, UNSIGNED , "- ", "Load output off reason"},
    {0xED90, 1   , 2, UNSIGNED, "minute", "Load AES timer "},

    //Relay settings registers
    {0xEDD9, 1   , 1, UNSIGNED , "- ", "Relay operation mode "},
    {0x0350, 0.01, 2, UNSIGNED, "V ", "Relay battery low voltage set "},
    {0x0351, 0.01, 2, UNSIGNED, "V ", "Relay battery low voltage clear "},
    {0x0352, 0.01, 2, UNSIGNED, "V ", "Relay battery high voltage set "},
    {0x0353, 0.01, 2, UNSIGNED, "V ", "Relay battery high voltage clear"},
    {0xEDBA, 0.01, 2, UNSIGNED, "V ", "Relay panel high voltage set "},
    {0xEDB9, 0.01, 2, UNSIGNED, "V ", "Relay panel high voltage clear "},
    {0x100A, 1   , 2, UNSIGNED, "minute", "Relay minimum enabled time "},

    //Lighting controller timer
    {0xEDA0, 1   , 4, UNSIGNED, "-", "Timer events 0"},
    {0xEDA1, 1   , 4, UNSIGNED, "-", "Timer events 1"},
    {0xEDA2, 1   , 4, UNSIGNED, "-", "Timer events 2"},
    {0xEDA3, 1   , 4, UNSIGNED, "-", "Timer events 3"},
    {0xEDA4, 1   , 4, UNSIGNED, "-", "Timer events 4"},
    {0xEDA5, 1   , 4, UNSIGNED, "-", "Timer events 5"},
    {0xEDA7, 1   , 2, SIGNED, "min ", "Mid-point shift (*1) "},
    {0xED9B, 1   , 1, UNSIGNED , "s ", "Gradual dim speed (*2) "},
    {0xED9A, 0.01, 2, UNSIGNED, "V ", "Panel voltage night (*3)"},
    {0xED99, 0.01, 2, UNSIGNED, "V ", "Panel voltage day (*3) "},
    {0xED96, 1   , 2, UNSIGNED, "min ", "Sunset delay (*4) "},
    {0xED97, 1   , 2, UNSIGNED, "min ", "Sunrise delay (*4) "},
    {0xED90, 1   , 2, UNSIGNED, "min ", "AES Timer "},
    {0x2030, 1   , 1, UNSIGNED , "0=dark, 1=light ", "Solar activity "},
    {0x2031, 1   , 2, UNSIGNED, "min, 0=mid-night", "Time-of-day (*5) "},

    //VE.Direct port functions
    {0xED9E, 1, 1, UNSIGNED, "", "TX Port operation mode"},
    {0xED98, 1, 1, UNSIGNED, "", "RX Port operation mode"},

    //History data
    {0x1030, 1, 0, UNSIGNED, "", "Clear history"},
    {0x104F, 1, 34, CHAR, "", "Total history"},
    {0x1050, 1, 34, CHAR, "", "Daily history-0"},
    {0x1051, 1, 34, CHAR, "", "Daily history-1"},
    {0x1052, 1, 34, CHAR, "", "Daily history-2"},
    {0x1053, 1, 34, CHAR, "", "Daily history-3"},
    {0x1054, 1, 34, CHAR, "", "Daily history-4"},
    {0x1055, 1, 34, CHAR, "", "Daily history-5"},
    {0x1056, 1, 34, CHAR, "", "Daily history-6"},
    {0x1057, 1, 34, CHAR, "", "Daily history-7"},
    {0x1058, 1, 34, CHAR, "", "Daily history-8"},
    {0x1059, 1, 34, CHAR, "", "Daily history-9"},
    {0x105A, 1, 34, CHAR, "", "Daily history-10"},
    {0x105B, 1, 34, CHAR, "", "Daily history-11"},
    {0x105C, 1, 34, CHAR, "", "Daily history-12"},
    {0x105D, 1, 34, CHAR, "", "Daily history-13"},
    {0x105E, 1, 34, CHAR, "", "Daily history-14"},
    {0x105F, 1, 34, CHAR, "", "Daily history-15"},
    {0x1060, 1, 34, CHAR, "", "Daily history-16"},
    {0x1061, 1, 34, CHAR, "", "Daily history-17"},
    {0x1062, 1, 34, CHAR, "", "Daily history-18"},
    {0x1063, 1, 34, CHAR, "", "Daily history-19"},
    {0x1064, 1, 34, CHAR, "", "Daily history-20"},
    {0x1065, 1, 34, CHAR, "", "Daily history-21"},
    {0x1066, 1, 34, CHAR, "", "Daily history-22"},
    {0x1067, 1, 34, CHAR, "", "Daily history-23"},
    {0x1068, 1, 34, CHAR, "", "Daily history-24"},
    {0x1069, 1, 34, CHAR, "", "Daily history-25"},
    {0x106A, 1, 34, CHAR, "", "Daily history-26"},
    {0x106B, 1, 34, CHAR, "", "Daily history-27"},
    {0x106C, 1, 34, CHAR, "", "Daily history-28"},
    {0x106D, 1, 34, CHAR, "", "Daily history-29"},
    {0x106E, 1, 34, CHAR, "", "Daily history-30"},

    //Pluggable display settings
    {0x0400, 1, 1, UNSIGNED, "", "Display backlight mode (0 = keypress, 1 = on, 2 = auto)"},
    {0x0401, 1, 1, UNSIGNED, "", "Display backlight intensity (0 = always off, 1 = on) "},
    {0x0402, 1, 1, UNSIGNED, "", "Display scroll text speed (1 = slow, 5 = fast) "},
    {0x0403, 1, 1, UNSIGNED, "", "Display setup lock (0 = unlocked, 1 = locked) (*2) "},
    {0x0404, 1, 1, UNSIGNED, "", "Display temperature unit (0 = Celsius, 1 = Fahrenheit) "},


    //Remote control registers - firmware v1.29 or higher
    {0x2000, 1    , 1, UNSIGNED , "- ", "Charge algorithm version"},
    {0x2001, 0.01 , 2, UNSIGNED, "V ", "Charge voltage set-point"},
    {0x2002, 0.01 , 2, UNSIGNED, "V ", "Battery voltage sense"},
    {0x2003, 0.01 , 2, SIGNED, "°C ", "Battery temperature sense"},
    {0x2004, 1    , 1, UNSIGNED , "- ", "Remote command "},
    {0x2007, 1    , 4, UNSIGNED, "ms ", "Charge state elapsed time"},
    {0x2008, 0.01 , 2, UNSIGNED, "hours", "Absorption time"},
    {0x2009, 1    , 1, UNSIGNED , "- ", "Error code"},
    {0x200A, 0.001, 4, SIGNED, "A ", "Battery charge current"},
    {0x200B, 0.01 , 2, UNSIGNED, "V ", "Battery idle voltage"},
    {0x200C, 1    , 1, UNSIGNED , "- ", "Device state"},
    {0x200D, 1    , 1, UNSIGNED , "- ", "Network info"},
    {0x200E, 1    , 1, UNSIGNED , "- ", "Network mode"},
    {0x200F, 1    , 1, UNSIGNED , "- ", "Network status register"},
    {0x2013, 0.001, 4, SIGNED, "A ", "Total charge current"},
    {0x2014, 1    , 1, UNSIGNED , "% ", "Charge current percentage"},
    {0x2015, 0.1  , 2, UNSIGNED, "A ", "Charge current limit"},
    {0x2018, 1    , 1, UNSIGNED , "- ", "Manual equalisation pending"},
    {0x2027, 0.01 , 4, UNSIGNED, "W ", "Total DC input power"},

};


#endif /* VHP_REGISTERS_H_ */
