#pragma once
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "Source/Raknet/Source/RakPeerInterface.h";
#include "Source/Raknet/Source/MessageIdentifiers.h"
#include "Source/Raknet/Source/BitStream.h"
#include "Source/Raknet/Source/GetTime.h"
#include "Source/Raknet/Source/RakNetTypes.h"			// MessageID


enum GameMessages
{
	ID_GAME_MESSAGE_GENERIC = ID_USER_PACKET_ENUM + 1,
	ID_GAME_LOG_ON = ID_USER_PACKET_ENUM + 2,
	ID_GAME_REGISTER_NAME = ID_USER_PACKET_ENUM + 3
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
	unsigned char timeID;		 //												UNCOMMENT!!!!
	RakNet::Time timeStamp; //assign using RakNet::GetTime();				UNCOMMENT!!!!

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

class NetworkManager
{
public:
	NetworkManager() 
	{
		mpServerPeer = nullptr;
		mServerActive = false;
		mClientActive = false;
		mpServOps = nullptr;
	};
	~NetworkManager() {};

	void init();
	
	void initServer(unsigned short port, unsigned short maxConnections, std::vector<std::string>* history);
	void initClient(char nameType[512], unsigned short serverPort, char* serverIP, std::vector<std::string>* history);

	void updateServer(std::vector<std::string> *history);
	void updateClient(std::vector<std::string> *history);

	void closeServer();
	void closeClient();

	void sendClientMessage(char mesKB[512]);
	void sendClientAdminRequest(char mesKB[512]);

	void sendServerMessage(char mesKB[512]);

	bool isLocalServerActive() { return mServerActive; };
	bool isLocalClientActive() { return mClientActive; };

private:
	//both client and server
	unsigned char getPacketIdentifier(RakNet::Packet* p);

	//server specific
	void serverIL_GenericMessage(char mesKB[512]);
	void serverHandleInputRemote(std::vector<std::string>* history);
	void serverPacketHandlerGameMessageGeneric(RakNet::Packet* p, std::vector<std::string>* history);
	void serverPacketHandlerIncomingPlayer(RakNet::Packet* p, std::vector<std::string>* history);
	void serverPacketHandlerGameLogOn(RakNet::Packet* p, std::vector<std::string>* history);

	//client specific
	void clientIL_AdminPassEnter(char mesKB[512]);
	void clientPacketHandlerGameMessageGeneric(RakNet::Packet* p, std::vector<std::string>* history);
	void clientIL_GenericMessage(char mesKB[512]);
	void clientHandleInputRemote(std::vector<std::string>* history);

	GameState mGS[1] = { 0 };				//array of size one gives address
	RakNet::RakPeerInterface* mpServerPeer;

	bool mServerActive;
	bool mClientActive;
	
	ServerOptions* mpServOps;
};