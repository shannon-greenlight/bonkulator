#include "Greenface_EEPROM.h"
#include "EEPROM_Arr16.h"
#include "EEPROM_Int.h"
#include "EEPROM_Bool.h"
#include "EEPROM_String.h"

#define EEPROM_DATA_START 16
#define EEPROM_INIT_FLAG 14
#define EEPROM_OFFSET_FLAG 12
#define EEPROM_INIT_PATTERN 0x55aa

int Greenface_EEPROM::eeprom_offset = EEPROM_DATA_START;
EEPROM_Int ee_info = EEPROM_Int(0, 0x7fff); // needed for eeprom_is_initialized
// check for new eeprom usage
bool eeprom_is_initialized()
{

  // Serial.print("EEPROM_INIT_FLAG: " + String(ee_info.read_int(EEPROM_INIT_FLAG)));
  // Serial.println(" EEPROM_OFFSET: " + String(ee_info.read_int(EEPROM_OFFSET_FLAG)) + " offset: " + String(Greenface_EEPROM::eeprom_offset));
  return ee_info.read_int(EEPROM_INIT_FLAG) == EEPROM_INIT_PATTERN && ee_info.read_int(EEPROM_OFFSET_FLAG) == Greenface_EEPROM::eeprom_offset;
}
