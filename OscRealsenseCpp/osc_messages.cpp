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