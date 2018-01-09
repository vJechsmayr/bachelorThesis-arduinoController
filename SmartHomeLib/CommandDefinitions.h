/*
	Name:		CommandDefinitions.h
	Created:	22.10.2016
	Author:		Viktoria Jechsmayr

	Definition-File for all definitions of possible Commands for
	Communication with the Arduino
	Make sure Command-Numbers at Arduino and at SmartHome-Server are identical
*/

#ifndef CommandDefinitions_h
#define CommandDefinitions_h

#define COMMAND_DISCOVER_REQUEST 0x01
#define COMMAND_DISOCVER_REPLY 0x02
#define COMMAND_GET_SUBDEVICES_REQUEST 0x03
#define COMMAND_GET_SUBDEVICES_REPLY 0x04
#define COMMAND_GET_VALUE_REQUEST 0x05
#define COMMAND_GET_VALUE_REPLY 0x06
#define COMMAND_SET_VALUE_REQUEST 0x07
#define COMMAND_SET_VALUE_REPLY 0x08

#endif