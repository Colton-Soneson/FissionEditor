#pragma once
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <queue>

#include "Scene.h"
#include "Source/Raknet/Source/RakPeerInterface.h";
#include "Source/Raknet/Source/MessageIdentifiers.h"
#include "Source/Raknet/Source/BitStream.h"
#include "Source/Raknet/Source/GetTime.h"
#include "Source/Raknet/Source/RakNetTypes.h"			// MessageID


enum GameMessages
{
	ID_GAME_MESSAGE_GENERIC = ID_USER_PACKET_ENUM + 1,
	ID_GAME_LOG_ON = ID_USER_PACKET_ENUM + 2,
	ID_GAME_REGISTER_NAME = ID_USER_PACKET_ENUM + 3,
	ID_GAME_OBJECT_ADD = ID_USER_PACKET_ENUM + 4,
	ID_GAME_OBJECT_EDIT = ID_USER_PACKET_ENUM + 5,
	ID_GAME_OBJECT_REMOVE = ID_USER_PACKET_ENUM + 6
};

struct GameState
{
	unsigned short SERVER_PORT;
	char* SERVER_IP;

	//in here is where the peer lives
	RakNet::RakPeerInterface* clientPeer;

	//game data goes here
	char clientName[512];

	char logOn[512];
};

struct ServerOptions
{
	ServerOptions(unsigned short port, unsigned short maxConnections)
	{
		SERVER_PORT = port;
		MAX_CLIENTS = maxConnections;
	}

	unsigned short SERVER_PORT = 4024;
	unsigned short MAX_CLIENTS = 10;
	char adminPass[512];
	std::vector<std::string> adminList;		//max number clients with their names
	std::vector<std::pair<std::string, std::pair<RakNet::SystemAddress, int>>> playerList;	//list of players with their IPs and ports (first player, then ip)
};

#pragma pack (push)	//this is because with msgId and msg only, instead of 513 bytes you have 520 (for the included pointer)
#pragma pack (1)		//number of bytes to pack
struct GameMessageGeneric
{
	//HAS TO BE THIS ORDER
	// if timespaming, then 1) time ID, 2) time
	unsigned char timeID;		 
	RakNet::Time timeStamp; //assign using RakNet::GetTime();				

	// id: use char for byte limit
	unsigned char msgID;

	// contains actual message itself
	char msg[512];	//or you could do char* msg, but pointers are only valid locally

	//system address of the player that owns the msgID
	RakNet::SystemAddress sysAddr;

	//owner name
	char ownerName[512];
};
#pragma pack (pop)


#pragma pack (push)	
#pragma pack (1)		//number of bytes to pack
struct GameAddObject
{
	//HAS TO BE THIS ORDER
	// if timespaming, then 1) time ID, 2) time
	unsigned char timeID;		 
	RakNet::Time timeStamp; //assign using RakNet::GetTime();		

	// id: use char for byte limit
	unsigned char msgID;

	// contains actual data
	int msgObjectIndex;
	float msgPosX;			//use these down to just floats rather than glm because passing that data would be a nightmare
	float msgPosY; 
	float msgPosZ; 
	float msgScaleX;
	float msgScaleY;
	float msgScaleZ;
	float msgRotX;
	float msgRotY;
	float msgRotZ;
	float msgAmbMod;
	bool msgActivatelighting;

	//system address of the player that owns the msgID
	RakNet::SystemAddress sysAddr;

	//owner name
	char ownerStatement[512];
};
#pragma pack (pop)


class NetworkManager
{
public:
	NetworkManager(std::vector<std::string>* serverHistory, std::vector<std::string>* clientChatHistory, std::queue<ObjectCommandQueueData>* ocqList)
	{
		mpServerPeer = nullptr;
		mServerActive = false;
		mClientActive = false;
		mpServOps = nullptr;
		mpClientChatHistory = clientChatHistory;
		mpServerHistory = serverHistory;
		mpClientCommands = ocqList;
	};
	~NetworkManager() {};

	void init();
	
	void initServer(unsigned short port, unsigned short maxConnections, char serverPass[512]);
	void initClient(char nameType[512], unsigned short serverPort, char* serverIP);

	void updateServer();
	void updateClient();

	void closeServer();
	void closeClient();

	void sendClientMessage(char mesKB[512]);
	void clientInputPasswordGuess(char mesKB[512]);
	void sendClientAdminRequest(char mesKB[512]);
	void clientObjectAddSend(int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting);

	void sendServerMessage(char mesKB[512]);

	bool isLocalServerActive() { return mServerActive; };
	bool isLocalClientActive() { return mClientActive; };

private:
	//both client and server
	unsigned char getPacketIdentifier(RakNet::Packet* p);

	//server specific
	void serverIL_GenericMessage(char mesKB[512]);
	void serverIL_AddObject(int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting);
	void serverHandleInputRemote();
	void serverPacketHandlerGameMessageGeneric(RakNet::Packet* p);
	void serverPacketHandlerIncomingPlayer(RakNet::Packet* p);
	void serverPacketHandlerGameLogOn(RakNet::Packet* p);
	void serverPacketHandlerGameObjectAdd(RakNet::Packet* p);

	//client specific
	void clientPacketHandlerGameMessageGeneric(RakNet::Packet* p);
	void clientPacketHandlerGameObjectAdd(RakNet::Packet* p);
	void clientIL_AdminPassEnter(char mesKB[512]);
	void clientIL_GenericMessage(char mesKB[512]);
	void clientIL_AddObject(int objectIndex, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot, float ambMod, bool activatelighting);
	void clientHandleInputRemote();

	GameState mGS[1] = { 0 };				//array of size one gives address
	RakNet::RakPeerInterface* mpServerPeer;

	bool mServerActive;
	bool mClientActive;
	
	ServerOptions* mpServOps;

	//LOGS
	std::vector<std::string>* mpClientChatHistory;
	std::vector<std::string>* mpServerHistory;

	//Queue Pointer (owned by options window)
	std::queue<ObjectCommandQueueData>* mpClientCommands;
};