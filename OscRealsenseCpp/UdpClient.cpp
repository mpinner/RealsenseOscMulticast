#include "stdafx.h"

#include <stdio.h>
#include <winsock2.h>
#include "pxcsensemanager.h"
#include "pxccapture.h"
#include "pxcvideomodule.h"
#include "pxchandmodule.h"
#include "pxchandconfiguration.h"
#include "pxchanddata.h"
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <ctime>
using namespace std;

const int OUTPUT_BUFFER_SIZE = 2048;
const int INTERVAL = 1; // In seconds

#pragma comment(lib,"ws2_32.lib") //Winsock Library
PXCSession* session = 0;
PXCSenseManager* sm = 0;
PXCHandData* handData = 0;

pxcStatus InitModule()
{
	pxcStatus status = pxcStatus::PXC_STATUS_NO_ERROR;
	session = PXCSession::CreateInstance();
	if (session == NULL)
	{
		return pxcStatus::PXC_STATUS_INIT_FAILED;
	}
	sm = session->CreateSenseManager();
	if (!sm)
	{
		return pxcStatus::PXC_STATUS_INIT_FAILED;
	}
	/* Set Module */
	status = sm->EnableHand(0);
	if (status < pxcStatus::PXC_STATUS_NO_ERROR)
	{
		return status;
	}

	PXCHandModule* handModule = sm->QueryHand();
	if (handModule == NULL || status < pxcStatus::PXC_STATUS_NO_ERROR)
	{
		return status;
	}

	status = sm->Init();

	PXCHandConfiguration* config = handModule->CreateActiveConfiguration();
	if (config == NULL)
	{
		return pxcStatus::PXC_STATUS_INIT_FAILED;
	}
	status = config->SetTrackingMode(PXCHandData::TrackingModeType::TRACKING_MODE_CURSOR);
	status = config->EnableGesture(L"cursor_click");
	status = config->ApplyChanges();

	pxcBool isE = config->IsTrackingModeEnabled(PXCHandData::TRACKING_MODE_CURSOR);

	config->Release();

	handData = handModule->CreateOutput();
	if (handData == NULL)
	{
		return pxcStatus::PXC_STATUS_INIT_FAILED;
	}


	return status;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if ((argc != 1) && (argc != 4))
	{
		std::cout << "usage: UdpClient <camera ID> <IP> <Port>" << std::endl;
		exit(1);
	}

	char IP[30];
	char camID[30];
	int Port;
	if (argc == 1)
	{
		strcpy_s(camID, 2, "0");
		strcpy_s(IP, 10, "127.0.0.1");
		Port = 8888;
	}
	else
	{
		size_t charsConverted = 0;
		wcstombs_s(&charsConverted, camID, sizeof(camID), argv[1], 12);
		wcstombs_s(&charsConverted, IP, sizeof(IP), argv[2], 20);
		Port = _ttoi(argv[3]);
	}

	UdpTransmitSocket transmitSocket(IpEndpointName(IP, Port));
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);

	pxcStatus status = InitModule();
	if (status < pxcStatus::PXC_STATUS_NO_ERROR)
	{
		return 0;
	}


	clock_t t1 = clock();
	while (sm->AcquireFrame(true) >= pxcStatus::PXC_STATUS_NO_ERROR)
	{



		double dt = (clock() - t1) / CLOCKS_PER_SEC;
		if (dt > INTERVAL)
		{
			t1 = clock();
			char heartBeatAddress[80];
			strcpy_s(heartBeatAddress, "/cursor/");
			strcat_s(heartBeatAddress, camID);
			strcat_s(heartBeatAddress, "/heartbeat");
			p.Clear();
			p << osc::BeginMessage(heartBeatAddress) << (t1 / CLOCKS_PER_SEC) << osc::EndMessage;
			transmitSocket.Send(p.Data(), p.Size());
			std::cout << "h:" << camID << "," << (t1 / CLOCKS_PER_SEC) << endl;

		}
		pxcStatus status = handData->Update();
		if (status >= pxcStatus::PXC_STATUS_NO_ERROR)
		{
			pxcI32 numOfHands = handData->QueryNumberOfHands();
			for (int i = 0; i < numOfHands; ++i)
			{
				PXCHandData::IHand* hand;
				handData->QueryHandData(PXCHandData::AccessOrderType::ACCESS_ORDER_BY_TIME, i, hand);
				if (hand->HasCursor())
				{

					PXCHandData::BodySideType bodySide = hand->QueryBodySide();
					PXCHandData::ICursor* cursor;
					if (hand->QueryCursor(cursor) == pxcStatus::PXC_STATUS_NO_ERROR)
					{
						PXCPoint3DF32 pointWorld = cursor->QueryPointWorld();
						char address[80];
						strcpy_s(address, "/cursor/");
						strcat_s(address, camID);
						switch (bodySide)
						{
						case PXCHandData::BODY_SIDE_UNKNOWN:
							strcat_s(address, "/unknown");
							break;
						case PXCHandData::BODY_SIDE_LEFT:
							strcat_s(address, "/left");
							break;
						case PXCHandData::BODY_SIDE_RIGHT:
							strcat_s(address, "/right");
							break;
						default:
							break;
						}
						p.Clear();

						p << osc::BeginMessage(address)
							<< (pointWorld.x) << (pointWorld.y) << (pointWorld.z)
							<< osc::EndMessage;
						std::cout << "p:" << camID << "," << (pointWorld.x) << "," << (pointWorld.y) << "," << (pointWorld.z) << endl;
						transmitSocket.Send(p.Data(), p.Size());
					}
				}
			}
		}

		sm->ReleaseFrame();
	}

	handData->Release();
	sm->Close();
	sm->Release();
	sm = 0;
	session->Release();
	session = 0;
	return 0;
}

