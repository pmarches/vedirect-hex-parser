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
    {0x0100, 1, 4, UNSIGNED, "", "Product Id"   },
    {0x0104, 1, 8, UNSIGNED, "", "Group Id"     },
    {0x010A, 1, 4, CHAR    , "", "Serial number"},
    {0x010B, 1, 4, CHAR    , "", "Model name"   },
    {0x0140, 1, 4, UNSIGNED, "", "Capabilities" },

    {0x0200, 1, 1, UNSIGNED, "", "Device mode"},
    {0x0201, 1, 1, UNSIGNED, "", "Device state"},
    {0x0202, 1, 4, UNSIGNED, "", "Remote control used"},
    {0x0205, 1, 1, UNSIGNED, "", "Device off reason1"},
    {0x0207, 1, 4, UNSIGNED, "", "Device off reason2"},

    //Battery settings registers
    {0xEDFF, 1   , 1, UNSIGNED, "0=off, 1=on", "Batterysafe mode"},
    {0xEDFE, 1   , 1, UNSIGNED, "0=off, 1=on", "Adaptive mode"},
    {0xEDFD, 1   , 1, UNSIGNED, "0=off, 1..250", "Automatic equalisation mode"},
    {0xEDFC, 0.01, 2, UNSIGNED, "hours", "Battery bulk time limit"},
    {0xEDFB, 0.01, 2, UNSIGNED, "hours", "Battery absorption time limit"},
    {0xEDF7, 0.01, 2, UNSIGNED, "V", "Battery absorption voltage"},
    {0xEDF6, 0.01, 2, UNSIGNED, "V", "Battery float voltage"},
    {0xEDF4, 0.01, 2, UNSIGNED, "V", "Battery equalisation voltage"},
    {0xEDF2, 0.01, 2, SIGNED  , "mV/K", "Battery temp. compensation"},
    {0xEDF1, 1   , 1, UNSIGNED, "0xFF = user", "Battery type"},
    {0xEDF0, 0.10, 2, UNSIGNED, "A", "Battery maximum current"},
    {0xEDEF, 1   , 1, UNSIGNED, "V", "Battery voltage"},
    {0xEDEC, 0.01, 2, UNSIGNED, "K, 0xFFFF=N/A", "Battery temperature"},
    {0xEDEA, 1   , 1, UNSIGNED, "V", "Battery voltage setting"},
    {0xEDE8, 1   , 1, UNSIGNED, "0=no, 1=yes", "BMS present"},
    {0xEDE7, 0.10, 2, UNSIGNED, "", "Tail current"},
    {0xEDE6, 0.10, 2, UNSIGNED, "A, 0xFFFF=use max", "Low temperature charge current"},
    {0xEDE5, 1   , 1, UNSIGNED, "0=no, 1=yes", "Auto equalise stop on voltage"},
    {0xEDE4, 1   , 1, UNSIGNED, "% (of 0xEDF0)", "Equalisation current level"},
    {0xEDE3, 0.01, 2, UNSIGNED, "hours", "Equalisation duration"},
    {0xED2E, 0.01, 2, UNSIGNED, "V", "Re-bulk voltage offset"},
    {0xEDE0, 0.01, 2, SIGNED  , "°C", "Battery low temperature level"},
    {0xEDCA, 0.01, 2, UNSIGNED, "V", "Voltage compensation"},

    //Charger regsiters
    {0xEDEC, 0.01, 2, UNSIGNED, "K", "Battery temperature"},
    {0xEDDF, 0.1 , 2, UNSIGNED, "A", "Charger maximum current"},
    {0xEDDD, 0.01, 4, UNSIGNED, "kWh", "System yield"},
    {0xEDDC, 0.01, 4, UNSIGNED, "kWh", "User yield (resettable)"},
    {0xEDDB, 0.01, 2, SIGNED  , "°C", "Charger internal temperature"},
    {0xEDDA, 1   , 1, UNSIGNED, "-", "Charger error code"},
    {0xEDD7, 0.1 , 2, UNSIGNED, "A", "Charger current"},
    {0xEDD5, 0.01, 2, UNSIGNED, "V", "Charger voltage"},
    {0xEDD4, 1   , 1, UNSIGNED, "-", "Additional charger state info"},
    {0xEDD3, 0.01, 2, UNSIGNED, "kWh", "Yield today"},
    {0xEDD2, 1   , 2, UNSIGNED, "W", "Maximum power today"},
    {0xEDD1, 0.01, 2, UNSIGNED, "kWh", "Yield yesterday"},
    {0xEDD0, 1   , 2, UNSIGNED, "W", "Maximum power yesterday"},
    {0xEDCE, 1   , 2, UNSIGNED, "-", "Voltage settings range"},
    {0xEDCD, 1   , 1, UNSIGNED, "-", "History version"},
    {0xEDCC, 1   , 1, UNSIGNED, "-", "Streetlight version"},
    {0x2211, 0.01, 2, UNSIGNED, "V", "Adjustable voltage minimum"},
    {0x2212, 0.01, 2, UNSIGNED, "V", "Adjustable voltage maximum"},

    //Solar panel registers
    {0xEDBC, 0.01, 4, UNSIGNED, "W", "Panel power"},
    {0xEDBB, 0.01, 2, UNSIGNED, "V", "Panel voltage"},
    {0xEDBD, 0.1 , 2, UNSIGNED, "A", "Panel current"},
    {0xEDB8, 0.01, 2, UNSIGNED, "V", "Panel maximum voltage"},
    {0xEDB3, 1   , 1, UNSIGNED, "-", "Tracker mode"},

    //Load output registers
    {0xEDAD, 0.1 , 2, UNSIGNED, "A ", "Load current "},
    {0xEDAC, 0.01, 1, UNSIGNED, "V ", "Load offset voltage "},
    {0xEDAB, 1   , 1, UNSIGNED, "- ", "Load output control "},
    {0xEDA9, 0.01, 2, UNSIGNED, "V ", "Load output voltage "},
    {0xEDA8, 1   , 1, UNSIGNED, "- ", "Load output state "},
    {0xED9D, 0.01, 2, UNSIGNED, "V ", "Load switch high level "},
    {0xED9C, 0.01, 2, UNSIGNED, "V ", "Load switch low level "},
    {0xED91, 1   , 1, UNSIGNED, "- ", "Load output off reason"},
    {0xED90, 1   , 2, UNSIGNED, "minute", "Load AES timer "},

    //Relay settings registers
    {0xEDD9, 1   , 1, UNSIGNED, "- ", "Relay operation mode "},
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
    {0xEDA7, 1   , 2, SIGNED  , "min ", "Mid-point shift (*1) "},
    {0xED9B, 1   , 1, UNSIGNED, "s ", "Gradual dim speed (*2) "},
    {0xED9A, 0.01, 2, UNSIGNED, "V ", "Panel voltage night (*3)"},
    {0xED99, 0.01, 2, UNSIGNED, "V ", "Panel voltage day (*3) "},
    {0xED96, 1   , 2, UNSIGNED, "min ", "Sunset delay (*4) "},
    {0xED97, 1   , 2, UNSIGNED, "min ", "Sunrise delay (*4) "},
    {0xED90, 1   , 2, UNSIGNED, "min ", "AES Timer "},
    {0x2030, 1   , 1, UNSIGNED, "0=dark, 1=light ", "Solar activity "},
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
    {0x2000, 1    , 1, UNSIGNED, "- ", "Charge algorithm version"},
    {0x2001, 0.01 , 2, UNSIGNED, "V ", "Charge voltage set-point"},
    {0x2002, 0.01 , 2, UNSIGNED, "V ", "Battery voltage sense"},
    {0x2003, 0.01 , 2, SIGNED  , "°C ", "Battery temperature sense"},
    {0x2004, 1    , 1, UNSIGNED, "- ", "Remote command "},
    {0x2007, 1    , 4, UNSIGNED, "ms ", "Charge state elapsed time"},
    {0x2008, 0.01 , 2, UNSIGNED, "hours", "Absorption time"},
    {0x2009, 1    , 1, UNSIGNED, "- ", "Error code"},
    {0x200A, 0.001, 4, SIGNED  , "A ", "Battery charge current"},
    {0x200B, 0.01 , 2, UNSIGNED, "V ", "Battery idle voltage"},
    {0x200C, 1    , 1, UNSIGNED, "- ", "Device state"},
    {0x200D, 1    , 1, UNSIGNED, "- ", "Network info"},
    {0x200E, 1    , 1, UNSIGNED, "- ", "Network mode"},
    {0x200F, 1    , 1, UNSIGNED, "- ", "Network status register"},
    {0x2013, 0.001, 4, SIGNED  , "A ", "Total charge current"},
    {0x2014, 1    , 1, UNSIGNED, "% ", "Charge current percentage"},
    {0x2015, 0.1  , 2, UNSIGNED, "A ", "Charge current limit"},
    {0x2018, 1    , 1, UNSIGNED, "- ", "Manual equalisation pending"},
    {0x2027, 0.01 , 4, UNSIGNED, "W ", "Total DC input power"},

};

const RegisterDesc* lookupRegister(uint16_t registerId){
  //TODO use an index
  uint16_t nbRegisterDesc=sizeof(registerDescriptions)/sizeof(RegisterDesc);
  for(int i=0; i<nbRegisterDesc; i++){
    const RegisterDesc* it=&registerDescriptions[i];
    if(it->registerValue==registerId){
      return it;
    }
  }
  return NULL;
}

typedef struct {
  const uint16_t productId;
  const char* productName;
} ProductDescription;

const ProductDescription PRODUCT_DESC[]={
    {0x0300, "BlueSolar MPPT 70|15"},
    {0xA040, "BlueSolar MPPT 75|50"},
    {0xA041, "BlueSolar MPPT 150|35"},
    {0xA042, "BlueSolar MPPT 75|15"},
    {0xA043, "BlueSolar MPPT 100|15"},
    {0xA044, "BlueSolar MPPT 100|30"},
    {0xA045, "BlueSolar MPPT 100|50"},
    {0xA046, "BlueSolar MPPT 150|70"},
    {0xA047, "BlueSolar MPPT 150|100"},
    {0xA048, "BlueSolar MPPT 75|50 rev2"},
    {0xA049, "BlueSolar MPPT 100|50 rev2"},
    {0xA04A, "BlueSolar MPPT 100|30 rev2"},
    {0xA04B, "BlueSolar MPPT 150|35 rev2"},
    {0xA04C, "BlueSolar MPPT 75|10"},
    {0xA04D, "BlueSolar MPPT 150|45"},
    {0xA04E, "BlueSolar MPPT 150|60"},
    {0xA04F, "BlueSolar MPPT 150|85"},
    {0xA050, "SmartSolar MPPT 250|100"},
    {0xA051, "SmartSolar MPPT 150|100"},
    {0xA052, "SmartSolar MPPT 150|85"},
    {0xA053, "SmartSolar MPPT 75|15"},
    {0xA054, "SmartSolar MPPT 75|10"},
    {0xA055, "SmartSolar MPPT 100|15"},
    {0xA056, "SmartSolar MPPT 100|30"},
    {0xA057, "SmartSolar MPPT 100|50"},
    {0xA058, "SmartSolar MPPT 150|35"},
    {0xA059, "SmartSolar MPPT 150|100 rev2"},
    {0xA05A, "SmartSolar MPPT 150|85 rev2"},
    {0xA05B, "SmartSolar MPPT 250|70"},
    {0xA05C, "SmartSolar MPPT 250|85"},
    {0xA05D, "SmartSolar MPPT 250|60"},
    {0xA05E, "SmartSolar MPPT 250|45"},
    {0xA05F, "SmartSolar MPPT 100|20"},
    {0xA060, "SmartSolar MPPT 100|20 48V"},
    {0xA061, "SmartSolar MPPT 150|45"},
    {0xA062, "SmartSolar MPPT 150|60"},
    {0xA063, "SmartSolar MPPT 150|70"},
    {0xA064, "SmartSolar MPPT 250|85 rev2"},
    {0xA065, "SmartSolar MPPT 250|100 rev2"},
    {0xA102, "SmartSolar MPPT VE.Can 150|70"},
    {0xA103, "SmartSolar MPPT VE.Can 150|45"},
    {0xA104, "SmartSolar MPPT VE.Can 150|60"},
    {0xA105, "SmartSolar MPPT VE.Can 150|85"},
    {0xA106, "SmartSolar MPPT VE.Can 150|100"},
    {0xA107, "SmartSolar MPPT VE.Can 250|45"},
    {0xA108, "SmartSolar MPPT VE.Can 250|60"},
    {0xA109, "SmartSolar MPPT VE.Can 250|70"},
    {0xA10A, "SmartSolar MPPT VE.Can 250|85"},
    {0xA10B, "SmartSolar MPPT VE.Can 250|100"},
};

const ProductDescription* lookupProductId(uint16_t productId){
  uint16_t nbProductDesc=sizeof(PRODUCT_DESC)/sizeof(ProductDescription);
  for(int i=0; i<nbProductDesc; i++){
    const ProductDescription* it=&PRODUCT_DESC[i];
    if(it->productId==productId){
      return it;
    }
  }
  return NULL;
}

#endif /* VHP_REGISTERS_H_ */
