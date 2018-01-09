/*
	Name:		SmartHomeTest.ino
	Created:	22.10.2016
	Author:		Viktoria Jechsmayr

	Main-File for Communication with Arduino
*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SmartHome.h>
#include "UdpCommandInterface.h"
#include <CommandDefinitions.h>

#define SMARTHOME_ID 2
#define COMMAND_COUNT 6
#define COMMAND_DATA_MAXSIZE 10

#define BUTTON_PIN 7
#define LED1_PIN 8
#define LED2_PIN 9


#define TEMP_PIN A0
#define FOTO_PIN A1

//Mac-Adress (ethernet hardware) address for the shield
byte mac[] = {
	0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP udp;

// local port to listen on
unsigned int localPort = 8888;

//Button-Status by default LOW=OFF
int buttonStatus = LOW;

//tempAVG and temps[10] for calculation an average over the last 10 values
float tempAVG = 0;
int temps[10];

//counter for temps-array
int tempCount = 0;

byte commandData[COMMAND_COUNT][COMMAND_DATA_MAXSIZE];
Command commands[COMMAND_COUNT];

SubDevice subDevices[4];

UdpCommandInterface udpCommandInterface(&udp, localPort);
SmartHome smartHome(SMARTHOME_ID, &udpCommandInterface, commands, COMMAND_COUNT);

// the setup function runs once when you press reset or power the board
void setup()
{
	//set pinMode to define INPUT and OUTPUT of PINs
	pinMode(LED1_PIN, OUTPUT);
	pinMode(LED2_PIN, OUTPUT);
	pinMode(BUTTON_PIN, INPUT);

	//init commands-Array
	for (int i = 0; i < COMMAND_COUNT; i++)
	{
		commands[i].data = &commandData[i][0];
		commands[i].dataMaxLength = COMMAND_DATA_MAXSIZE;
	}

	smartHome.setCommandHandler(handleCommand);

	/*
		Set Actor and Sensor-IDs and Types
	*/
	//Temperature-Sensor
	subDevices[0].id = 1;
	subDevices[0].type = SUBDEVICE_TYPE_SENSOR;

	//LED1
	subDevices[1].id = 2;
	subDevices[1].type = SUBDEVICE_TYPE_ACTOR;

	//Foto-Sensor
	subDevices[2].id = 3;
	subDevices[2].type = SUBDEVICE_TYPE_SENSOR;

	//LED2
	subDevices[3].id = 4;
	subDevices[3].type = SUBDEVICE_TYPE_ACTOR;

	/*
		setSubDevices for the SmartHome
		When more Actor/Sensor are added the number of SubDevices had to be adjusted
	*/
	smartHome.setSubDevices(subDevices, 4);

	//default Communication-Rate (9600 bits per second)
	Serial.begin(9600);

	/*
	Initializes the ethernet library and network settings
	Ethernet.begin(mac) returns an int: 1 on a successful DHCP connection, 0 on failure
	*/
	if (Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP");
		// no point in carrying on, so do nothing forevermore:
		while (true);
	}

	udp.begin(localPort);
	printIPAddress();
}

// the loop function runs over and over again until power down or reset
void loop()
{
	//current Status of Button on Board (LOW or HIGH)
	int currentButtonStatus = digitalRead(BUTTON_PIN);

	//Call buttonPushed-Function if new Status isn't the set buttonStatus
	if (currentButtonStatus != buttonStatus)
	{
		buttonPushed(currentButtonStatus);
	}

	//calculate the average Temperature for better result
	calculateTempAVG();

	//Start smartHome-Logic
	smartHome.work();
}

//if the new Button-Status is HIGH(ON) then read the Status of the LED1 and inverse it
void buttonPushed(int currentButtonStatus)
{
	if (currentButtonStatus == HIGH)
	{
		int ledStatus = digitalRead(LED1_PIN);
		if (ledStatus == LOW)
		{
			digitalWrite(LED1_PIN, HIGH);
		}
		else
		{
			digitalWrite(LED1_PIN, LOW);
		}
	}
	buttonStatus = currentButtonStatus;
}

/*
	Calculate the average over 10 readed Temperature-Values for a better result
	Result is not correct because of inaccuracy of the sensor
	When more parts (Actor, Sensor) are connected the more inaccuracy it will be
	Better Result with external 9V-Battery
*/
void calculateTempAVG()
{
	if (tempCount < 10)
	{
		temps[tempCount] = map(analogRead(TEMP_PIN), 0, 410, -50, 150);
		tempCount++;
	}
	else
	{
		tempAVG = 0;
		for (int i = 0; i < 10; i++)
		{
			tempAVG = tempAVG + temps[i];
		}
		tempAVG = tempAVG / 10;
		tempCount = 0;
	}
}

/*
	handle received non-system Commands 
*/
bool handleCommand(Command* command)
{
	Serial.print("Non-System command received: ");
	Serial.println(command->commandId);

	if (command->commandId == COMMAND_GET_VALUE_REQUEST)
	{
		handleGETValueCommand(command);
	}

	if (command->commandId == COMMAND_SET_VALUE_REQUEST)
	{
		handleSETValueCommand(command);
	}
	return true;
}

/*
	
*/
void handleGETValueCommand(Command* command)
{
	if (command->dataLength >= 1)
	{
		//reserve free command for reply
		Command* replyCommand = smartHome.getFreeCommand();

		//NULL when all Commands used
		if (replyCommand != NULL)
		{
			//fill replyCommand [SenderID 1Byte | ReceiverID 1 Byte | CommandID 1 Byte]
			udpCommandInterface.fillCommand(replyCommand, SMARTHOME_ID, command->senderId, COMMAND_GET_VALUE_REPLY);
			
			//received subDeviceID 
			byte id = command->data[0];

			//getSensorValue from SensorIds 1 or 3
			if (id == 1 || id == 3)
			{
				udpCommandInterface.setCommandData(replyCommand, id, getSensorValue(id));
			}

			//getActorValue from ActorIds 2 or 4
			if (id == 2 || id == 4)
			{
				udpCommandInterface.setCommandData(replyCommand, id, getActorValue(id));
			}

			//send command
			udpCommandInterface.writeCommand(replyCommand);

			//free command for use
			smartHome.freeCommand(replyCommand);
		}
	}
}

/*

*/
void handleSETValueCommand(Command* command)
{
	if (command->dataLength >= 2)
	{
		//subDeviceId
		byte id = command->data[0];
		//value to set
		byte value = command->data[1];

		if (value == 0)
		{
			setActorValue(id, false);
		}
		else
		{
			setActorValue(id, true);
		}

		//reserve free command for reply
		Command* replyCommand = smartHome.getFreeCommand();

		//NULL when all Commands used
		if (replyCommand != NULL)
		{
			//fill replyCommand [SenderID 1Byte | ReceiverID 1 Byte | CommandID 1 Byte]
			udpCommandInterface.fillCommand(replyCommand, SMARTHOME_ID, command->senderId, COMMAND_SET_VALUE_REPLY);
			
			//set new Actor value
			udpCommandInterface.setCommandData(replyCommand, id, getActorValue(id));

			//send command
			udpCommandInterface.writeCommand(replyCommand);
			//free command for use
			smartHome.freeCommand(replyCommand);
		}
	}
}

//get Value from the Sensor by ID
float getSensorValue(byte id)
{
	if (id == 1)
	{
		return tempAVG - 4;
	}

	if (id == 3)
	{
		return analogRead(FOTO_PIN);
	}
	return 0;
}

//get Value from the Actor by ID
bool getActorValue(byte id)
{
	if (id == 2)
	{
		if (digitalRead(LED1_PIN) == HIGH)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (id == 4)
	{
		if (digitalRead(LED2_PIN) == HIGH)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

//set new Value for Actor by ID
void setActorValue(byte id, bool value)
{
	if (id == 2)
	{
		if (value)
		{
			digitalWrite(LED1_PIN, HIGH);
		}
		else
		{
			digitalWrite(LED1_PIN, LOW);
		}
	}

	if (id == 4)
	{
		if (value)
		{
			digitalWrite(LED2_PIN, HIGH);
		}
		else
		{
			digitalWrite(LED2_PIN, LOW);
		}
	}
}

//print IP Adress - initial-Function for network communication testing
void printIPAddress()
{
	Serial.print("My IP address: ");
	for (byte thisByte = 0; thisByte < 4; thisByte++) {
		// print the value of each byte of the IP address:
		Serial.print(Ethernet.localIP()[thisByte], DEC);
		Serial.print(".");
	}

	Serial.println();
}