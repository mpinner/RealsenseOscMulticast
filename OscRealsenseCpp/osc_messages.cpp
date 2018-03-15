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
	packetStream.Clear();
	packetStream << osc::BeginMessage(heartBeatAddress) << tick << osc::EndMessage;
	transmitSocket.Send(packetStream.Data(), packetStream.Size());
	//std::cout << "h:" << camID << "," << (t1 / CLOCKS_PER_SEC) << endl;

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
