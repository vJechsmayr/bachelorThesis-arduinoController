/*
	Name:		SmartHome.h
	Created:	22.10.2016
	Author:		Viktoria Jechsmayr

	SmartHome Structure
*/

#ifndef SmartHome_h
#define SmartHome_h

#include "Arduino.h"
#include "CommandInterface.h"

//Defice SubDevice Types
#define SUBDEVICE_TYPE_ACTOR 1
#define SUBDEVICE_TYPE_SENSOR 2

struct SubDevice {
	byte id;
	byte type;

};

class SmartHome
{
public:
	SmartHome(byte id, CommandInterface* commandInterface, Command* commands, uint8_t commandLength);

	void setCommandHandler(bool(*commandHandler)(Command*));
	void setSubDevices(SubDevice* subDevices, uint8_t length);
	void work();

	Command* getFreeCommand();
	void freeCommand(Command* command);

private:
	byte const _id;
	CommandInterface* const _commandInterface;
	Command* const _commands;
	uint8_t const _commandLength;
	SubDevice* _subDevices;
	uint8_t _subDevicesLength;

	bool(*_commandHandler)(Command*);

	bool handleCommand(Command* command);
	bool handleSystemCommand(Command* command);

};

#endif