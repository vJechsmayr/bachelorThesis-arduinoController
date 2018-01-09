#include "CommandInterface.h"

void CommandInterface::fillCommand(Command* command, byte senderId, byte receiverId, byte commandId)
{
	fillCommand(command, senderId, receiverId, commandId, NULL, 0);
}

void CommandInterface::fillCommand(Command* command, byte senderId, byte receiverId, byte commandId, byte* data, int dataLength)
{
	command->senderId = senderId;
	command->receiverId = receiverId;
	command->commandId = commandId;

	if (dataLength > command->dataMaxLength)
	{
		command->dataLength = command->dataMaxLength;
	}
	else
	{
		command->dataLength = dataLength;
	}

	if (command->dataLength > 0)
	{
		memcpy(command->data, data, command->dataLength * sizeof(byte));
	}
}

void CommandInterface::setCommandData(Command* command, byte id, float value)
{
	if (command->dataMaxLength >= sizeof(float) + 1)
	{
		command->data[0] = id;
		memcpy(&command->data[1], &value, sizeof(float));
		command->dataLength = sizeof(float) + 1;
	}
}


void CommandInterface::setCommandData(Command* command, byte id, bool value)
{
	if (command->dataMaxLength >= 2)
	{
		command->data[0] = id;
		command->data[1] = value ? 1 : 0;
		command->dataLength = 2;
	}
}

bool CommandInterface::bufferToCommand(byte* buffer, int offset, int length, Command* command)
{
	if (length < 3)
	{
		return false;
	}

	fillCommand(command, buffer[0], buffer[1], buffer[2], &buffer[3], length - 3);
	return true;
}

int CommandInterface::commandToBuffer(Command* command, byte* buffer)
{
	int bufferLength = command->dataLength + 3;

	buffer[0] = command->senderId;
	buffer[1] = command->receiverId;
	buffer[2] = command->commandId;

	if (command->dataLength > 0)
	{
		byte* dataBuffer = &buffer[3];
		memcpy(dataBuffer, command->data, command->dataLength * sizeof(byte));
	}

	return bufferLength;
}