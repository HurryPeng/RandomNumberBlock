/*
This is a library for Arduino programmes.
Written by HurryPeng with the license of CC-BY-NC-SA 4.0.
Version: 2017.03.27 A
*/

#include "Digitron_3461BS.h"

const uint8_t Digitron_3461BS::list[18] = { 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e,0xff,0xbf };
const uint8_t Digitron_3461BS::digitCode[4] = { 0x08,0x04,0x02,0x01 };
Digitron_3461BS *(Digitron_3461BS::reg[16]) = { NULL };
uint8_t Digitron_3461BS::currentDigit = 0;
bool Digitron_3461BS::init = false;

Digitron_3461BS::Digitron_3461BS(pin_t _sclk, pin_t _rclk, pin_t _dio)
{
	pM(sclk = _sclk, OUTPUT);
	pM(rclk = _rclk, OUTPUT);
	pM(dio = _dio, OUTPUT);
	uint8_t i = 0;
	while (reg[i] != NULL) i++;
	reg[i] = this;
	clear();
	if (!init)
	{
		initialise();
		init = true;
	}
}

Digitron_3461BS::~Digitron_3461BS()
{
}

void Digitron_3461BS::clear()
{
	for (uint8_t i = 0; i <= 3; i++) dispDigit(i, 16);
}

void Digitron_3461BS::dispNum(uint16_t num, uint8_t font = DEC)
{
	clear();
	bool flag = false;
	uint8_t current = 0;
	for (uint16_t i = 0, j =appr(pow(font, 3)); i <= 3; i++, j /= font)
	{
		current = 0;
		current = num / j;
		if (current != 0 || flag == true || i == 3)
		{
			dispDigit(i, current);
			flag = true;
		}
		else code[i] = list[16];
		num %= j;
	}
}

void Digitron_3461BS::dispDigit(uint8_t digit, uint8_t num)
{
	code[digit] = list[num];
}

void Digitron_3461BS::update()
{
	for (uint8_t i = 0; reg[i] != NULL; i++) reg[i]->flash();
	++currentDigit %= 4;
}

void Digitron_3461BS::initialise()
{
	MsTimer2::set(1, Digitron_3461BS::update);
	MsTimer2::start();
}

void Digitron_3461BS::flash()
{
	digitalWrite(rclk, HIGH);
	shiftOut(dio, sclk, MSBFIRST, code[currentDigit]);
	shiftOut(dio, sclk, MSBFIRST, digitCode[currentDigit]);
	digitalWrite(rclk, LOW);
}