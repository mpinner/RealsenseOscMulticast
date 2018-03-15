#include <windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <ctime>
#include <cstring>

#include "pxcsensemanager.h"
#include "pxcmetadata.h"
#include "service/pxcsessionservice.h"

#include "pxchandmodule.h"
#include "pxchanddata.h"
#include "pxchandconfiguration.h"
#include "pxccursordata.h"
#include "pxccursorconfiguration.h"
#include "pxchandcursormodule.h"

#include "Definitions.h"
#include "timer.h"

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"
#include "osc_messages.h"

using namespace std;
const int OUTPUT_BUFFER_SIZE = 2048;
const int INTERVAL = 1; // In seconds


bool g_gestures = true;	// Writing gesture data to console output
bool g_alerts = false;	// Writing alerts data to console output
bool g_stop = false;	// user closes application
bool g_cursor = true;	// Enable Cursor tracking mode
bool g_skeleton = false; // Enable Full hand tracking mode
bool g_Info = true;	// Enable Cursor / Skeleton Information
bool g_timer = true;    // Enable FPS output
bool g_heartbeat = true;    // Enable heartbeat output

std::wstring g_sequencePath;

PXCSession *g_session;
PXCSenseManager *g_senseManager;
PXCHandModule *g_handModule;
PXCHandData *g_handDataOutput;
PXCHandConfiguration *g_handConfiguration;
PXCHandCursorModule *g_handCursorModule;
PXCCursorData *g_cursorDataOutput;
PXCCursorConfiguration *g_cursorConfiguration;

void releaseAll();

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:

		// confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		g_stop = true;
		Sleep(1000);
		releaseAll();
		return(TRUE);

	default:
		return FALSE;
	}
}

void main(int argc, const char* argv[])
{

	if ((argc != 1) && (argc != 4) && (argc != 5))
	{
		std::cout << "usage: UdpClient <camera ID> <IP> <Port> [-bends]" << std::endl;
		exit(1);
	}

	char IP[30];
	char camID[30];
	int Port;
	if (argc == 1)
	{
		strcpy_s(camID, 5, "s90");
		strcpy_s(IP, 14, "239.255.0.85");
		Port = 10085;
	}
	else 
	{
		size_t charsConverted = 0;
		strcpy(camID, argv[1]);
		strcpy(IP, argv[2]);
		Port = atoi(argv[3]);
	}
	if (argc == 5) {
		if (strcmp(argv[4], "-bends") == 0)
		{
			//++moduleCount;
			g_skeleton = true;
			g_cursor = false;
		}
	}

	std::cout << "Running cam:" << camID << " ip:" << IP << " port:" << Port << endl;

	UdpTransmitSocket transmitSocket(IpEndpointName(IP, Port));
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream packetStream(buffer, OUTPUT_BUFFER_SIZE);

	osc_messages osc(camID, &transmitSocket, &packetStream);
	osc.sendHeartbeat(0);


	// Measuring FPS
	FPSTimer timer;

	Definitions::appName = argv[0];

	// Setup
	g_session = PXCSession::CreateInstance();
	if (!g_session)
	{
		std::printf("Failed Creating PXCSession\n");
		return;
	}

	g_senseManager = g_session->CreateSenseManager();
	if (!g_senseManager)
	{
		releaseAll();
		std::printf("Failed Creating PXCSenseManager\n");
		return;
	}

	if (g_skeleton)
	{
		if (g_cursor)
		{
			std::printf("Cannot enable both cursor and skeleton\n");
			return;
		}

		if (g_senseManager->EnableHand(0) != PXC_STATUS_NO_ERROR)
		{
			releaseAll();
			std::printf("Failed Enabling Hand Module\n");
			return;
		}

		g_handModule = g_senseManager->QueryHand();
		if (!g_handModule)
		{
			releaseAll();
			std::printf("Failed Creating PXCHandModule\n");
			return;
		}



		g_handDataOutput = g_handModule->CreateOutput();
		if (!g_handDataOutput)
		{
			releaseAll();
			std::printf("Failed Creating PXCHandData\n");
			return;
		}

		g_handConfiguration = g_handModule->CreateActiveConfiguration();
		if (!g_handConfiguration)
		{
			releaseAll();
			std::printf("Failed Creating PXCHandConfiguration\n");
			return;
		}

		g_handConfiguration->SetTrackingMode(PXCHandData::TRACKING_MODE_FULL_HAND);

		if (g_gestures)
		{
			std::printf("-Gestures Are Enabled-\n");
			g_handConfiguration->EnableAllGestures();
		}

		if (g_alerts)
		{
			std::printf("-Alerts Are Enabled-\n");
			g_handConfiguration->EnableAllAlerts();
		}

		// Apply configuration setup
		g_handConfiguration->ApplyChanges();

		std::printf("-Skeleton Information Enabled-\n");
	}

	if (g_cursor)
	{


		if (g_senseManager->EnableHandCursor(0) != PXC_STATUS_NO_ERROR)
		{
			releaseAll();
			std::printf("Failed Enabling Hand Cursor Module\n");
			return;
		}

		g_handCursorModule = g_senseManager->QueryHandCursor();
		if (!g_handCursorModule)
		{
			releaseAll();
			std::printf("Failed Creating PXCHandCursorModule\n");
			return;
		}

		g_cursorDataOutput = g_handCursorModule->CreateOutput();
		if (!g_cursorDataOutput)
		{
			releaseAll();
			std::printf("Failed Creating PXCCursorData\n");
			return;
		}

		g_cursorConfiguration = g_handCursorModule->CreateActiveConfiguration();
		if (!g_cursorConfiguration)
		{
			releaseAll();
			std::printf("Failed Creating PXCCursorConfiguration\n");
			return;
		}

		if (g_gestures)
		{
			std::printf("-Gestures Are Enabled-\n");
			g_cursorConfiguration->EnableAllGestures();

		}
		if (g_alerts)
		{
			std::printf("-Alerts Are Enabled-\n");
			g_cursorConfiguration->EnableAllAlerts();
		}


		// Apply configuration setup
		g_cursorConfiguration->ApplyChanges();
		std::printf("-Cursor Information Enabled-\n");
	}


	pxcI32 numOfHands = 0;

	// First Initializing the sense manager
	if (g_senseManager->Init() == PXC_STATUS_NO_ERROR)
	{
		std::printf("\nPXCSenseManager Initializing OK\n========================\n");

		clock_t t1 = clock();

		// Acquiring frames from input device
		while (g_senseManager->AcquireFrame(true) == PXC_STATUS_NO_ERROR && !g_stop)
		{
			// HEARTBEATS
			double dt = (clock() - t1) / CLOCKS_PER_SEC;
			if (dt > INTERVAL)
			{	
				t1 = clock();
				int tick = (t1 / CLOCKS_PER_SEC);
				osc.sendHeartbeat(tick);
			}

			if (g_skeleton)
			{
				// Get current hand outputs
				if (g_handDataOutput->Update() == PXC_STATUS_NO_ERROR)
				{

					// Display alerts
					if (g_alerts)
					{
						PXCHandData::AlertData alertData;
						for (int i = 0; i < g_handDataOutput->QueryFiredAlertsNumber(); ++i)
						{
							if (g_handDataOutput->QueryFiredAlertData(i, alertData) == PXC_STATUS_NO_ERROR)
							{
								std::printf("%s was fired at frame %d \n", Definitions::AlertToString(alertData.label).c_str(), alertData.frameNumber);
							}
						}
					}

					// Display gestures
					if (g_gestures)
					{
						PXCHandData::GestureData gestureData;
						for (int i = 0; i < g_handDataOutput->QueryFiredGesturesNumber(); ++i)
						{
							if (g_handDataOutput->QueryFiredGestureData(i, gestureData) == PXC_STATUS_NO_ERROR)
							{
								std::wprintf(L"%s, Gesture: %s was fired at frame %d \n", Definitions::GestureStateToString(gestureData.state), gestureData.name, gestureData.frameNumber);
							}
						}
					}

					// Display joints
					if (g_Info)
					{
						PXCHandData::IHand *hand;
						PXCHandData::JointData jointData;
						for (int i = 0; i < g_handDataOutput->QueryNumberOfHands(); ++i)
						{
							g_handDataOutput->QueryHandData(PXCHandData::ACCESS_ORDER_BY_TIME, i, hand);
							std::string handSide = "Unknown Hand";
							handSide = hand->QueryBodySide() == PXCHandData::BODY_SIDE_LEFT ? "Left Hand" : "Right Hand";

							std::printf("%s\n==============\n", handSide.c_str());
							for (int j = 0; j < 22; ++j)
							{
								if (hand->QueryTrackedJoint((PXCHandData::JointType)j, jointData) == PXC_STATUS_NO_ERROR)
								{
									std::printf("     %s)\tX: %f, Y: %f, Z: %f \n", Definitions::JointToString((PXCHandData::JointType)j).c_str(), jointData.positionWorld.x, jointData.positionWorld.y, jointData.positionWorld.z);
								}
							}
						}
					}
				}
			}



			if (g_cursor)
			{
				// Get current hand outputs
				if (g_cursorDataOutput->Update() == PXC_STATUS_NO_ERROR)
				{
					// Display alerts
					if (g_alerts)
					{
						PXCCursorData::AlertData alertData;
						for (int i = 0; i < g_cursorDataOutput->QueryFiredAlertsNumber(); ++i)
						{
							if (g_cursorDataOutput->QueryFiredAlertData(i, alertData) == PXC_STATUS_NO_ERROR)
							{
								std::printf("%s was fired at frame %d \n", Definitions::CursorAlertToString(alertData.label).c_str(), alertData.frameNumber);
							}
						}
					}

					// Display gestures
					if (g_gestures)
					{
						PXCCursorData::GestureData gestureData;
						for (int i = 0; i < g_cursorDataOutput->QueryFiredGesturesNumber(); ++i)
						{
							if (g_cursorDataOutput->QueryFiredGestureData(i, gestureData) == PXC_STATUS_NO_ERROR)
							{
								const pxcCHAR* name = Definitions::GestureTypeToString(gestureData.label);
								//std::wprintf(L"Gesture: %s was fired at frame %d \n", Definitions::GestureTypeToString(gestureData.label), gestureData.frameNumber);

								if (name == L"CURSOR_HAND_CLOSING") {
									std::wprintf(L"Gesture: %s was fired at frame %d \n", Definitions::GestureTypeToString(gestureData.label), gestureData.frameNumber);

									t1 = clock();
									osc.cursorClose(int(t1 / CLOCKS_PER_SEC));
									
								}
								else if (name == L"CURSOR_HAND_OPENING") {
									std::wprintf(L"Gesture: %s was fired at frame %d \n", Definitions::GestureTypeToString(gestureData.label), gestureData.frameNumber);

									t1 = clock();
									osc.cursorOpen(int(t1 / CLOCKS_PER_SEC));
								} 
								
							}
						}
					}

					// Display cursor information
					if (g_Info)
					{
						//TODO
						PXCCursorData::ICursor *cursor;
						for (int i = 0; i < g_cursorDataOutput->QueryNumberOfCursors(); ++i)
						{
							g_cursorDataOutput->QueryCursorData(PXCCursorData::ACCESS_ORDER_BY_TIME, i, cursor);
							std::string handSide = "Unknown Hand";
							handSide = cursor->QueryBodySide() == PXCHandData::BODY_SIDE_LEFT ? "Left Hand Cursor" : "Right Hand Cursor";

							//	std::printf("%s\n==============\n", handSide.c_str());
							//	std::printf("Cursor Image Point: X: %f, Y: %f \n", cursor->QueryCursorImagePoint().x, cursor->QueryCursorImagePoint().y);
							//std::printf("Cursor %s %s: X: %f, Y: %f, Z: %f \n", camID, handSide.c_str(), cursor->QueryCursorWorldPoint().x, cursor->QueryCursorWorldPoint().y, cursor->QueryCursorWorldPoint().z);
							//	std::printf("Cursor %s Point: X: %f, Y: %f, Z: %f \n", cursor->QueryCursorWorldPoint().x, cursor->QueryCursorWorldPoint().y, cursor->QueryCursorWorldPoint().z);
								//	std::printf("Cursor Engagement status: %d%c \n\n", cursor->QueryEngagementPercent(), '%');

							std::string oschand = "unknown";
							oschand = cursor->QueryBodySide() == PXCHandData::BODY_SIDE_LEFT ? "left" : "right";


							const char* handedness = oschand.c_str();
							float x = cursor->QueryCursorWorldPoint().x;
							float y = cursor->QueryCursorWorldPoint().y;
							float z = cursor->QueryCursorWorldPoint().z;

							osc.cursor(handedness, x, y, z );

						}
					}

				}
			}

			if (g_timer)
			{
				timer.Tick();
			}

			g_senseManager->ReleaseFrame();
		} // end while acquire frame
	} // end if Init

	else
	{
		releaseAll();
		std::printf("Failed Initializing PXCSenseManager\n");
		return;
	}


	releaseAll();
}

void releaseAll()
{

	if (g_handConfiguration)
	{
		g_handConfiguration->Release();
		g_handConfiguration = NULL;
	}
	if (g_cursorConfiguration)
	{
		g_cursorConfiguration->Release();
		g_cursorConfiguration = NULL;
	}

	if (g_handDataOutput)
	{
		g_handDataOutput->Release();
		g_handDataOutput = NULL;
	}
	if (g_cursorDataOutput)
	{
		g_cursorDataOutput->Release();
		g_cursorDataOutput = NULL;
	}


	if (g_senseManager)
	{
		g_senseManager->Close();
		g_senseManager->Release();
		g_senseManager = NULL;
	}
	if (g_session)
	{
		g_session->Release();
		g_session = NULL;
	}
}


