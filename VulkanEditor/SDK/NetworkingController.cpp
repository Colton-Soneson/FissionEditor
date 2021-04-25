#include "NetworkingController.h"

void NetworkManager::init()
{

}

void NetworkManager::initServer(unsigned short port, unsigned short maxConnections)
{
	mpServOps = new ServerOptions(port, maxConnections);
	mpServerPeer = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd(mpServOps->SERVER_PORT, 0);
	mpServerPeer->Startup(mpServOps->MAX_CLIENTS, &sd, 1);

	printf("Starting the server.\n");
	// We need to let the server accept incoming connections from the clients
	mpServerPeer->SetMaximumIncomingConnections(mpServOps->MAX_CLIENTS);

	mServerActive = true;
}

void NetworkManager::updateServer()
{
	if (mServerActive)
	{
		serverHandleInputRemote();
	}
}

void NetworkManager::closeServer()
{
	mpServerPeer->ClearBanList();

	RakNet::RakPeerInterface::DestroyInstance(mpServerPeer);

	mServerActive = false;
}

void NetworkManager::initClient(char nameType[512], unsigned short serverPort, char* serverIP)
{
	//char str[512];
	//const unsigned short SERVER_PORT = 4024;
	//const char SERVER_IP[] = "172.16.2.186";

	mGS->SERVER_PORT = serverPort;
	mGS->SERVER_IP = serverIP;

	mGS->clientPeer = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd;

	printf("Initializing client.\n");
	mGS->clientPeer->Startup(1, &sd, 1);
	mGS->clientPeer->SetMaximumIncomingConnections(0);


	printf("Connecting the client.\n");
	mGS->clientPeer->Connect(mGS->SERVER_IP, mGS->SERVER_PORT, 0, 0);
	printf("Starting the client.\n");

	strcpy(mGS->clientName, nameType);

	GameMessageGeneric msg = {
				(unsigned char)ID_TIMESTAMP,
				RakNet::GetTime(),
				(unsigned char)ID_GAME_REGISTER_NAME,		// This time, we only use this for log ons
				"Registering...",
				RakNet::UNASSIGNED_SYSTEM_ADDRESS,
				"UNUSED"
	};

	strcpy(msg.ownerName, mGS->clientName);

	mGS->clientPeer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet

}

void NetworkManager::updateClient()
{
	if (mClietActive)
	{
		// recieve data and merge
		clientHandleInputRemote();
	}

	strcpy(mGS->logOn, "");	//wipe the log on guess
}

void NetworkManager::closeClient()
{
	RakNet::RakPeerInterface::DestroyInstance(mGS->clientPeer);
}

void NetworkManager::sendClientMessage(char mesKB[512])
{
	//make sure we registered as a client first
	if (mClietActive)
	{
		clientIL_GenericMessage(mesKB);
	}
}

void NetworkManager::sendServerMessage(char mesKB[512])
{
	//make sure we have a server to send the message on
	if (mServerActive)
	{
		serverIL_GenericMessage(mesKB);
	}
}


//------Handling BOTH CLIENT AND SERVER Functionality------
unsigned char NetworkManager::getPacketIdentifier(RakNet::Packet* p)
{
	/*
	if (msg == ID_TIMESTAMP)
	{
		// handle the time stamp
		// 1) bitstream
		// 2) skip msg byte
		// 3) read time
		// 4) read new msg byte: what the actual ID to handle
	}
	*/
	unsigned char temp = (unsigned char)p->data[0];
	//printf("%s\n",temp);

	if (temp == ID_TIMESTAMP)
	{
		//printf("SERVER PACKET IDENTIFIER %i \n", (int)(sizeof(unsigned char) + sizeof(RakNet::Time)));
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(RakNet::Time)];
	}
	else
	{
		return (unsigned char)p->data[0];
	}
}

//--------------Handling SERVER Functionality--------------
void NetworkManager::serverIL_GenericMessage(char mesKB[512])
{

	/*
	* USE THIS STRUCTURE FOR SPECIFIC CASE KEYWORDS, LIKE "PING" OR "CHANGE NAME"
	if (strcmp(state->msgGD, "KEYWORD HERE") == 0)
	{
		//do thing for this keyword
	}
	*/

	GameMessageGeneric msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_MESSAGE_GENERIC,
			"UNUSED",
			RakNet::UNASSIGNED_SYSTEM_ADDRESS,
			"SERVER: "
	};

	char result[512];
	result[0] = 0;
	strncpy(result, msg.ownerName, sizeof(result));
	strncat(result, mesKB, (sizeof(result) - strlen(msg.ownerName) - 1));

	strcpy(msg.msg, result);

	//send function in raknet follows UDP Rules
	mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections

	
}

void NetworkManager::serverPacketHandlerGameMessageGeneric(RakNet::Packet* p)
{
	GameMessageGeneric* s = (GameMessageGeneric*)p->data;
	assert(p->length == sizeof(GameMessageGeneric));
	if (p->length != sizeof(GameMessageGeneric))
	{
		return;
	}

	printf("SERVER RECIEVED: %s\n", s->msg);

	char firstLetter;
	firstLetter = s->msg[0];

	if (firstLetter == '-')
	{
		if (std::find(mpServOps->adminList.begin(), mpServOps->adminList.end(), s->ownerName) != mpServOps->adminList.end())	//make sure owner is on the list
		{
			if (strcmp(s->msg, "-help") == 0)
			{
				char cList[512] = "\nLIST OF ADMIN FUNCTIONS: \n -bh [PLAYERNAME]    to ban player \n -pi [PLAYERNAME]    to ping player";


				GameMessageGeneric msg = {
						(unsigned char)ID_TIMESTAMP,
						RakNet::GetTime(),
						(unsigned char)ID_GAME_MESSAGE_GENERIC,
						"UNUSED",
						RakNet::UNASSIGNED_SYSTEM_ADDRESS,
						"SERVER: "
				};

				strcpy(msg.msg, cList);

				mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
					HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
					p->systemAddress, false);			//internet		
			}

			//commands will always be a "-" followed by two letters
			// "-bh playerName" ban hammers for playerName
			// so always get the first 3 letters
			// when searching for name here, make sure to account for the space

			char com[3];
			char bh_input[3] = { '-','b','h' };	//ban hammer
			char kp_input[3] = { '-','k','p' };	//kick player
			char pi_input[3] = { '-','p','i' };	//ping ip
			strncpy(com, s->msg, 3);


			if (com[0] == bh_input[0] && com[1] == bh_input[1] && com[2] == bh_input[2])
			{
				std::string x = s->msg;
				x = x.substr(x.find(' ') + 1);
				int i = 0;

				std::string resultOut;

				for (i; i < mpServOps->playerList.size(); ++i)
				{
					std::string plPlayer = mpServOps->playerList.at(i).first;
					if (x.compare(plPlayer) == 0)
					{
						mpServerPeer->AddToBanList(mpServOps->playerList.at(i).second.first.ToString());

						resultOut = "SERVER: banned player ";
						resultOut += x;
						resultOut += " at ";
						resultOut += mpServOps->playerList.at(i).second.first.ToString();
						resultOut += "\n";
						printf(resultOut.c_str());
						break;
					}
				}

				if (i >= mpServOps->playerList.size())
				{
					resultOut = "SERVER: player does not exist or is already banned";
					printf("SERVER: player does not exist or is already banned");
				}

				GameMessageGeneric msg = {
						(unsigned char)ID_TIMESTAMP,
						RakNet::GetTime(),
						(unsigned char)ID_GAME_MESSAGE_GENERIC,
						"UNUSED",
						RakNet::UNASSIGNED_SYSTEM_ADDRESS,
						"SERVER: "
				};

				strcpy(msg.msg, resultOut.c_str());

				mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
					HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
					p->systemAddress, false);			//internet	
			}

			if (com[0] == pi_input[0] && com[1] == pi_input[1] && com[2] == pi_input[2])
			{
				std::string x = s->msg;
				x = x.substr(x.find(' ') + 1);
				int i = 0;

				std::string resultOut;

				for (i; i < mpServOps->playerList.size(); ++i)
				{
					std::string plPlayer = mpServOps->playerList.at(i).first;
					if (x.compare(plPlayer) == 0)
					{
						mpServerPeer->Ping(mpServOps->playerList.at(i).second.first);

						//sp->AddToBanList();
						resultOut = "SERVER: pinged player ";
						resultOut += x;
						resultOut += " at ";
						resultOut += mpServOps->playerList.at(i).second.first.ToString();
						resultOut += "\n";
						printf(resultOut.c_str());
						break;
					}
				}

				if (i >= mpServOps->playerList.size())
				{
					resultOut = "SERVER: player not pingable";
					printf("SERVER: player not pingable");
				}

				GameMessageGeneric msg = {
						(unsigned char)ID_TIMESTAMP,
						RakNet::GetTime(),
						(unsigned char)ID_GAME_MESSAGE_GENERIC,
						"UNUSED",
						RakNet::UNASSIGNED_SYSTEM_ADDRESS,
						"SERVER: "
				};

				strcpy(msg.msg, resultOut.c_str());

				mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
					HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
					p->systemAddress, false);			//internet	
			}

			if (com[0] == kp_input[0] && com[1] == kp_input[1] && com[2] == kp_input[2])
			{
				std::string x = s->msg;
				x = x.substr(x.find(' ') + 1);
				int i = 0;

				std::string resultOut;

				for (i; i < mpServOps->playerList.size(); ++i)
				{
					std::string plPlayer = mpServOps->playerList.at(i).first;
					if (x.compare(plPlayer) == 0)
					{
						mpServerPeer->CloseConnection(mpServOps->playerList.at(i).second.first, true, 0);

						resultOut = "SERVER: kicked player  ";
						resultOut += x;
						resultOut += " at ";
						resultOut += mpServOps->playerList.at(i).second.first.ToString();
						resultOut += "\n";
						printf(resultOut.c_str());
						break;
					}
				}

				if (i >= mpServOps->playerList.size())
				{
					resultOut = "SERVER: player not available to kick";
					printf("SERVER: player not available to kick");
				}

				GameMessageGeneric msg = {
						(unsigned char)ID_TIMESTAMP,
						RakNet::GetTime(),
						(unsigned char)ID_GAME_MESSAGE_GENERIC,
						"UNUSED",
						RakNet::UNASSIGNED_SYSTEM_ADDRESS,
						"SERVER: "
				};

				strcpy(msg.msg, resultOut.c_str());

				mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
					HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
					p->systemAddress, false);			//internet	
			}
		}
	}
	else
	{
		//send game message one to everyone BUT who originally sent it
	//send function in raknet follows UDP Rules
		mpServerPeer->Send((char*)&(*s), sizeof(*s),			//application
			HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
			p->systemAddress, true);			//internet		SEND TO ALL BUT THIS ADDRESS (true is checked)
	}

}

void NetworkManager::serverHandleInputRemote()
{
	RakNet::Packet* packet;

	while (packet = mpServerPeer->Receive())
	{
		switch (getPacketIdentifier(packet))
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("Our connection request has been accepted.\n");
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("A client has disconnected.\n");
		}
		break;
		case ID_CONNECTION_LOST:
		{
			printf("A client lost the connection.\n");
		}
		break;

		case ID_GAME_MESSAGE_GENERIC:
		{
			serverPacketHandlerGameMessageGeneric(packet);	//recieve the game message one
		}
		break;
		case ID_GAME_LOG_ON:
		{
			serverPacketHandlerGameLogOn(packet);
		}
		break;
		case ID_GAME_REGISTER_NAME:
		{
			serverPacketHandlerIncomingPlayer(packet);
		}
		break;
		default:
			printf("SERVER: Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}


		mpServerPeer->DeallocatePacket(packet);
	}
}

void NetworkManager::serverPacketHandlerIncomingPlayer(RakNet::Packet* p)
{
	GameMessageGeneric* s = (GameMessageGeneric*)p->data;
	assert(p->length == sizeof(GameMessageGeneric));
	if (p->length != sizeof(GameMessageGeneric))
	{
		return;
	}

	std::pair<RakNet::SystemAddress, int> IPandPort(p->systemAddress.ToString(), mpServOps->SERVER_PORT);
	std::pair<std::string, std::pair<RakNet::SystemAddress, int>> result(s->ownerName, IPandPort);

	printf("SERVER: adding to player list");
	mpServOps->playerList.push_back(result);
}

void NetworkManager::serverPacketHandlerGameLogOn(RakNet::Packet* p)
{
	GameMessageGeneric* s = (GameMessageGeneric*)p->data;
	assert(p->length == sizeof(GameMessageGeneric));
	if (p->length != sizeof(GameMessageGeneric))
	{
		return;
	}

	printf("SERVER RECIEVED ADMIN LOG IN ATTEMPT\n");

	if (strcmp(s->msg, mpServOps->adminPass) == 0)	//if our password guess matches, add them to the admin list
	{
		//add the message owner to admin list
		mpServOps->adminList.push_back(s->ownerName);
		printf("SUCCESSFUL ADMIN LOG ON.    Current List:\n");

		for (int i = 0; i < mpServOps->adminList.size(); ++i)
		{
			printf(mpServOps->adminList.at(i).c_str());
			printf("\n");
		}
	}
	else
	{
		printf("FAILED ADMIN LOG ON\n");
	}
}

//--------------Handling CLIENT Functionality--------------
void NetworkManager::clientIL_AdminPassEnter()
{
	printf("\n\ntry and enter admin pass: ");
	std::cin >> mGS->logOn;

	GameMessageGeneric msg = {
				(unsigned char)ID_TIMESTAMP,
				RakNet::GetTime(),
				(unsigned char)ID_GAME_LOG_ON,		// This time, we only use this for log ons
				"UNUSED",
				RakNet::UNASSIGNED_SYSTEM_ADDRESS,
				"UNUSED B"
	};


	strcpy(msg.msg, mGS->logOn);
	strcpy(msg.ownerName, mGS->clientName);

	RakNet::RakPeerInterface* peer = mGS->clientPeer;

	//send function in raknet follows UDP Rules
	peer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet
}

void NetworkManager::clientPacketHandlerGameMessageGeneric(RakNet::Packet* p)
{
	GameMessageGeneric* s = (GameMessageGeneric*)p->data;
	assert(p->length == sizeof(GameMessageGeneric));
	if (p->length != sizeof(GameMessageGeneric))
	{
		return;
	}

	//if you make it to here, perform functionality for this packet type
	// PRINT THE MESSAGE 
	//printf("%s\n", p->data);
	printf("%s\n", s->msg);
}

void NetworkManager::clientIL_GenericMessage(char mesKB[512])
{
	RakNet::RakPeerInterface* peer = mGS->clientPeer;
	//RakNet::Packet* packet;
	//RakNet::MessageID msg = packet->data[0];

	/*
	* USE THIS STRUCTURE FOR SPECIFIC CASE KEYWORDS, LIKE "PING" OR "CHANGE NAME"
	if (strcmp(state->msgGD, "KEYWORD HERE") == 0)
	{
		//do thing for this keyword
	}
	*/

	char firstLetter = mesKB[0];

	if (strcmp(mesKB, "admin_mode") == 0)
	{
		clientIL_AdminPassEnter();
		strcpy(mesKB, "");
	}

	//consol command mode
	if (firstLetter == '-')
	{
		GameMessageGeneric msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_MESSAGE_GENERIC,
			"UNUSED",
			RakNet::UNASSIGNED_SYSTEM_ADDRESS,
			"UNUSED"
		};

		strcpy(msg.ownerName, mGS->clientName);
		strcpy(msg.msg, mesKB);

		//send function in raknet follows UDP Rules
		peer->Send((char*)&msg, sizeof(msg),			//application
			HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
			msg.sysAddr, true);			//internet
			//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
			//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections
	}

	//normal game message
	if (firstLetter != '-' && strcmp(mesKB, "admin_mode") != 0 && strcmp(mesKB, "\t") != 0 && strcmp(mesKB, " ") != 0 && strcmp(mesKB, "") != 0)
	{

		GameMessageGeneric msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_MESSAGE_GENERIC,
			"UNUSED",
			RakNet::UNASSIGNED_SYSTEM_ADDRESS,
			"UNUSED"
		};

		strcpy(msg.ownerName, mGS->clientName);
		std::string tempName = mGS->clientName;
		tempName += ": ";
		char result[512];
		result[0] = 0;
		strncpy(result, tempName.c_str(), sizeof(result));
		strncat(result, mesKB, (sizeof(result) - strlen(tempName.c_str()) - 1));

		strcpy(msg.msg, result);

		//send function in raknet follows UDP Rules
		peer->Send((char*)&msg, sizeof(msg),			//application
			HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
			msg.sysAddr, true);			//internet
			//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
			//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections
	}

	
}

void NetworkManager::clientHandleInputRemote()
{
	//recieve packets, merge with above

	RakNet::RakPeerInterface* peer = mGS->clientPeer;
	RakNet::Packet* packet;

	//for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())	//represents networking loop

	while (packet = peer->Receive())
	{
		//RakNet::MessageID msg = packet->data[0];

		switch (getPacketIdentifier(packet))
		{
		case ID_CONNECTION_ATTEMPT_FAILED:
			printf("connection attempt failed. CHECK THE IP AND PORT.\n");
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("Our connection request has been accepted.\n");
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("We have been disconnected.\n");
		}
		break;
		case ID_CONNECTION_LOST:
		{
			printf("Connection lost.\n");
		}
		break;

		case ID_GAME_MESSAGE_GENERIC:
		{
			//printf("GAME MES 1");
			clientPacketHandlerGameMessageGeneric(packet);
		}
		break;

		default:
			printf("CLIENT: Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}

		//done with the packet
		peer->DeallocatePacket(packet);
	}

}