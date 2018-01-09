#ifndef CommandInterface_h
#define CommandInterface_h

#include "Arduino.h"

struct Command
{
	byte senderId;
	byte receiverId;
	byte commandId;
	uint8_t dataMaxLength;
	uint8_t dataLength;
	byte* data;
	bool inUse;
};

class CommandInterface
{
public:
	void fillCommand(Command* command, byte senderId, byte receiverId, byte commandId);
	void fillCommand(Command* command, byte senderId, byte receiverId, byte commandId, byte* data, int dataLength);
	void setCommandData(Command* command, byte id, float value);
	void setCommandData(Command* command, byte id, bool value);

	virtual bool readCommand(Command* command) = 0;
	virtual void writeCommand(Command* command) = 0;

protected:
	bool bufferToCommand(byte* buffer, int offset, int length, Command* command);
	int commandToBuffer(Command* command, byte* buffer);
};

#endif