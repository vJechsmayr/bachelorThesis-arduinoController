/*
	Name:		UdpCommandInterface.cpp
	Created:	3.11.2016
	Author:		Viktoria Jechsmayr

	Logic for UdpCommand Communication via UDP-Ethernet
*/

#include "UdpCommandInterface.h"

//Constructor
UdpCommandInterface::UdpCommandInterface(EthernetUDP* udp, uint16_t commandPort) : _udp(udp), _commandPort(commandPort){}

/*
	when UDP-Package in Buffer check Size
	copy received data to packetBuffer and create command
*/
bool UdpCommandInterface::readCommand(Command* command)
{
	int packetSize = _udp->parsePacket();
	if (packetSize > 0)
	{
		int bufferSize = _udp->read((char*)packetBuffer, UDP_TX_PACKET_MAX_SIZE);
		return bufferToCommand(packetBuffer, 0, bufferSize, command);
	}
	return false;
}

/*
	copy command-data to Buffer and send to Broadcast
*/
void UdpCommandInterface::writeCommand(Command* command)
{
	int bufferSize = commandToBuffer(command, packetBuffer);

	if (bufferSize > 0)
	{
		_udp->beginPacket("255.255.255.255", _commandPort);
		_udp->write(packetBuffer, bufferSize);
		_udp->endPacket();
	}
}