/*
  Greenface_AD5328 - DAC library
  Copyright (c) 2022 Shannon Vance.  All right reserved.
  Released under Creative Commons Attribution-ShareAlike 2.5 Generic License
*/

// ensure this library description is only included once
#ifndef Greenface_AD5328_h
#define Greenface_AD5328_h

#define AD5328_LDAC_PASSTHRU 0x00
#define AD5328_LDAC_BLOCKED 0x01
#define AD5328_LDAC_LATCHED 0x02

#define AD5328_VDD_NONE 0x00
#define AD5328_VDD_ABCD 0x01
#define AD5328_VDD_EFGH 0x02
#define AD5328_VDD_BOTH 0x03

#define AD5328_BUFFERED_NONE 0x00
#define AD5328_BUFFERED_ABCD 0x01
#define AD5328_BUFFERED_EFGH 0x02
#define AD5328_BUFFERED_BOTH 0x03

#define AD5328_GAIN_NONE 0x00
#define AD5328_GAIN_ABCD 0x01
#define AD5328_GAIN_EFGH 0x02
#define AD5328_GAIN_BOTH 0x03

// include types & constants of Wiring core API

// library interface description
class Greenface_AD5328
{
	// user-accessible "public" interface
public:
	Greenface_AD5328(int _sync_pin, int _din_pin, int _sck_pin, int _ldac_pin);

	// int ldac_mode;  // 0 = pass thru, 1 = blocked, 2 = latched
	// below params use 2 bits. Bit 0: ABCD dacs, Bit 1: EFGH dacs
	// int vdd;  // 1 = use vdd as ref. overrides buf and gain
	// int buf;  // 1 = buffered reference
	// int gain; // 0 = gain of 1, 1 = gain of 2
	void begin(int ldac_mode, int vdd, int buf, int gain);
	void dac_out(unsigned int dac_num, unsigned int val);
	void xfer(); // use ldac to set output registers
	void data_reset();
	void factory_reset();
	void power_down();

	// library-accessible "private" interface
private:
	int sync_pin;
	int din_pin;
	int sck_pin;
	int ldac_pin;
	void send_to_device(uint16_t val); // raw data
};

#endif
