/*
This is a library for Arduino programmes.
Written by HurryPeng with the license of CC-BY-NC-SA 4.0.
Version: 2017.03.27 A
*/

#pragma once

#include <Arduino.h>
#include "HurryPeng.h"
#include "MsTimer2.h"

class Digitron_3461BS
{
public:
	Digitron_3461BS(pin_t _sclk, pin_t _rclk, pin_t _dio);
	~Digitron_3461BS();

	void clear();
	void dispNum(uint16_t num, uint8_t font = DEC);
	void dispDigit(uint8_t digit, uint8_t num);

	static void update();

private:
	static const uint8_t list[18];
	static const uint8_t digitCode[4];
	static Digitron_3461BS *(reg[16]);
	static uint8_t currentDigit;
	static bool init;
	static void initialise();

	pin_t sclk;
	pin_t rclk;
	pin_t dio;
	uint8_t code[4] = { 0xff };

	void flash();
};
