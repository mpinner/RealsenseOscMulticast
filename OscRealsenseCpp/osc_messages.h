#pragma once

#include "timer.h"

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"


class osc_messages
{
public:
	const int INTERVAL = 1; // In seconds

	char *camID;
	UdpTransmitSocket transmitSocket;
	osc::OutboundPacketStream packetStream;


	osc_messages(char *c, UdpTransmitSocket t, osc::OutboundPacketStream p) : camID(c), transmitSocket(t), packetStream(p) {};
	~osc_messages();



	//void init(char *camID, UdpTransmitSocket transmitSocket, osc::OutboundPacketStream packetStream);

	void sendHeartbeat(int tick);
	void checkAndSendHeartbeat();



};

