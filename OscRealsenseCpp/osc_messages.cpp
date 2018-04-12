#include "osc_messages.h"
#include <ctime>



osc_messages::~osc_messages()
{
}



void osc_messages::sendHeartbeat(int tick) {

	char heartBeatAddress[80];
	strcpy_s(heartBeatAddress, "/cursor/");
	strcat_s(heartBeatAddress, camID);
	strcat_s(heartBeatAddress, "/heartbeat");
	packetStream->Clear();
	*packetStream << osc::BeginMessage(heartBeatAddress) << tick << osc::EndMessage;
	transmitSocket->Send(packetStream->Data(), packetStream->Size());
	std::cout << "h:" << camID << "," << tick << std::endl;

}
void osc_messages::checkAndSendHeartbeat() {
	clock_t t1 = clock();
	double dt = (clock() - t1) / CLOCKS_PER_SEC;
	if (dt > INTERVAL)
	{
		int tick = (t1 / CLOCKS_PER_SEC);
		sendHeartbeat(tick);
	}
}

void osc_messages::cursorOpen(int tick) {
	cursorWithSuffix(tick, "/open");
}

void osc_messages::cursorClose(int tick) {
	cursorWithSuffix(tick, "/close");
}

void osc_messages::cursorWithSuffix(int tick, char* suffix) {
	char heartBeatAddress[80];
	strcpy_s(heartBeatAddress, "/cursor/");
	strcat_s(heartBeatAddress, camID);
	strcat_s(heartBeatAddress, suffix);
	packetStream->Clear();
	*packetStream << osc::BeginMessage(heartBeatAddress) << tick << osc::EndMessage;
	transmitSocket->Send(packetStream->Data(), packetStream->Size());
}



void osc_messages::cursor(const char* handedness, float x, float y, float z) {

	char address[80];
	strcpy_s(address, "/cursor/");
	strcat_s(address, camID);
	strcat_s(address, "/");
	strcat_s(address, handedness);

	packetStream->Clear();
	*packetStream << osc::BeginMessage(address)
		<< x << y << z
		<< osc::EndMessage;
	transmitSocket->Send(packetStream->Data(), packetStream->Size());
	std::cout << "c:" << camID << "," << x << "," << y << "," << z << std::endl;

}

void osc_messages::bend(const char* handedness, int f1, int f2, int f3, int f4, int f5) {
	char address[80];
	strcpy_s(address, "/gesture/");
	strcat_s(address, handedness);
	strcat_s(address, "/fingers");

	packetStream->Clear();
	*packetStream << osc::BeginMessage(address)
		<< "firefly" << f1 << f2 << f3 << f4 << f5
		<< osc::EndMessage;
	transmitSocket->Send(packetStream->Data(), packetStream->Size());
	std::cout << "b:" << "," << f1 << "," << f2 << "," << f3 << "," << f4 << "," << f5 << std::endl;



}

void osc_messages::bend(const char* handedness, float f1, float f2, float f3, float f4, float f5) {
	bend(handedness, (int)f1, (int)f2, (int)f3, (int)f4, (int)f5);

}


// /midi/channel <int midi status> <int pitch> <int velocity>
void osc_messages::midi_note_send(int status, int channel, int pitch, int velocity) {
	char address[80];
	snprintf(address, sizeof address, "/midi/%d", channel);

	packetStream->Clear();
	*packetStream << osc::BeginMessage(address)
		<< status << pitch << velocity
		<< osc::EndMessage;
	transmitSocket->Send(packetStream->Data(), packetStream->Size());
	std::cout << "n:" << channel << "," << status << "," << pitch << "," << velocity << std::endl;

}


void osc_messages::midi_note_off(int channel, int pitch) {
	midi_note_send(128, channel, pitch, 0);
}


void osc_messages:: midi_note_on(int channel, int pitch, int velocity){
	midi_note_send(144, channel, pitch, velocity);
}

