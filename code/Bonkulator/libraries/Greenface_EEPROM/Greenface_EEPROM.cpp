#include "Arduino.h"
#include <TerminalVT100.h>
#include <Adafruit_FRAM_I2C.h>

#include <Greenface_EEPROM.h>

Greenface_EEPROM::Greenface_EEPROM()
{
	fram = Adafruit_FRAM_I2C();
	framAddr = 0;
}

uint8_t Greenface_EEPROM::read(int framAddr)
{
	return fram.read8(framAddr);
}

void Greenface_EEPROM::write(int framAddr, uint8_t value)
{
	// Serial.println("Writing byte: "+String(value) + " to: " + String(framAddr));
	fram.write8(framAddr, value);
}

boolean Greenface_EEPROM::update(int framAddr, uint8_t val)
{
	uint8_t existing = read(framAddr);
	boolean different = existing != val;
	if (different)
	{
		write(framAddr, val);
	}
	return different;
}

int Greenface_EEPROM::read_int(int framAddr)
{
	return (int)(read(framAddr++) | read(framAddr) << 8);
}

void Greenface_EEPROM::write_int(int framAddr, int val)
{
	write(framAddr++, val & 0xff);
	write(framAddr, val >> 8);
}

boolean Greenface_EEPROM::update_int(int framAddr, int val)
{
	int existing = read_int(framAddr);
	boolean different = existing != val;
	if (different)
	{
		write(framAddr, val);
	}
	return different;
}

void Greenface_EEPROM::get(int framAddr, uint8_t *ptr, uint16_t num_bytes)
{
	for (uint16_t i = 0; i < num_bytes; i++)
	{
		*ptr++ = read(framAddr + i);
		// Serial.print(i);
	}
}

void Greenface_EEPROM::put(int framAddr, uint8_t *ptr, uint16_t num_bytes)
{
	for (uint16_t i = 0; i < num_bytes; i++)
	{
		write(framAddr + i, *ptr++);
		// Serial.print(i);
	}
}

uint16_t Greenface_EEPROM::begin(boolean inc_ctr)
{
	if (fram.begin())
	{ // you can stick the new i2c addr in here, e.g. begin(0x51);
		if (inc_ctr)
			Serial.println("\nFound I2C FRAM");
	}
	else
	{
		Serial.println("I2C FRAM not identified ... check your connections?\r\n");
		Serial.println("Will continue in case this processor doesn't support repeated start\r\n");
	}
	// Read the first byte
	// uint8_t test = fram.read8(0x0);
	int test = read_int(0x0);
	if (inc_ctr)
	{
		Serial.print("Restarted ");
		Serial.print(test);
		Serial.println(" times");
	}
	// Test write ++
	// fram.write8(0x0, test + 1);
	if (inc_ctr)
		write_int(0x0, test + 1);
	return 0;
}

void Greenface_EEPROM::dump(int start = 0, int len = 64)
{
	// dump the memory
	uint8_t value;
	for (uint16_t a = start; a < start + len; a++)
	{
		value = fram.read8(a);
		if ((a % 32) == 0)
		{
			Serial.print("\n\r 0x");
			Serial.print(a, HEX);
			Serial.print(": ");
		}
		Serial.print("0x");
		if (value < 0x1)
			Serial.print('0');
		Serial.print(value, HEX);
		Serial.print(" ");
	}
}

void Greenface_EEPROM::test()
{
	Serial.println("Offset: " + String(offset) + " Size: " + String(size));
	Serial.println("EEProm Offset: " + String(eeprom_offset));
	dump(0, 0x400);
	Serial.println("");
	return;

	//   Serial.println("Fxn max: "+String(fxn.max));
	//   Serial.println("Params: "+String(params.get(0)));
	//   Serial.println("Params offset: "+String(params.offset));
	//   Serial.println("Repeat val: "+String(repeat_on.get()));
	// test eeprom routines
	// eeprom.write_int(128,255);
	// int val=-1;
	// uint8_t byte = eeprom.read(0x01);
	// Serial.print("Byte1 = ");
	// Serial.println(byte++);
	// boolean u = eeprom.update(0x06,byte);
	// Serial.println(u ? "Updated" : "Not changed");
	// eeprom.write(0x02,0x55);
	// eeprom.put(2, (uint8_t*)&val, sizeof(val));
	// String b = "zBCDEF";
	// //Serial.print("\rb: "+b);
	// //Serial.println(sizeof(b));
	// eeprom.put(64, (uint8_t*)&b[0], b.length());

	// int arr[10] = {10,2,3,4,5,6,7,8,9,10};
	// eeprom.put(10,(uint8_t*) &arr[0], sizeof(arr));
	// eeprom.get(10,(uint8_t*) arr, sizeof(arr));
	// Serial.print("arr: ");
	// Serial.println(sizeof(arr));
	// Serial.print("[6]: ");
	// for(int i=0;i<sizeof(arr)/sizeof(arr[0]);i++) {
	// 	Serial.println(arr[i]);
	// }
	// val = eeprom.read_int(0);
	// Serial.print("Val: ");
	// Serial.println(val);
	// params.dump(0,256);
}
void Greenface_EEPROM::init()
{
	// write_int(EEPROM_OFFSET_FLAG,eeprom_offset);
}
