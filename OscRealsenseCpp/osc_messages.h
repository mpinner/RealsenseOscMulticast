#pragma once

#include <stdio.h>
#include <iostream>


#include "timer.h"

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"


class osc_messages
{
public:
	const int INTERVAL = 1; // In seconds

	char *camID;
	UdpTransmitSocket *transmitSocket;
	osc::OutboundPacketStream *packetStream;


	osc_messages(char *c, UdpTransmitSocket *t, osc::OutboundPacketStream *p) : camID(c), transmitSocket(t), packetStream(p) {};
	~osc_messages();



	//void init(char *camID, UdpTransmitSocket transmitSocket, osc::OutboundPacketStream packetStream);

	void sendHeartbeat(int tick);
	void checkAndSendHeartbeat();

	void cursorOpen(int tick);
	void cursorClose(int tick);

	void cursor(const char* handedness, float x, float y, float z);

	void bend(const char* handedness, float f1, float f2, float f3, float f4, float f5);
	void bend(const char* handedness, int f1, int f2, int f3, int f4, int f5);



	void cursorWithSuffix(int tick, char* suffix);





};

