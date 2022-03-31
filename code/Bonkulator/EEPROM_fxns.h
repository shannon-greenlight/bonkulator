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
EEPROM_Int selected_output = EEPROM_Int(0, 7); // the output we are working on
// check for new eeprom usage
bool eeprom_is_initialized()
{
  return selected_output.read_int(EEPROM_INIT_FLAG) == EEPROM_INIT_PATTERN && selected_output.read_int(EEPROM_OFFSET_FLAG) == Greenface_EEPROM::eeprom_offset;
}
