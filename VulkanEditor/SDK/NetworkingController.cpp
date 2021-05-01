#include "NetworkingController.h"

void NetworkManager::init()
{

}

void NetworkManager::initServer(unsigned short port, unsigned short maxConnections, char serverPass[512])
{
	mpServOps = new ServerOptions(port, maxConnections);
	strcpy(mpServOps->adminPass, serverPass);
	mpServerPeer = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd(mpServOps->SERVER_PORT, 0);
	mpServerPeer->Startup(mpServOps->MAX_CLIENTS, &sd, 1);

	printf("Starting the server.\n");
	mpServerHistory->push_back("Starting the server.");
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
	if (mServerActive)
	{
		mpServerPeer->ClearBanList();

		RakNet::RakPeerInterface::DestroyInstance(mpServerPeer);

		mServerActive = false;
	}
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
	mpClientChatHistory->push_back("Initializing Local client");
	mGS->clientPeer->Startup(1, &sd, 1);
	mGS->clientPeer->SetMaximumIncomingConnections(0);


	printf("Connecting the client.\n");
	mpClientChatHistory->push_back("Connecting the local client.");
	mGS->clientPeer->Connect(mGS->SERVER_IP, mGS->SERVER_PORT, 0, 0);
	printf("Starting the client.\n");
	mpClientChatHistory->push_back("Starting the local client.");

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

	mClientActive = true;
}

void NetworkManager::updateClient()
{
	if (mClientActive)
	{
		// recieve data and merge
		clientHandleInputRemote();
	}

	strcpy(mGS->logOn, "");	//wipe the log on guess
}

void NetworkManager::closeClient()
{
	if (mClientActive)
	{
		RakNet::RakPeerInterface::DestroyInstance(mGS->clientPeer);
		mClientActive = false;
	}
}

void NetworkManager::sendClientMessage(char mesKB[512])
{
	//make sure we registered as a client first
	if (mClientActive)
	{
		clientIL_GenericMessage(mesKB);
	}
}

void NetworkManager::clientObjectAddSend(int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{
	if (mClientActive)
	{
		clientIL_AddObject(objectIndex, pos, scale, rot, ambMod, activatelighting);
	}
}

void NetworkManager::clientObjectAddSendToNewClient(RakNet::SystemAddress newClientAddress, int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{
	if (mClientActive)
	{
		clientIL_AddObjectToNewClient(newClientAddress, objectIndex, pos, scale, rot, ambMod, activatelighting);
	}
}

void NetworkManager::clientObjectEditSend(int selectionInHierarchy, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{
	if (mClientActive)
	{
		clientIL_EditObject(selectionInHierarchy, pos, scale, rot, ambMod, activatelighting);
	}
}

void NetworkManager::clientObjectDeleteSend(int selectionInHierarchy)
{
	if (mClientActive)
	{
		clientIL_DeleteObject(selectionInHierarchy);
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

void NetworkManager::serverObjectAddSendToNewClient(RakNet::SystemAddress newClientAddress, int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{
	if (mServerActive)
	{
		serverIL_AddObjectToNewClient(newClientAddress, objectIndex, pos, scale, rot, ambMod, activatelighting);
	}
}

void NetworkManager::sendClientAdminRequest(char mesKB[512])
{
	if (mClientActive)
	{
		clientIL_AdminPassEnter(mesKB);
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

	mpServerHistory->push_back(msg.msg);

	//send function in raknet follows UDP Rules
	mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections

	
}

void NetworkManager::serverIL_AddObject(int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{

	GameObjectChange msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_OBJECT_ADD,
			objectIndex,
			pos.x,			//use these down to just floats rather than glm because passing that data would be a nightmare
			pos.y,
			pos.z,
			scale.x,
			scale.y,
			scale.z,
			rot.x,
			rot.y,
			rot.z,
			ambMod,
			activatelighting,
			RakNet::UNASSIGNED_SYSTEM_ADDRESS,
			"[SERVER] Object Added"
	};

	mpServerHistory->push_back(msg.ownerStatement);

	//send function in raknet follows UDP Rules
	mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections

}

void NetworkManager::serverIL_AddObjectToNewClient(RakNet::SystemAddress newClientAddress, int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{

	GameObjectChange msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_OBJECT_ADD_NEW_CLIENT,
			objectIndex,
			pos.x,			//use these down to just floats rather than glm because passing that data would be a nightmare
			pos.y,
			pos.z,
			scale.x,
			scale.y,
			scale.z,
			rot.x,
			rot.y,
			rot.z,
			ambMod,
			activatelighting,
			newClientAddress,
			"[SERVER] Object Added For New Client"
	};

	mpServerHistory->push_back(msg.ownerStatement);

	//send function in raknet follows UDP Rules
	mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, false);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections
}

void NetworkManager::serverIL_EditObject(int selectionInHierarchy, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{

	GameObjectChange msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_OBJECT_EDIT,
			selectionInHierarchy,
			pos.x,			//use these down to just floats rather than glm because passing that data would be a nightmare
			pos.y,
			pos.z,
			scale.x,
			scale.y,
			scale.z,
			rot.x,
			rot.y,
			rot.z,
			ambMod,
			activatelighting,
			RakNet::UNASSIGNED_SYSTEM_ADDRESS,
			"[SERVER] Object Editted"
	};

	mpServerHistory->push_back(msg.ownerStatement);

	//send function in raknet follows UDP Rules
	mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections

}

void NetworkManager::serverIL_DeleteObject(int selectionInHierarchy)
{

	GameObjectDelete msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_OBJECT_REMOVE,
			selectionInHierarchy,
			RakNet::UNASSIGNED_SYSTEM_ADDRESS,
			"[SERVER] Object Removed"
	};

	mpServerHistory->push_back(msg.ownerStatement);

	//send function in raknet follows UDP Rules
	mpServerPeer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections

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
			mpServerHistory->push_back("Another client has disconnected");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			mpServerHistory->push_back("Another client has lost the connection.");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
		{
			printf("Another client has connected.\n");
			mpServerHistory->push_back("Another client has connected.");

			//int startOfTempAddr = sizeof(unsigned char) + sizeof(RakNet::Time) + sizeof(unsigned char)
			//	+ sizeof(int)
			//	+ sizeof(float) + sizeof(float) + sizeof(float)
			//	+ sizeof(float) + sizeof(float) + sizeof(float)
			//	+ sizeof(float) + sizeof(float) + sizeof(float)
			//	+ sizeof(float)
			//	+ sizeof(bool);
			//int endOfTempAddr = startOfTempAddr + sizeof(RakNet::SystemAddress);

			////system address needs const char* (maybe not const) to be the input
			//RakNet::SystemAddress tempAddress = (char *)packet->data[startOfTempAddr];

			GameObjectChange* s = (GameObjectChange*)packet->data;
			assert(packet->length == sizeof(GameObjectChange));
			if (packet->length != sizeof(GameObjectChange))
			{
				return;
			}

			//make sure its not our first clients address, so we dont resend the first client their own data (remember this is a server command)
			//if (mpServOps->playerList.at(0).second.first != s->sysAddr)
			if (!mpServOps->playerList.at(0).second.first.EqualsExcludingPort(s->sysAddr))
			{
				mpServerHistory->push_back("Queueing Scene Data to be sent to new client");
				mpNewClients->push(s->sysAddr);
			}
		}
		break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("Our connection request has been accepted.\n");
			mpServerHistory->push_back("Our connection request has been accepted.");
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			mpServerHistory->push_back("A connection is incoming.");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			mpServerHistory->push_back("The server is full.");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("A client has disconnected.\n");
			mpServerHistory->push_back("A client has disconnected.");
		}
		break;
		case ID_CONNECTION_LOST:
		{
			printf("A client lost the connection.\n");
			mpServerHistory->push_back("A client lost the connection.");
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
		case ID_GAME_OBJECT_ADD:
		{
			serverPacketHandlerGameObjectAdd(packet);
		}
		break;
		case ID_GAME_OBJECT_EDIT:
		{
			serverPacketHandlerGameObjectEdit(packet);
		}
		break;
		case ID_GAME_OBJECT_REMOVE:
		{
			serverPacketHandlerGameObjectDelete(packet);
		}
		break;
		case ID_GAME_OBJECT_ADD_NEW_CLIENT:
		{
			serverPacketHandlerGameObjectAddToNewClient(packet);
		}
		break;
		default:
			printf("SERVER: Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}


		mpServerPeer->DeallocatePacket(packet);
	}
}

void NetworkManager::serverPacketHandlerGameObjectAdd(RakNet::Packet* p)
{
	GameObjectChange* s = (GameObjectChange*)p->data;
	assert(p->length == sizeof(GameObjectChange));
	if (p->length != sizeof(GameObjectChange))
	{
		return;
	}

	
	std::string smsg = "[SERVER RECIEVED] ";
	smsg += s->ownerStatement;
	mpServerHistory->push_back(smsg);
	
	//send game message one to everyone BUT who originally sent it
	//send function in raknet follows UDP Rules
		mpServerPeer->Send((char*)&(*s), sizeof(*s),			//application
			HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
			p->systemAddress, true);			//internet		SEND TO ALL BUT THIS ADDRESS (true is checked)
	
}

void NetworkManager::serverPacketHandlerGameObjectAddToNewClient(RakNet::Packet* p)
{
	GameObjectChange* s = (GameObjectChange*)p->data;
	assert(p->length == sizeof(GameObjectChange));
	if (p->length != sizeof(GameObjectChange))
	{
		return;
	}


	std::string smsg = "[SERVER RECIEVED] ";
	smsg += s->ownerStatement;
	mpServerHistory->push_back(smsg);

	//send game message one to everyone BUT who originally sent it
	//send function in raknet follows UDP Rules
	mpServerPeer->Send((char*)&(*s), sizeof(*s),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		s->sysAddr, false);			//internet		SEND TO ALL BUT THIS ADDRESS (true is checked)

}

void NetworkManager::serverPacketHandlerGameObjectEdit(RakNet::Packet* p)
{
	GameObjectChange* s = (GameObjectChange*)p->data;
	assert(p->length == sizeof(GameObjectChange));
	if (p->length != sizeof(GameObjectChange))
	{
		return;
	}


	std::string smsg = "[SERVER RECIEVED] ";
	smsg += s->ownerStatement;
	mpServerHistory->push_back(smsg);

	//send game message one to everyone BUT who originally sent it
	//send function in raknet follows UDP Rules
	mpServerPeer->Send((char*)&(*s), sizeof(*s),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		p->systemAddress, true);			//internet		SEND TO ALL BUT THIS ADDRESS (true is checked)

}

void NetworkManager::serverPacketHandlerGameObjectDelete(RakNet::Packet* p)
{
	GameObjectDelete* s = (GameObjectDelete*)p->data;
	assert(p->length == sizeof(GameObjectDelete));
	if (p->length != sizeof(GameObjectDelete))
	{
		return;
	}


	std::string smsg = "[SERVER RECIEVED] ";
	smsg += s->ownerStatement;
	mpServerHistory->push_back(smsg);

	//send game message one to everyone BUT who originally sent it
	//send function in raknet follows UDP Rules
	mpServerPeer->Send((char*)&(*s), sizeof(*s),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		p->systemAddress, true);			//internet		SEND TO ALL BUT THIS ADDRESS (true is checked)

}

void NetworkManager::serverPacketHandlerGameMessageGeneric(RakNet::Packet* p)
{
	GameMessageGeneric* s = (GameMessageGeneric*)p->data;
	assert(p->length == sizeof(GameMessageGeneric));
	if (p->length != sizeof(GameMessageGeneric))
	{
		return;
	}

	printf("[SERVER RECIEVED] %s\n", s->msg);
	//char smsg[512] = "SERVER RECIEVED: ";
	//strcat(smsg, s->msg);
	std::string smsg = "[SERVER RECIEVED] ";
	smsg += s->msg;
	mpServerHistory->push_back(smsg);

	char firstLetter;
	firstLetter = s->msg[0];

	if (firstLetter == '-')
	{
		if (std::find(mpServOps->adminList.begin(), mpServOps->adminList.end(), s->ownerName) != mpServOps->adminList.end())	//make sure owner is on the list
		{
			if (strcmp(s->msg, "-help") == 0)
			{
				char cList[512] = "\nLIST OF ADMIN FUNCTIONS: \n -bh [PLAYERNAME]    to ban player \n -pi [PLAYERNAME]    to ping player	\n -kp [PLAYERNAME]		to kick player";


				GameMessageGeneric msg = {
						(unsigned char)ID_TIMESTAMP,
						RakNet::GetTime(),
						(unsigned char)ID_GAME_MESSAGE_GENERIC,
						"UNUSED",
						RakNet::UNASSIGNED_SYSTEM_ADDRESS,
						"SERVER: "
				};

				strcpy(msg.msg, cList);
				mpServerHistory->push_back(msg.msg);

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

						//log
						mpServerHistory->push_back(resultOut);

						break;
					}
				}

				if (i >= mpServOps->playerList.size())
				{
					resultOut = "SERVER: player does not exist or is already banned";
					printf("SERVER: player does not exist or is already banned");
					mpServerHistory->push_back(resultOut);
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

						//log
						mpServerHistory->push_back(resultOut);
						break;
					}
				}

				if (i >= mpServOps->playerList.size())
				{
					resultOut = "SERVER: player not pingable";
					printf("SERVER: player not pingable");
					mpServerHistory->push_back(resultOut);
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
		
						mpServerHistory->push_back(resultOut);
						break;
					}
				}

				if (i >= mpServOps->playerList.size())
				{
					resultOut = "SERVER: player not available to kick";
					printf("SERVER: player not available to kick");
					mpServerHistory->push_back(resultOut);
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
	char smsg[512] = "SERVER: adding to player list";
	mpServerHistory->push_back(smsg);

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
	char smsg[512] = "SERVER RECIEVED ADMIN LOG IN ATTEMPT\n";
	mpServerHistory->push_back(smsg);

	if (strcmp(s->msg, mpServOps->adminPass) == 0)	//if our password guess matches, add them to the admin list
	{
		//add the message owner to admin list
		mpServOps->adminList.push_back(s->ownerName);
		printf("SUCCESSFUL ADMIN LOG ON.    Current List:\n");

		for (int i = 0; i < mpServOps->adminList.size(); ++i)
		{
			printf(mpServOps->adminList.at(i).c_str());
			printf("\n");

			//chat log
			char smsg[512];
			strcpy(smsg, mpServOps->adminList.at(i).c_str());
			mpServerHistory->push_back(smsg);
		}
	}
	else
	{
		printf("FAILED ADMIN LOG ON\n");
		char smsg[512] = "FAILED ADMIN LOG ON\n";
		mpServerHistory->push_back(smsg);
	}
}

//--------------Handling CLIENT Functionality--------------
void NetworkManager::clientIL_AdminPassEnter(char mesKB[512])
{
	strcpy(mGS->logOn, mesKB);

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

	char clientOut[512] = "[AdminRequest] ";
	strcat(clientOut, msg.msg);
	mpClientChatHistory->push_back(clientOut);

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
	mpClientChatHistory->push_back(s->msg);
}

void NetworkManager::clientPacketHandlerGameObjectAdd(RakNet::Packet* p)
{
	GameObjectChange* s = (GameObjectChange*)p->data;
	assert(p->length == sizeof(GameObjectChange));
	if (p->length != sizeof(GameObjectChange))
	{
		return;
	}

	mpClientChatHistory->push_back("[CLIENT] Recieved Object Added");
	ObjectCommandQueueData ocq;
	ocq.commandType = (unsigned char)OCQ_OBJECT_ADD;
	ocq.objectIndex = s->msgObjectIndex;
	ocq.pos = glm::vec3(s->msgPosX, s->msgPosY, s->msgPosZ);
	ocq.scale = glm::vec3(s->msgScaleX, s->msgScaleY, s->msgScaleZ);
	ocq.rot = glm::vec3(s->msgRotX, s->msgRotY, s->msgRotZ);
	ocq.ambMod = s->msgAmbMod;
	ocq.activatelighting = s->msgActivatelighting;

	mpClientCommands->push(ocq);
}

void NetworkManager::clientPacketHandlerGameObjectAddToNewClient(RakNet::Packet* p)
{
	GameObjectChange* s = (GameObjectChange*)p->data;
	assert(p->length == sizeof(GameObjectChange));
	if (p->length != sizeof(GameObjectChange))
	{
		return;
	}

	mpClientChatHistory->push_back("[CLIENT] Recieved Object From NEW CLIENT STATUS Added");
	ObjectCommandQueueData ocq;
	ocq.commandType = (unsigned char)OCQ_OBJECT_ADD;
	ocq.objectIndex = s->msgObjectIndex;
	ocq.pos = glm::vec3(s->msgPosX, s->msgPosY, s->msgPosZ);
	ocq.scale = glm::vec3(s->msgScaleX, s->msgScaleY, s->msgScaleZ);
	ocq.rot = glm::vec3(s->msgRotX, s->msgRotY, s->msgRotZ);
	ocq.ambMod = s->msgAmbMod;
	ocq.activatelighting = s->msgActivatelighting;

	mpClientCommands->push(ocq);
}

void NetworkManager::clientPacketHandlerGameObjectEdit(RakNet::Packet* p)
{
	GameObjectChange* s = (GameObjectChange*)p->data;
	assert(p->length == sizeof(GameObjectChange));
	if (p->length != sizeof(GameObjectChange))
	{
		return;
	}

	mpClientChatHistory->push_back("[CLIENT] Recieved Object Editted");
	ObjectCommandQueueData ocq;
	ocq.commandType = (unsigned char)OCQ_OBJECT_EDIT;
	ocq.objectIndex = s->msgObjectIndex;
	ocq.pos = glm::vec3(s->msgPosX, s->msgPosY, s->msgPosZ);
	ocq.scale = glm::vec3(s->msgScaleX, s->msgScaleY, s->msgScaleZ);
	ocq.rot = glm::vec3(s->msgRotX, s->msgRotY, s->msgRotZ);
	ocq.ambMod = s->msgAmbMod;
	ocq.activatelighting = s->msgActivatelighting;

	mpClientCommands->push(ocq);
}

void NetworkManager::clientPacketHandlerGameObjectDelete(RakNet::Packet* p)
{
	GameObjectDelete* s = (GameObjectDelete*)p->data;
	assert(p->length == sizeof(GameObjectDelete));
	if (p->length != sizeof(GameObjectDelete))
	{
		return;
	}

	mpClientChatHistory->push_back("[CLIENT] Recieved Object Deleted");
	ObjectCommandQueueData ocq;
	ocq.commandType = (unsigned char)OCQ_OBJECT_REMOVE;
	ocq.objectIndex = s->msgObjectIndex;
	mpClientCommands->push(ocq);
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

	/*if (strcmp(mesKB, "admin_mode") == 0)
	{
		clientIL_AdminPassEnter();
		strcpy(mesKB, "");
	}*/

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

		mpClientChatHistory->push_back(msg.msg);

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
		mpClientChatHistory->push_back(msg.msg);

		//send function in raknet follows UDP Rules
		peer->Send((char*)&msg, sizeof(msg),			//application
			HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
			msg.sysAddr, true);			//internet
			//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
			//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections
	}
}

void NetworkManager::clientIL_AddObject(int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{
	RakNet::RakPeerInterface* peer = mGS->clientPeer;
	//RakNet::Packet* packet;
	//RakNet::MessageID msg = packet->data[0];

	GameObjectChange msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_OBJECT_ADD,
			objectIndex,
			pos.x,			//use these down to just floats rather than glm because passing that data would be a nightmare
			pos.y,
			pos.z,
			scale.x,
			scale.y,
			scale.z,
			rot.x,
			rot.y,
			rot.z,
			ambMod,
			activatelighting,
			RakNet::UNASSIGNED_SYSTEM_ADDRESS,
			"[CLIENT] Object Added Server Command"
	};

	
	mpClientChatHistory->push_back(msg.ownerStatement);

	//send function in raknet follows UDP Rules
	peer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections
}

void NetworkManager::clientIL_AddObjectToNewClient(RakNet::SystemAddress newClientAddress, int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{
	RakNet::RakPeerInterface* peer = mGS->clientPeer;
	//RakNet::Packet* packet;
	//RakNet::MessageID msg = packet->data[0];

	GameObjectChange msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_OBJECT_ADD_NEW_CLIENT,
			objectIndex,
			pos.x,			//use these down to just floats rather than glm because passing that data would be a nightmare
			pos.y,
			pos.z,
			scale.x,
			scale.y,
			scale.z,
			rot.x,
			rot.y,
			rot.z,
			ambMod,
			activatelighting,
			newClientAddress,
			"[CLIENT] Object Added For New Client Server Command"
	};


	mpClientChatHistory->push_back(msg.ownerStatement);

	//send function in raknet follows UDP Rules
	peer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, false);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections
}

void NetworkManager::clientIL_EditObject(int selectionInHierarchy, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting)
{
	RakNet::RakPeerInterface* peer = mGS->clientPeer;
	//RakNet::Packet* packet;
	//RakNet::MessageID msg = packet->data[0];

	GameObjectChange msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_OBJECT_EDIT,
			selectionInHierarchy,
			pos.x,			//use these down to just floats rather than glm because passing that data would be a nightmare
			pos.y,
			pos.z,
			scale.x,
			scale.y,
			scale.z,
			rot.x,
			rot.y,
			rot.z,
			ambMod,
			activatelighting,
			RakNet::UNASSIGNED_SYSTEM_ADDRESS,
			"[CLIENT] Object Edit Server Command"
	};


	mpClientChatHistory->push_back(msg.ownerStatement);

	//send function in raknet follows UDP Rules
	peer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections
}

void NetworkManager::clientIL_DeleteObject(int selectionInHierarchy)
{
	RakNet::RakPeerInterface* peer = mGS->clientPeer;
	//RakNet::Packet* packet;
	//RakNet::MessageID msg = packet->data[0];

	GameObjectDelete msg = {
			(unsigned char)ID_TIMESTAMP,
			RakNet::GetTime(),
			(unsigned char)ID_GAME_OBJECT_REMOVE,
			selectionInHierarchy,
			RakNet::UNASSIGNED_SYSTEM_ADDRESS,
			"[CLIENT] Object Delete Server Command"
	};


	mpClientChatHistory->push_back(msg.ownerStatement);

	//send function in raknet follows UDP Rules
	peer->Send((char*)&msg, sizeof(msg),			//application
		HIGH_PRIORITY, RELIABLE_ORDERED, 0,		//transport
		msg.sysAddr, true);			//internet
		//packet->systemAddress, true);			//putting in "true" for broadcast will send to all connected system addresses EXCEPT the "packet->systemAddress"
		//RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // send to all connections
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
			mpClientChatHistory->push_back("connection attempt failed. CHECK THE IP AND PORT.");
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			mpClientChatHistory->push_back("Another client has disconnected.");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			mpClientChatHistory->push_back("Another client has lost the connection.");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			mpClientChatHistory->push_back("Another client has connected.");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("Our connection request has been accepted.\n");
			mpClientChatHistory->push_back("Our connection request has been accepted.");
		}
		break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			mpClientChatHistory->push_back("A connection is incoming.");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			mpClientChatHistory->push_back("The server is full.");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("We have been disconnected.\n");
			mpClientChatHistory->push_back("We have been disconnected.");
		}
		break;
		case ID_CONNECTION_LOST:
		{
			printf("Connection lost.\n");
			mpClientChatHistory->push_back("Connection lost.");
		}
		break;

		case ID_GAME_MESSAGE_GENERIC:
		{
			clientPacketHandlerGameMessageGeneric(packet);
		}
		break;

		case ID_GAME_OBJECT_ADD:
		{
			clientPacketHandlerGameObjectAdd(packet);
		}
		break;

		case ID_GAME_OBJECT_EDIT:
		{
			clientPacketHandlerGameObjectEdit(packet);
		}
		break;

		case ID_GAME_OBJECT_REMOVE:
		{
			clientPacketHandlerGameObjectDelete(packet);
		}
		break;
		case ID_GAME_OBJECT_ADD_NEW_CLIENT:
		{
			clientPacketHandlerGameObjectAddToNewClient(packet);
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