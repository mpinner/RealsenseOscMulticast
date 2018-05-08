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

	//	/cursor/camID/handedness <float x> <float y> <float z>
	void cursor(const char* handedness, float x, float y, float z);

	//	/gesture/handedness/fingers <int thumb> <int index> <int middle> <int ring> <int pinky>
	void bend(const char* handedness, float f1, float f2, float f3, float f4, float f5);
	void bend(const char* handedness, int f1, int f2, int f3, int f4, int f5);

	// /midi/channel <int midi status> <int pitch> <int velocity>
	void midi_note_send(int status, int channel, int pitch, int velocity);
	void midi_note_on(int channel, int pitch, int velocity);
	void midi_note_off(int channel, int pitch);

	void cursorWithSuffix(int tick, char* suffix);





};

