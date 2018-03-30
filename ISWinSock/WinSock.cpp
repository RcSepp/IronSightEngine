#include "WinSock.h"


#define PING_TIMEOUT	10000 // Ping timeout [ms]


//-----------------------------------------------------------------------------
// Name: CreateWinSock()
// Desc: DLL API for creating an instance of WinSock
//-----------------------------------------------------------------------------
__declspec(dllexport) LPWINSOCK __cdecl CreateWinSock()
{
	return new WinSock();
}

//-----------------------------------------------------------------------------
// Name: Sync()
// Desc: Synchronize global variables over module boundaries
//-----------------------------------------------------------------------------
void WinSock::Sync(GLOBALVARDEF_LIST)
{
	SYNC_GLOBALVARS
}

DWORD __stdcall ListenerThread(LPVOID args)
{
	SOCKET socklisten = ((ListenerThreadArgs*)args)->socket;
	sockaddr_in SenderAddr;
	int SenderAddrSize;
#define DEFAULT_BUFLEN 65000
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;
    int rlt;

	while(1)
	{
		//rlt = recv(socklisten, recvbuf, recvbuflen, 0);
		SenderAddrSize = sizeof(SenderAddr);
		rlt = recvfrom(socklisten, recvbuf, recvbuflen, 0, (sockaddr*)&SenderAddr, &SenderAddrSize);
		if(rlt > 0)
			((ListenerThreadArgs*)args)->parsemsgcbk(((ListenerThreadArgs*)args)->sender, ((ListenerThreadArgs*)args)->user, recvbuf, rlt, &SenderAddr);
		else if(rlt == 0)
		{
			cle->Print("Connection terminated\n");
			break;
		}
		else
		{
			cle->Print("Error receiving data: "); cle->Print(WSAGetLastError()); cle->Print("\n");
			CLOSESOCKET(socklisten);
			return 1;
		}
	}

	// Cleanup
	delete args;
	CLOSESOCKET(socklisten);
	cle->Print("Connection closed\n");

    return 0;
}

#pragma region Unified

void __stdcall WSUnified::ParseMessage(LPVOID sender, LPVOID user, char* msg, int msglen, const sockaddr_in* senderaddr)
{
	WSUnified* wsu = (WSUnified*)sender;
	if(strcmp(msg, "PING") == 0)
		SetEvent(wsu->pingrecvevent);
	else
		wsu->onrecv(msg, msglen, wsu->iwsu, user);
}

//-----------------------------------------------------------------------------
// Name: WSUnified()
// Desc: Constructor
//-----------------------------------------------------------------------------
WSUnified::WSUnified(WinSock* parent, WSIPVersion ipversion, WSProtocol protocol, RECEIVE_CALLBACK onrecv, LPVOID user, IWSUnified* iwsu)
					: parent(parent), ipversion(ipversion), protocol(protocol), onrecv(onrecv), onrecvuser(user), iwsu(iwsu)
{
	listenerthread = NULL;
}

//-----------------------------------------------------------------------------
// Name: SendData()
// Desc: Send data (connection-bound)
//-----------------------------------------------------------------------------
Result WSUnified::SendData(SOCKET sendsocket, LPVOID data, int datalen)
{
	int sendlen = send(sendsocket, (char*)data, datalen, NULL);
	if(sendlen == SOCKET_ERROR)
		return Error(SOCKET_ERROR);

//cle->Print("Bytes send: "); cle->Print(sendlen); cle->Print("\n");

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SendData()
// Desc: Send data (connectionless)
//-----------------------------------------------------------------------------
Result WSUnified::SendData(SOCKET sendsocket, LPVOID data, int datalen, const sockaddr_in* recvaddr)
{
	int sendlen = sendto(sendsocket, (char*)data, datalen, NULL, (sockaddr*)recvaddr, sizeof(*recvaddr));
	if(sendlen == SOCKET_ERROR)
		return Error(SOCKET_ERROR);

//cle->Print("Bytes send: "); cle->Print(sendlen); cle->Print("\n");

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SendFile()
// Desc: Send a file over network using high performance TransmitFile() function
//-----------------------------------------------------------------------------
Result WSUnified::SendFile(SOCKET sendsocket, const FilePath& filename)
{
	OFSTRUCT ofs;
	HFILE file = OpenFile(filename, &ofs, OF_READ);
	BOOL rlt = TransmitFile(sendsocket, (HANDLE)file, 0, 0, NULL, NULL, 0);
	DWORD err = GetLastError();

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Ping()
// Desc: Send 'PING' over sendsocket and wait for a response
//-----------------------------------------------------------------------------
Result WSUnified::Ping(SOCKET sendsocket, DWORD &ping, const sockaddr_in* recvaddr = NULL)
{
	LARGE_INTEGER freq, tstart, tend;

	// Start timer
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&tstart);

	// Send 'PING'
	int sendlen = recvaddr ? sendto(sendsocket, "PING", strlen("PING"), NULL, (sockaddr*)recvaddr, sizeof(*recvaddr)) : send(sendsocket, "PING", strlen("PING"), NULL);
	if(sendlen == SOCKET_ERROR)
		return Error(SOCKET_ERROR);

	// Wait for response
	DWORD rlt = WaitForSingleObject(pingrecvevent, PING_TIMEOUT);

	if(rlt == WAIT_TIMEOUT)
	{
		// Return timeout error
		ping = (DWORD)-1;
		return ERR("Ping operation timed out");
	}
	else
	{
		// Get time
		QueryPerformanceCounter(&tend);
		ping = (DWORD)((tend.QuadPart - tstart.QuadPart) * 1000 / freq.QuadPart);
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Shutdown()
// Desc: Terminate all connections and close sockets
//-----------------------------------------------------------------------------
void WSUnified::Shutdown(SOCKET allsockets)
{
	// Initiate connection termination by informing all other parties
	if(allsockets != INVALID_SOCKET)
		shutdown(allsockets, SD_SEND);

	// Wait 10sec for connection termination handshake (FIN/ACK)
	if(listenerthread)
		WaitForSingleObject(listenerthread, 10000);

	// Finally, close socket
	CLOSESOCKET(allsockets);
}
#pragma endregion

#pragma region Server
//-----------------------------------------------------------------------------
// Name: WSServer()
// Desc: Constructor
//-----------------------------------------------------------------------------
WSServer::WSServer(WinSock* parent, WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user) : WSUnified(parent, ipversion, P_TCP, onrecv, user, this)
{
	socktcp = INVALID_SOCKET;
}

//-----------------------------------------------------------------------------
// Name: Setup()
// Desc: Create a server
//-----------------------------------------------------------------------------
Result WSServer::Setup(String port, bool blocking)
{
	addrinfo *result = NULL, hints;
	int irlt;
	Result rlt;

	SOCKET socklisten = INVALID_SOCKET;

	// Close old socket
	CLOSESOCKET(socktcp);

	ZeroMemory(&hints, sizeof(hints));
	switch(ipversion)
	{
	case IP_V4: hints.ai_family = AF_INET; break;
	case IP_V6: hints.ai_family = AF_INET6; break;
	default: hints.ai_family = AF_UNSPEC;
	}
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = protocol == P_TCP ? IPPROTO_TCP : IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve local address and port to be used by the server
	if((irlt = getaddrinfo(NULL, port, &hints, &result)) != ERROR_SUCCESS)
		return Error(irlt);

	// Create a socket for the server to listen for client connections
	socklisten = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(socklisten == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		return Error(SOCKET_ERROR);
	}

	// Setup the TCP listening socket
	if(bind(socklisten, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
	{
		freeaddrinfo(result);
		CLOSESOCKET(socklisten);
		return Error(SOCKET_ERROR);
	}
	freeaddrinfo(result);

//int broadcastPermission = 1;
//setsockopt(socklisten, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastPermission, sizeof(broadcastPermission));
	if(listen(socklisten, 1) == SOCKET_ERROR) // Use SOMAXCONN to listen for more than one connection request
	{
		CLOSESOCKET(socklisten);
		return Error(SOCKET_ERROR);
	}

	// Accept a client socket
	if(blocking)
// Use a continous loop when listening for multiple connection requests
		socktcp = accept(socklisten, NULL, NULL); // Use NULL-params to return client address information
	else
	{
		// Set socklisten to non-blocking
		u_long iMode = 1;
		ioctlsocket(socklisten, FIONBIO, &iMode);

		// Accept a client socket
// Use a continous loop when listening for multiple connection requests
		do
		{
			socktcp = accept(socklisten, NULL, NULL); // Use NULL-params to return client address information
			irlt = WSAGetLastError();
			eng->Work();
		} while(irlt == WSAEWOULDBLOCK);
	}

	CLOSESOCKET(socklisten);

	if(socktcp == INVALID_SOCKET)
		return Error(SOCKET_ERROR);

	// Start listening
	listenerthread = CreateThread(NULL, 0, ListenerThread, (LPVOID)new ListenerThreadArgs((WSUnified*)this, onrecvuser, socktcp, WSUnified::ParseMessage), 0, NULL);

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SendData()
// Desc: See WSUnified::SendData()
//-----------------------------------------------------------------------------
Result WSServer::SendData(LPVOID data, int datalen)
{
	return WSUnified::SendData(socktcp, data, datalen);
}

//-----------------------------------------------------------------------------
// Name: SendFile()
// Desc: See WSUnified::SendFile()
//-----------------------------------------------------------------------------
Result WSServer::SendFile(const FilePath& filename)
{
	return WSUnified::SendFile(socktcp, filename);
}

//-----------------------------------------------------------------------------
// Name: Ping()
// Desc: See WSUnified::Ping()
//-----------------------------------------------------------------------------
Result WSServer::Ping(DWORD &ping)
{
	return WSUnified::Ping(socktcp, ping);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void WSServer::Release()
{
	Shutdown(socktcp);
}
#pragma endregion

#pragma region Client
//-----------------------------------------------------------------------------
// Name: WSClient()
// Desc: Constructor
//-----------------------------------------------------------------------------
WSClient::WSClient(WinSock* parent, WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user) : WSUnified(parent, ipversion, P_TCP, onrecv, user, this)
{
	socktcp = INVALID_SOCKET;
}

//-----------------------------------------------------------------------------
// Name: Connect()
// Desc: Connect to a server at the given address and port
//-----------------------------------------------------------------------------
Result WSClient::Connect(String address, String port, bool &serverfound)
{
	addrinfo *result = NULL, *ptr = NULL, hints;
	int irlt;
	Result rlt;

	serverfound = false;

	// Close old socket
	CLOSESOCKET(socktcp);

	ZeroMemory(&hints, sizeof(hints));
	switch(ipversion)
	{
	case IP_V4: hints.ai_family = AF_INET; break;
	case IP_V6: hints.ai_family = AF_INET6; break;
	default: hints.ai_family = AF_UNSPEC;
	}
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve server address and port
	if((irlt = getaddrinfo(address, port, &hints, &result)) != ERROR_SUCCESS)
		return Error(irlt);

// Attempt to connect to the first address returned by the call to getaddrinfo
ptr = result;
// Create a socket for connecting to the server
socktcp = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
if(socktcp == INVALID_SOCKET)
{
	freeaddrinfo(result);
	return Error(SOCKET_ERROR);
}

// Connect to server.
if((irlt = connect(socktcp, ptr->ai_addr, (int)ptr->ai_addrlen)) == SOCKET_ERROR)
	CLOSESOCKET(socktcp);

// Should really try the next address returned by getaddrinfo
// if the connect call failed
// But for this simple example we just free the resources
// returned by getaddrinfo and print an error message

freeaddrinfo(result);

if(socktcp == INVALID_SOCKET)
	return R_OK; // Server not found, return without setting serverfound to true

	// Start listening
	listenerthread = CreateThread(NULL, 0, ListenerThread, (LPVOID)new ListenerThreadArgs((WSUnified*)this, onrecvuser, socktcp, WSUnified::ParseMessage), 0, NULL);

	serverfound = true;
	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SendData()
// Desc: See WSUnified::SendData()
//-----------------------------------------------------------------------------
Result WSClient::SendData(LPVOID data, int datalen)
{
	return WSUnified::SendData(socktcp, data, datalen);
}

//-----------------------------------------------------------------------------
// Name: SendFile()
// Desc: See WSUnified::SendFile()
//-----------------------------------------------------------------------------
Result WSClient::SendFile(const FilePath& filename)
{
	return WSUnified::SendFile(socktcp, filename);
}

//-----------------------------------------------------------------------------
// Name: Ping()
// Desc: See WSUnified::Ping()
//-----------------------------------------------------------------------------
Result WSClient::Ping(DWORD &ping)
{
	return WSUnified::Ping(socktcp, ping);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void WSClient::Release()
{
	Shutdown(socktcp);
}
#pragma endregion

#pragma region UdpBridge
//-----------------------------------------------------------------------------
// Name: WSUdpBridge()
// Desc: Constructor
//-----------------------------------------------------------------------------
WSUdpBridge::WSUdpBridge(WinSock* parent, WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user) : WSUnified(parent, ipversion, P_UDP, onrecv, user, this)
{
	socksend = sockrecv = INVALID_SOCKET;
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Create sockets for sending and/or receiving
//-----------------------------------------------------------------------------
Result WSUdpBridge::Init(const String& address, u_short sendport, u_short recvport)
{
	int irlt;
	Result rlt;

	// Close old sockets
	CLOSESOCKET(socksend);
	CLOSESOCKET(sockrecv);

	int ipv;
	switch(ipversion)
	{
	case IP_V4: ipv = AF_INET; break;
	case IP_V6: ipv = AF_INET6; break;
	default: ipv = AF_UNSPEC;
	}

	if(sendport)
	{
		// >>> Send socket

		// Create a socket for sending data
		socksend = socket(ipv, SOCK_DGRAM, IPPROTO_UDP);
		if(socksend == INVALID_SOCKET)
			return Error(SOCKET_ERROR);

		// Set up the receive address structure with the IP address of the receiver and the specified port number
		ZeroMemory(&recvaddr, sizeof(recvaddr));
		recvaddr.sin_family = ipv;
		recvaddr.sin_port = htons(sendport);
		recvaddr.sin_addr.s_addr = inet_addr(address);
	}

	if(recvport)
	{
		// >>> Receive socket

		// Create a socket to receive datagrams
		sockrecv = socket(ipv, SOCK_DGRAM, IPPROTO_UDP);
		if(sockrecv == INVALID_SOCKET)
			return Error(SOCKET_ERROR);

		// Bind the socket to any address and the specified port
		sockaddr_in myrecvaddr;
		ZeroMemory(&myrecvaddr, sizeof(myrecvaddr));
		myrecvaddr.sin_family = ipv;
		myrecvaddr.sin_port = htons(recvport);
		myrecvaddr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("192.168.1.2");
		if((irlt = bind(sockrecv, (SOCKADDR*)&myrecvaddr, sizeof(myrecvaddr))) != 0)
		{
			CLOSESOCKET(sockrecv);
			return Error(SOCKET_ERROR);
		}

// Start listening
listenerthread = CreateThread(NULL, 0, ListenerThread, (LPVOID)new ListenerThreadArgs((WSUnified*)this, onrecvuser, sockrecv, WSUnified::ParseMessage), 0, NULL);

/*// Receive datagrams on the bound socket
irlt = recvfrom(sockudprecv, RecvBuf, BufLen, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);
if (iResult == SOCKET_ERROR) {
    wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
}*/
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: SendData()
// Desc: See WSUnified::SendData()
//-----------------------------------------------------------------------------
Result WSUdpBridge::SendData(LPVOID data, int datalen)
{
	if(!socksend)
		return ERR("Trying to send on a socket that wasn't created for sending.");
	return WSUnified::SendData(socksend, data, datalen, &recvaddr);
}

//-----------------------------------------------------------------------------
// Name: Ping()
// Desc: See WSUnified::Ping()
//-----------------------------------------------------------------------------
Result WSUdpBridge::Ping(DWORD &ping)
{
	return WSUnified::Ping(socksend, ping, &recvaddr);
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void WSUdpBridge::Release()
{
	CLOSESOCKET(socksend);
	CLOSESOCKET(sockrecv);
}
#pragma endregion

#pragma region WinSock
//-----------------------------------------------------------------------------
// Name: WinSock()
// Desc: Constructor
//-----------------------------------------------------------------------------
WinSock::WinSock() : socklcsend(INVALID_SOCKET), socklcrecv(INVALID_SOCKET), handshakeevent(NULL)
{
}

//-----------------------------------------------------------------------------
// Name: GetComputerName()
// Desc: Return the local computer's name
//-----------------------------------------------------------------------------
String WinSock::GetComputerName() const
{
	TCHAR username[UNLEN + 1];
	DWORD usernamelen = UNLEN + 1;
	::GetComputerName(username, &usernamelen);
	return String(username);
}

//-----------------------------------------------------------------------------
// Name: Init()
// Desc: Initialize Winsock
//-----------------------------------------------------------------------------
Result WinSock::Init(HWND wnd, UINT updatemsg)
{
	this->parentwnd = wnd;
	this->updatemsg = updatemsg;

	WSADATA wsadata;
	return CheckResult(WSAStartup(MAKEWORD(2, 2), &wsadata));
}

//-----------------------------------------------------------------------------
// Name: CreateServer()
// Desc: Create Winsock server
//-----------------------------------------------------------------------------
Result WinSock::CreateServer(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, IWSServer** server)
{
	Result rlt;

	*server = NULL;

	WSServer* newserver;
	CHKALLOC(newserver = new WSServer(this, ipversion, onrecv, user));

	servers.push_back(newserver);
	*server = newserver;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateClient()
// Desc: Create Winsock client
//-----------------------------------------------------------------------------
Result WinSock::CreateClient(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, IWSClient** client)
{
	Result rlt;

	*client = NULL;

	WSClient* newclient;
	CHKALLOC(newclient = new WSClient(this, ipversion, onrecv, user));

	clients.push_back(newclient);
	*client = newclient;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateAndSetupUnified()
// Desc: Try to connect to an existing Winsock server. If it doesn't exist, create and setup a new one
//-----------------------------------------------------------------------------
Result WinSock::CreateAndSetupUnified(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, String address, String port, IWSUnified** wsu)
{
	Result rlt;

	*wsu = NULL;

	// Create client
	WSClient* newclient;
	CHKALLOC(newclient = new WSClient(this, ipversion, onrecv, user));

	// Try to connect to server
	bool serverfound;
	CHKRESULT(newclient->Connect(address, port, serverfound));

	if(serverfound) // Server found
	{
		// Return with client as wsu
		*wsu = newclient;
		clients.push_back(newclient);
	}
	else // Server not found
	{
		// Free client
		newclient->Release();

		// Create and setup new server
		WSServer* newserver;
		CHKALLOC(newserver = new WSServer(this, ipversion, onrecv, user));
		CHKRESULT(newserver->Setup(port, true));

		// Return with server as wsu
		*wsu = newserver;
		servers.push_back(newserver);
	}

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateUdpBridge()
// Desc: Create sockets for sending and/or receiving UDP datagrams
//-----------------------------------------------------------------------------
Result WinSock::CreateUdpBridge(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, const String& address, u_short sendport, u_short recvport, IWSUdpBridge** bridge)
{
	Result rlt;

	*bridge = NULL;

	WSUdpBridge* newbridge;
	CHKALLOC(newbridge = new WSUdpBridge(this, ipversion, onrecv, user));
	CHKRESULT(newbridge->Init(address, sendport, recvport));

	bridges.push_back(newbridge);
	*bridge = newbridge;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: CreateAndSetupLanConnectorClient()
// Desc: Create UDP socket that can be queried from a call to QuerryLanConnections() on another machine
//-----------------------------------------------------------------------------
Result WinSock::CreateAndSetupLanConnectorClient(WSIPVersion ipversion, u_short queryport, u_short responseport, const String& clientname, ILCClient** lcclient)
{
	Result rlt;

	*lcclient = NULL;

	LanConnectorClient* newlcclient;
	CHKALLOC(newlcclient = new LanConnectorClient(this, ipversion, clientname));
	CHKRESULT(newlcclient->Init(queryport, responseport));

	lcclients.push_back(newlcclient);
	*lcclient = newlcclient;

	return R_OK;
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Fire stored events
//-----------------------------------------------------------------------------
void WinSock::Update()
{
	std::list<StoredEvent>::iterator iter;
	while(!storedevents.empty())
	{
		const StoredEvent* evt = storedevents.front();
		storedevents.remove(evt);
		evt->Call();
		delete evt;
	}
}

//-----------------------------------------------------------------------------
// Name: Release()
// Desc: Destructor replacement (destructors can't be overloaded)
//-----------------------------------------------------------------------------
void WinSock::Release()
{
	CLOSESOCKET(socklcsend);
	CLOSESOCKET(socklcrecv);

	for(std::list<WSServer*>::iterator iter = servers.begin(); iter != servers.end(); iter++)
		(*iter)->Release();
	servers.clear();

	for(std::list<WSClient*>::iterator iter = clients.begin(); iter != clients.end(); iter++)
		(*iter)->Release();
	clients.clear();

	for(std::list<WSUdpBridge*>::iterator iter = bridges.begin(); iter != bridges.end(); iter++)
		(*iter)->Release();
	bridges.clear();

	for(std::list<LanConnectorClient*>::iterator iter = lcclients.begin(); iter != lcclients.end(); iter++)
		(*iter)->Release();
	lcclients.clear();

	WSACleanup();
}
#pragma endregion