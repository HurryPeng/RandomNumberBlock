/*
This is a first-prize-winner project of ShenZhen Experimental Shchool Maker Fair 2017. 
The previous name of the project is "RandomNumberBox", which also refers to an old configuration design. 
The 3D-printed block is used in two ways, one of which is that you can gain a random number by pushing the button. 
The other way enables you to get a large number of random numbers by using the serial console. 
By connecting the box to a PC, generation configurations can also be chenged via the serial console. 
The random numbers are generated based on the data returned by a sound sensor, which makes the numbers more random. 
Coding by HaoRan "Hurry" Peng, generation algorithm by ZheWei Zhang.
All contents published under the license of CC-BY-NC-SA 4.0.
*/

#include "HurryPeng.h"
#include "Digitron_3461BS.h"
#include <EEPROM.h>

const pin_t Sound = A0, Button = 2;
typedef uint8_t EEPROMAdd_t;
EEPROMAdd_t MinAddr = 0, MaxAddr = 2, StepAddr = 4, LGAddr = 6;
Digitron_3461BS Digitron(6, 5, 4);
uint16_t Min, Max, Step, LastGenerated, MapAmount, MapGroup, MapMax;
uint32_t Time[2] = { 0,0 };

void setup()
{
	Serial.begin(9600);

	pM(Button, INPUT_PULLUP);
	attachInterrupt(0, onButtonPress, FALLING);

	Digitron.clear();
	showDisability();

	delay(100);
	Min = EEPROM.get(MinAddr, Min);
	Max = EEPROM.get(MaxAddr, Max);
	Step = EEPROM.get(StepAddr, Step);
	LastGenerated = EEPROM.get(LGAddr, LastGenerated);

	setupMap();

	showZero();
}

void loop()
{
	clearSerial();
	while (!Serial.available()) delay(5000);
	char command[5] = "    ";
	for (uint8_t i = 0; Serial.available() && i <= 3; i++) command[i] = Serial.read();
	if (!strcmp(command, "cmd\n")) console();
}

void showDisability()
{
	for (uint8_t i = 0; i <= 3; i++) Digitron.dispDigit(i, 17);
}

void showZero()
{
	for (uint8_t i = 0; i <= 3; i++) Digitron.dispDigit(i, 0);
}

inline void setupMap()
{
	uint16_t mapAmount = (Max - Min) / Step + 1;
	MapGroup = 10000 / mapAmount;
	MapMax = MapGroup*mapAmount;
}

inline void clearSerial()
{
	while (Serial.available()) Serial.read();
}

inline void cutSerial()
{
	while (Serial.peek() == ' ') Serial.read();
}

inline void generate()
{
	uint8_t a = 0;
	uint16_t sum = 0, l = LastGenerated;
	sum += a = aR(Sound) % 10;
	sum += 10 * ((l % 10 + a + millis()) % 10);
	sum += 100 * ((((l /= 10) % 10) + a + millis()) % 10);
	sum += 1000 * ((((l /= 10) % 10) + a + millis()) % 10);
	LastGenerated = sum;
}

void onButtonPress()
{
	Time[1] = millis();
	if (Time[1] - Time[0] >= 300)
	{
		Time[0] = millis();
		generate();
		EEPROM.put(LGAddr, LastGenerated);
		Digitron.dispNum(mapping(), DEC);
	}
}

uint16_t pow10(uint8_t pow)
{
	uint16_t result;
	switch (pow)
	{
	case 0: result = 1; break;
	case 1: result = 10; break;
	case 2: result = 100; break;
	case 3: result = 1000; break;
	}
	return result;
}

inline int16_t mapping()
{
	return (LastGenerated / MapGroup)*Step + Min;
}

inline void console()
{
	detachInterrupt(0);
	Digitron.clear();
	showDisability();
	Serial.println("");
	Serial.println(R"(Console initialised. )");
	Serial.flush();
	bool loop = true;
	while (loop)
	{
		clearSerial();
		Serial.println("");
		Serial.print("Console>");
		while (!Serial.available()) delay(500);
		char command[10] = "         ";
		uint8_t i = 0;
		while (Serial.available() && Serial.peek() != ' ' && i <= 8) command[i++] = Serial.read();
		while (i <= 8) command[i++] = ' ';
		cutSerial();
		Serial.print(command);
		if (!strcmp(command, "help\n    ")) consoleHelp();
		else if (!strcmp(command, "config\n  ")) consoleConfig();
		else if (!strcmp(command, "setup    ")) consoleSetup();
		else if (!strcmp(command, "generate ")) consoleGenerate();
		else if (!strcmp(command, "exit\n    ")) consoleExit(), loop = false;
		else
		{
			Serial.println("");
			Serial.println(R"(Unknown command. Enter "help" for further information. )");
			Serial.flush();
		}
	}
	Digitron.clear();
	attachInterrupt(0, onButtonPress, FALLING);
	Serial.println(R"(Console closed. Use "cmd" to recall. )");
	Serial.flush();
	showZero();
}

inline void consoleHelp()
{
	Serial.println("");
	Serial.println(R"(The following commands are avaliable in the console:)");
	Serial.println(R"(--help)");
	Serial.println(R"(----Show help. )");
	Serial.println(R"(--config)");
	Serial.println(R"(----Show current configurations. )");
	Serial.println(R"(--setup [min] [max] [step])");
	Serial.println(R"(----Setup the configurations with following parameters:)");
	Serial.println(R"(------min:the minimum value of generated numbers)");
	Serial.println(R"(------max:the maximum value of generated numbers)");
	Serial.println(R"(------step:the minimum difference between two generated numbers)");
	Serial.println(R"(------Please be aware that all four parameters must be positive integers. )");
	Serial.println(R"(------The parameters will NOT be saved into EEPROM until command "exit" is used. )");
	Serial.println(R"(--------This feature means to minimise the use of EEPROM and lengthen its usage. )");
	Serial.println(R"(--generate [amount])");
	Serial.println(R"(----Generate a series of numbers. )");
	Serial.println(R"(--exit)");
	Serial.println(R"(----Exit the console. )");
	Serial.println(R"(------This command will also save current configurations into EEPROM. )");
	Serial.println(R"(------Serious issues may occur if disconnected without this command. )");
	Serial.flush();
}

inline void consoleConfig()
{
	Serial.println("");
	Serial.println(R"(Current configurations:)");
	Serial.print(R"(--min: )");
	Serial.println(Min);
	Serial.print(R"(--max: )");
	Serial.println(Max);
	Serial.print(R"(--step: )");
	Serial.println(Step);
	Serial.flush();
}

inline void consoleSetup()
{
	uint16_t min = 10000, max = 10000, step = 0;
	bool argumentErr = false;
	if (Serial.available())
	{
		min = Serial.parseInt();
		Serial.print(' ');
		Serial.print(min);
	}
	else argumentErr = true;
	cutSerial();
	if (Serial.available())
	{
		max = Serial.parseInt();
		Serial.print(' ');
		Serial.print(max);
	}
	else argumentErr = true;
	cutSerial();
	if (Serial.available())
	{
		step = Serial.parseInt();
		Serial.print(' ');
		Serial.print(step);
	}
	else argumentErr = true;
	Serial.println("");
	bool minMaxErr = min >= max ? true : false,
		overflowErr = min < 0 || max>9999 ? true : false,
		stepErr = step <= 0 ? true : false,
		divisionErr = (max - min) % step ? true : false;
	if (argumentErr || minMaxErr || overflowErr || stepErr || divisionErr)
	{
		Serial.println(R"(Invalid parameters: )");
		if (argumentErr) Serial.println(R"(--Not enough parameters. )");
		else
		{
			if (minMaxErr) Serial.println(R"(--Min and max reversed. )");
			if (overflowErr) Serial.println(R"(--Min and/or max out of range. )");
			if (stepErr) Serial.println(R"(--Invalid step. )");
			if (divisionErr) Serial.println(R"(--(Max-min) can't be divided by step. )");
		}
		Serial.println(R"(Setup failed. )");
		Serial.flush();
	}
	else
	{
		Min = min, Max = max, Step = step;
		setupMap();
		Serial.println(R"(Setup successful. )");
		Serial.flush();
		consoleConfig();
	}
}

inline void consoleGenerate()
{
	uint16_t amount = 0;
	bool argumentErr = false, amountErr = false;
	if (Serial.available())
	{
		amount = Serial.parseInt();
		Serial.print(' ');
		Serial.println(amount);
	}
	else argumentErr = true;
	if (!amount) amountErr = true;
	if (argumentErr || amountErr)
	{
		Serial.println(R"(Invalid parameters: )");
		if (argumentErr) Serial.println(R"(--Not enough parameters. )");
		else Serial.println(R"(--Invalid amount. )");
		Serial.println(R"(Generation failed. )");
		Serial.flush();
	}
	else
	{
		for (uint16_t i = 1; i <= amount; i++)
		{
			generate();
			Serial.println(mapping());
		}
		Serial.print(R"(Successfully generated )");
		Serial.print(amount);
		Serial.println(R"( random numbers. )");
		Serial.flush();
	}
}

inline void consoleExit()
{
	EEPROM.put(MinAddr, Min);
	delay(100);
	EEPROM.put(MaxAddr, Max);
	delay(100);
	EEPROM.put(StepAddr, Step);
	delay(100);
	EEPROM.put(LGAddr, LastGenerated);
	Serial.println("");
	Serial.println(R"(Configurations saved. )");
	Serial.flush();
}
