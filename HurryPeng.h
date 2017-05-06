/*
This is a library for Arduino programmes.
Written by HurryPeng with the license of CC-BY-NC-SA 4.0.
Version: 2017.03.23 A
*/

#pragma once

#include <Arduino.h>

typedef uint8_t pin_t;

inline void pM(pin_t p, uint8_t mode)
//Short for pinMode(). 
{
	pinMode(p, mode);
}

inline bool dR(pin_t input)
//Short for digitalRead(). 
{
	return digitalRead(input);
}

inline void dW(pin_t output, bool value)
//Short for digitalWrite(). 
{
	digitalWrite(output, value);
}

inline uint8_t aR(pin_t input)
//Short for analogRead(). 
{
	return analogRead(input);
}

inline void aW(pin_t output, uint8_t value)
//Short for analogWrite(). 
{
	analogWrite(output, value);
}

inline void delayMs(uint16_t microsec)
//Short for delayMicroseconds(). 
{
	delayMicroseconds(microsec);
}

inline int approximate(float x)
//To approximate a float to its closest interger. 
{
	return x - (uint16_t)x >= 0.5 ? (uint16_t)x + 1 : (uint16_t)x;
}

inline int appr(float x)
//Short for approximate(). 
{
	return approximate(x);
}
