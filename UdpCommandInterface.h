/*
	Name:		UpdCommandInterface.h
	Created:	3.11.2016
	Author:		Viktoria Jechsmayr

	 Structure for UDP-Commands
*/

#ifndef UdpCommandInterface_h
#define UdpCommandInterface_h

#include "Arduino.h"
#include <EthernetUdp.h>
#include <CommandInterface.h>

class UdpCommandInterface : public CommandInterface
{
public:
	UdpCommandInterface(EthernetUDP* udp, uint16_t commandPort);

	bool readCommand(Command* command);
	void writeCommand(Command* command);

private:
	EthernetUDP* const _udp;
	uint16_t const _commandPort;
	byte packetBuffer[UDP_TX_PACKET_MAX_SIZE];
};

#endif