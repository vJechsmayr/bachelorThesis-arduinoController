/*
	Name:		SmartHome.cpp
	Created:	12.11.2016
	Author:		Viktoria Jechsmayr

	SmartHome Logic
*/

#include "SmartHome.h"
#include "CommandDefinitions.h"

#define BROADCAST_ID 0x00

SmartHome::SmartHome(byte id, CommandInterface* commandInterface, Command* commands, uint8_t commandLength)
	: _id(id), _commandInterface(commandInterface), _commands(commands), _commandLength(commandLength)
{
}

void SmartHome::setCommandHandler(bool(*commandHandler)(Command*))
{
	_commandHandler = commandHandler;
}

void SmartHome::setSubDevices(SubDevice* subDevices, uint8_t length)
{
	_subDevices = subDevices;
	_subDevicesLength = length;
}

void SmartHome::work()
{
	Command* command = getFreeCommand();
	while (command != NULL)
	{
		if (!_commandInterface->readCommand(command))
		{
			freeCommand(command);
			break;
		}

		if (handleCommand(command))
		{
			freeCommand(command);
		}

		command = getFreeCommand();
	}
}

Command* SmartHome::getFreeCommand()
{
	for (uint8_t i = 0; i < _commandLength; i++)
	{
		if (!_commands[i].inUse)
		{
			_commands[i].inUse = true;
			return &_commands[i];
		}
	}
	return NULL;
}

void SmartHome::freeCommand(Command* command)
{
	if (command != NULL)
	{
		command->inUse = false;
		command->senderId = 0;
		command->receiverId = 0;
		command->commandId = 0;
		command->dataLength = 0;

		for (int i = 0; i < command->dataMaxLength; i++)
		{
			command->data[i] = 0;
		}
	}
}

bool SmartHome::handleCommand(Command* command)
{
	if (command->receiverId != _id && command->receiverId != BROADCAST_ID)
	{
		return true;
	}

	if (handleSystemCommand(command))
	{
		return true;
	}

	if (_commandHandler != NULL)
	{
		return _commandHandler(command);
	}
	return true;
}

bool SmartHome::handleSystemCommand(Command* command)
{
	if (command->commandId == COMMAND_DISCOVER_REQUEST)
	{
		Command* reply = getFreeCommand();
		if (reply != NULL)
		{
			_commandInterface->fillCommand(reply, _id, command->senderId, COMMAND_DISOCVER_REPLY);
			_commandInterface->writeCommand(reply);
			freeCommand(reply);
		}
		return true;
	}

	if (command->commandId == COMMAND_GET_SUBDEVICES_REQUEST)
	{
		Command* reply = getFreeCommand();
		if (reply != NULL)
		{
			byte data[2];

			for (int i = 0; i < _subDevicesLength; i++)
			{
				data[0] = _subDevices[i].id;
				data[1] = _subDevices[i].type;

				_commandInterface->fillCommand(reply, _id, command->senderId, COMMAND_GET_SUBDEVICES_REPLY, data, 2);
				_commandInterface->writeCommand(reply);
			}
			freeCommand(reply);
		}
		return true;
	}
	return false;
}