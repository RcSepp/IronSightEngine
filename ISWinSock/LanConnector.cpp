#include "WinSock.h"

#define ACCEPTLANCONNECTION_HANDSHAKE_NUMRETRIES	2 // Number of times to retry sending accept
#define ACCEPTLANCONNECTION_HANDSHAKE_TIMEOUT		10000 // Time to wait for accept handshake [ms]

void __stdcall LanConnectorClient::ParseMessage(LPVOID sender, LPVOID user, char* msg, int msglen, const sockaddr_in* senderaddr)
{
//MessageBox(NULL, msg, "Client", 0);
	LanConnectorClient* lcclient = (LanConnectorClient*)sender;

	if(strcmp(msg, "Query") == 0)
	{
		// Send response
		lcclient->recvaddr.sin_addr.s_addr = senderaddr->sin_addr.s_addr; // Take over query source address as response destination address
		int sendlen = sendto(lcclient->socklcsend, String("QueryResponse") << lcclient->clientname, strlen("QueryResponse") + lcclient->clientname.length() + 1,
							 NULL, (sockaddr*)&lcclient->recvaddr, sizeof(lcclient->recvaddr));
		if(sendlen == SOCKET_ERROR)
			LOG("Error sending query response");
	}
	else if(strcmp(msg, "Accept") == 0)
	{
		// Send handshake
		lcclient->recvaddr.sin_addr.s_addr = senderaddr->sin_addr.s_addr; // Take over query source address as response destination address
		int sendlen = sendto(lcclient->socklcsend, "AcceptHandshake", strlen("AcceptHandshake") + 1, NULL, (sockaddr*)&lcclient->recvaddr, sizeof(lcclient->recvaddr));
		if(sendlen == SOCKET_ERROR)
			LOG("Error sending accept handshake");

		// Inform user
		if(lcclient->conacceptcbk)
		{
			// Parse address
			String addr = String((int)senderaddr->sin_addr.S_un.S_un_b.s_b1) << String(".") << String((int)senderaddr->sin_addr.S_un.S_un_b.s_b2) << String(".") <<
						  String((int)senderaddr->sin_addr.S_un.S_un_b.s_b3) << String(".") << String((int)senderaddr->sin_addr.S_un.S_un_b.s_b4);

			// Queue event
			((WinSock*)&*wsk)->QueueEvent(new ConAcceptEvent(lcclient->conacceptcbk, addr, lcclient->conacceptcbkuser));
			//lcclient->conacceptcbk(addr, lcclient->conacceptcbkuser);
		}
	}
}

void __stdcall WinSock::ParseMessageLC(LPVOID sender, LPVOID user, char* msg, int msglen, const sockaddr_in* senderaddr)
{
//MessageBox(NULL, msg, "Server", 0);
	WinSock* wsk = (WinSock*)sender;

	if(strcmp(msg, "AcceptHandshake") == 0)
{
cle->PrintLine("AcceptHandshake");
		SetEvent(wsk->handshakeevent);
}
	else if(strncmp(msg, "QueryResponse", strlen("QueryResponse")) == 0 && wsk->appfoundcbk)
	{
		// Parse address
		String addr = String((int)senderaddr->sin_addr.S_un.S_un_b.s_b1) << String(".") << String((int)senderaddr->sin_addr.S_un.S_un_b.s_b2) << String(".") <<
					  String((int)senderaddr->sin_addr.S_un.S_un_b.s_b3) << String(".") << String((int)senderaddr->sin_addr.S_un.S_un_b.s_b4);

		// Queue event
		String clientname;
		String(msg).Substring(strlen("QueryResponse"), &clientname);
		wsk->QueueEvent(new AppFoundEvent(wsk->appfoundcbk, clientname, addr, wsk->appfoundcbkuser));
		//wsk->appfoundcbk(String(msg), addr, ((WinSock*)user)->appfoundcbkuser);
	}
}

//-----------------------------------------------------------------------------
// Name: QuerryLanConnections()
// Desc: Create UDP sockets and send requests to lan connector clients on other machines
//-----------------------------------------------------------------------------
Result WinSock::QuerryLanConnections(WSIPVersion ipversion, const String& interfaceaddr, u_short queryport, u_short responseport, APPFOUND_CALLBACK appfoundcbk, LPVOID appfoundcbkuser)
{
	int irlt;
	Result rlt;

	this->appfoundcbk = appfoundcbk;
	this->appfoundcbkuser = appfoundcbkuser;

	// Close old sockets
	CLOSESOCKET(socklcsend);
	CLOSESOCKET(socklcrecv);

	int ipv;
	switch(ipversion)
	{
	case IP_V4: ipv = AF_INET; break;
	case IP_V6: ipv = AF_INET6; break;
	default: ipv = AF_UNSPEC;
	}

	// >>> Receive socket

	// Create a socket to receive datagrams
	socklcrecv = socket(ipv, SOCK_DGRAM, IPPROTO_UDP);
	if(socklcrecv == INVALID_SOCKET)
		return Error(SOCKET_ERROR);

	// Bind the socket to any address and the specified port
	sockaddr_in myrecvaddr;
	ZeroMemory(&myrecvaddr, sizeof(myrecvaddr));
	myrecvaddr.sin_family = ipv;
	myrecvaddr.sin_port = htons(responseport);
	myrecvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if((irlt = bind(socklcrecv, (SOCKADDR*)&myrecvaddr, sizeof(myrecvaddr))) != 0)
	{
		CLOSESOCKET(socklcrecv);
		return Error(SOCKET_ERROR);
	}

	// Start listening
	lclistenerthread = CreateThread(NULL, 0, ListenerThread, (LPVOID)new ListenerThreadArgs(this, NULL, socklcrecv, ParseMessageLC), 0, NULL);

	// >>> Open send socket

	// Create a socket for sending data
	socklcsend = socket(ipv, SOCK_DGRAM, IPPROTO_UDP);
	if(socklcsend == INVALID_SOCKET)
		return Error(SOCKET_ERROR);

	// Enable broadcast sending
	BOOL t = true;
	int r = setsockopt(socklcsend, SOL_SOCKET, SO_BROADCAST, (LPSTR)&t, sizeof(BOOL));

	// Set up the receive address structure with the IP address of the broadcast interface address and the specified port number
	sockaddr_in recvaddr;
	ZeroMemory(&recvaddr, sizeof(recvaddr));
	recvaddr.sin_family = ipv;
	recvaddr.sin_port = htons(queryport);
	recvaddr.sin_addr.s_addr = inet_addr(interfaceaddr);

	// >>> Send broadcast message

	int sendlen = sendto(socklcsend, "Query", strlen("Query") + 1, NULL, (sockaddr*)&recvaddr, sizeof(recvaddr));
	if(sendlen == SOCKET_ERROR)
		return Error(SOCKET_ERROR);

	// >>> Close send socket

	CLOSESOCKET(socklcsend);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: QuerryLanConnections()
// Desc: Create UDP sockets and send requests to lan connector clients on other machines
//-----------------------------------------------------------------------------
Result WinSock::AcceptLanConnection(WSIPVersion ipversion, const String& address, u_short queryport, u_short responseport)
{
	Result rlt;

	this->appfoundcbk = appfoundcbk;
	this->appfoundcbkuser = appfoundcbkuser;

	// Close old send socket
	CLOSESOCKET(socklcsend);

	int ipv;
	switch(ipversion)
	{
	case IP_V4: ipv = AF_INET; break;
	case IP_V6: ipv = AF_INET6; break;
	default: ipv = AF_UNSPEC;
	}

	// >>> Open send socket

	// Create a socket for sending data
	socklcsend = socket(ipv, SOCK_DGRAM, IPPROTO_UDP);
	if(socklcsend == INVALID_SOCKET)
		return Error(SOCKET_ERROR);

	// Enable broadcast sending
	BOOL t = true;
	int r = setsockopt(socklcsend, SOL_SOCKET, SO_BROADCAST, (LPSTR)&t, sizeof(BOOL));

	// Set up the receive address structure with the destination IP address and the specified port number
	sockaddr_in recvaddr;
	ZeroMemory(&recvaddr, sizeof(recvaddr));
	recvaddr.sin_family = ipv;
	recvaddr.sin_port = htons(queryport);
	recvaddr.sin_addr.s_addr = inet_addr(address);

	// >>> Accept connection with handshake

	// Create handshake event
	handshakeevent = CreateEvent(NULL, TRUE, FALSE, "HandshakeEvent");
	if(!handshakeevent)
		return ERR(String("Error creating event: ") << String((int)GetLastError()));

	for(BYTE retry = 0; retry < ACCEPTLANCONNECTION_HANDSHAKE_NUMRETRIES; retry++)
	{
		// Send accept message
		int sendlen = sendto(socklcsend, "Accept", strlen("Accept") + 1, NULL, (sockaddr*)&recvaddr, sizeof(recvaddr));
		if(sendlen == SOCKET_ERROR)
			return Error(SOCKET_ERROR);

		// Wait for handshake
cle->PrintLine("WaitForSingleObject");
		if(!WaitForSingleObject(handshakeevent, ACCEPTLANCONNECTION_HANDSHAKE_TIMEOUT))
		{
cle->PrintLine("success");
			// Handshake received
			CLOSESOCKET(socklcsend);
			return R_OK;
		}
cle->PrintLine("timeout");

		// Timeout expired. Try again
	}

	// No handshake received
	CLOSESOCKET(socklcsend);
	return ERR(String("Error connecting to ") << address);
}

//-----------------------------------------------------------------------------
// Name: LanConnectorClient()
// Desc: Constructor
//-----------------------------------------------------------------------------
LanConnectorClient::LanConnectorClient(WinSock* parent, WSIPVersion ipversion, const String& clientname) :
	socklcsend(INVALID_SOCKET), socklcrecv(INVALID_SOCKET), listenerthread(NULL), ipversion(ipversion), clientname(clientname), conacceptcbk(NULL)
{
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Create socket and thread to listen for incoming lan connector queries
//-----------------------------------------------------------------------------
Result LanConnectorClient::Init(u_short queryport, u_short responseport)
{
	int irlt;
	Result rlt;

	// Close old sockets
	CLOSESOCKET(socklcsend);
	CLOSESOCKET(socklcrecv);

	int ipv;
	switch(ipversion)
	{
	case IP_V4: ipv = AF_INET; break;
	case IP_V6: ipv = AF_INET6; break;
	default: ipv = AF_UNSPEC;
	}

	// >>> Send socket

	// Create a socket for sending data
	socklcsend = socket(ipv, SOCK_DGRAM, IPPROTO_UDP);
	if(socklcsend == INVALID_SOCKET)
		return Error(SOCKET_ERROR);

	// Enable broadcast sending
	BOOL t = true;
	int r = setsockopt(socklcsend, SOL_SOCKET, SO_BROADCAST, (LPSTR)&t, sizeof(BOOL));

	// Set up the receive address structure with the specified port number (the ip address will be set later)
	ZeroMemory(&recvaddr, sizeof(recvaddr));
	recvaddr.sin_family = ipv;
	recvaddr.sin_port = htons(responseport);

	// >>> Receive socket

	// Create a socket to receive datagrams
	socklcrecv = socket(ipv, SOCK_DGRAM, IPPROTO_UDP);
	if(socklcrecv == INVALID_SOCKET)
		return Error(SOCKET_ERROR);

	// Bind the socket to any address and the specified port
	sockaddr_in myrecvaddr;
	ZeroMemory(&myrecvaddr, sizeof(myrecvaddr));
	myrecvaddr.sin_family = ipv;
	myrecvaddr.sin_port = htons(queryport);
	myrecvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if((irlt = bind(socklcrecv, (SOCKADDR*)&myrecvaddr, sizeof(myrecvaddr))) != 0)
	{
		CLOSESOCKET(socklcrecv);
		return Error(SOCKET_ERROR);
	}

	// Start listening
	listenerthread = CreateThread(NULL, 0, ListenerThread, (LPVOID)new ListenerThreadArgs(this, NULL, socklcrecv, ParseMessage), 0, NULL);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void LanConnectorClient::Release()
{
	CLOSESOCKET(socklcsend);
	CLOSESOCKET(socklcrecv);
}