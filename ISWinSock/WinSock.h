#ifndef __WINSOCK_H
#define __WINSOCK_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "ISWinSock.h"
#include "..\\global\\WSKResult.h"

#include <windows.h>
#include <winsock2.h>
#include <Mswsock.h> // For TransmitFile()
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment (lib, "WS2_32.lib")
#pragma comment (lib, "Mswsock.lib") // For TransmitFile()


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#define CLOSESOCKET(socket)					{if((socket) != INVALID_SOCKET) {closesocket(socket); (socket) = INVALID_SOCKET;}}


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
class WSUnified;
typedef void (__stdcall* PARSEMESSAGE_CALLBACK)(LPVOID sender, LPVOID user, char* msg, int msglen, const sockaddr_in* senderaddr);


//-----------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------
struct ListenerThreadArgs
{
	LPVOID sender, user;
	SOCKET socket;
	PARSEMESSAGE_CALLBACK parsemsgcbk;

	ListenerThreadArgs(LPVOID sender, LPVOID user, SOCKET socket, PARSEMESSAGE_CALLBACK parsemsgcbk)
	{
		this->sender = sender;
		this->user = user;
		this->socket = socket;
		this->parsemsgcbk = parsemsgcbk;
	}
};
DWORD __stdcall ListenerThread(LPVOID args);

struct StoredEvent
{
	virtual void Call() const = 0;
};
struct AppFoundEvent : public StoredEvent
{
	const APPFOUND_CALLBACK cbk;
	const String clientname, address;
	LPVOID user;
	AppFoundEvent(const APPFOUND_CALLBACK cbk, const String& clientname, const String& address, LPVOID user) : cbk(cbk), clientname(clientname), address(address), user(user) {}
	void Call() const {cbk(clientname, address, user);}
};
struct ConAcceptEvent : public StoredEvent
{
	const CONACCEPT_CALLBACK cbk;
	const String address;
	LPVOID user;
	ConAcceptEvent(const CONACCEPT_CALLBACK cbk, const String& address, LPVOID user) : cbk(cbk), address(address), user(user) {}
	void Call() const {cbk(address, user);}
};


class WinSock;


//-----------------------------------------------------------------------------
// Name: WSUnified
// Desc: Unified base type with methodes applicable to both Winsock server and Winsock client
//-----------------------------------------------------------------------------
class WSUnified //EDIT: Change to WSPeer
{
private:
	IWSUnified* iwsu;

protected:
	const WinSock* parent;
	const WSIPVersion ipversion;
	const WSProtocol protocol;
	HANDLE listenerthread, pingrecvevent;
	RECEIVE_CALLBACK onrecv;
	LPVOID onrecvuser;

	static void __stdcall ParseMessage(LPVOID sender, LPVOID user, char* msg, int msglen, const sockaddr_in* senderaddr);
	Result SendData(SOCKET sendsocket, LPVOID data, int datalen);
	Result SendData(SOCKET sendsocket, LPVOID data, int datalen, const sockaddr_in* recvaddr);
	Result SendFile(SOCKET sendsocket, const FilePath& filename);
	Result Ping(SOCKET sendsocket, DWORD &ping, const sockaddr_in* recvaddr);
	void Shutdown(SOCKET allsockets);

public:

	WSUnified(WinSock* parent, WSIPVersion ipversion, WSProtocol protocol, RECEIVE_CALLBACK onrecv, LPVOID user, IWSUnified* iwsu);
};

//-----------------------------------------------------------------------------
// Name: WSServer
// Desc: Winsock server
//-----------------------------------------------------------------------------
class WSServer : public IWSServer, public WSUnified
{
public:
	SOCKET socktcp;

	WSServer(WinSock* parent, WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user);
	Result Setup(String port, bool blocking);
	Result SendData(LPVOID data, int datalen);
	Result SendFile(const FilePath& filename);
	Result Ping(DWORD &ping);
	bool IsServer() {return true;}
	bool IsClient() {return false;}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: WSClient
// Desc: Winsock client
//-----------------------------------------------------------------------------
class WSClient : public IWSClient, public WSUnified
{
public:
	SOCKET socktcp;

	WSClient(WinSock* parent, WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user);
	Result Connect(String address, String port, bool &serverfound);
	Result SendData(LPVOID data, int datalen);
	Result SendFile(const FilePath& filename);
	Result Ping(DWORD &ping);
	bool IsServer() {return false;}
	bool IsClient() {return true;}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: LanConnectorClient
// Desc: Client for querying available clients on a LAN
//-----------------------------------------------------------------------------
class LanConnectorClient : public ILCClient
{
private:
	SOCKET socklcsend, socklcrecv;
	sockaddr_in recvaddr;
	WSIPVersion ipversion;
	HANDLE listenerthread;
	String clientname;
	CONACCEPT_CALLBACK conacceptcbk;
	LPVOID conacceptcbkuser;

	static void __stdcall ParseMessage(LPVOID sender, LPVOID user, char* msg, int msglen, const sockaddr_in* senderaddr);

public:

	LanConnectorClient(WinSock* parent, WSIPVersion ipversion, const String& clientname);
	Result Init(u_short queryport, u_short responseport);
	void SetConnectionAcceptCallback(const CONACCEPT_CALLBACK conacceptcbk, LPVOID user) {this->conacceptcbk = conacceptcbk; conacceptcbkuser = user;}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: WSUdpBridge
// Desc: Winsock client
//-----------------------------------------------------------------------------
class WSUdpBridge : public IWSUdpBridge, public WSUnified
{
public:
	SOCKET socksend, sockrecv;
	sockaddr_in recvaddr;

	WSUdpBridge(WinSock* parent, WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user);
	Result Init(const String& address, u_short sendport, u_short recvport);
	Result SendData(LPVOID data, int datalen);
	Result SendFile(const FilePath& filename) {return ERR("File transfer is not supported for UDP sockets.");}
	Result Ping(DWORD &ping);
	bool IsServer() {return false;}
	bool IsClient() {return true;}
	void Release();
};

//-----------------------------------------------------------------------------
// Name: WinSock
// Desc: API to windows sockets library.
//-----------------------------------------------------------------------------
class WinSock : public IWinSock
{
private:
	std::list<const StoredEvent*> storedevents;
	std::list<WSServer*> servers;
	std::list<WSClient*> clients;
	std::list<WSUdpBridge*> bridges;
	std::list<LanConnectorClient*> lcclients;
	HWND parentwnd;
	UINT updatemsg;
	SOCKET socklcsend, socklcrecv;
	HANDLE lclistenerthread;
	APPFOUND_CALLBACK appfoundcbk;
	LPVOID appfoundcbkuser;
	HANDLE handshakeevent;

	static void __stdcall ParseMessageLC(LPVOID sender, LPVOID user, char* msg, int msglen, const sockaddr_in* senderaddr);

public:
	String GetComputerName() const;
	void QueueEvent(const StoredEvent* evt) {storedevents.push_back(evt); if(parentwnd) PostMessage(parentwnd, updatemsg, NULL, NULL);}

	WinSock();
	void Sync(GLOBALVARDEF_LIST);
	Result Init(HWND wnd, UINT updatemsg);
	Result CreateServer(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, IWSServer** server);
	Result CreateClient(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, IWSClient** client);
	Result CreateAndSetupUnified(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, String address, String port, IWSUnified** wsu);
	Result CreateUdpBridge(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, const String& address, u_short sendport, u_short recvport, IWSUdpBridge** bridge);
	Result CreateAndSetupLanConnectorClient(WSIPVersion ipversion, u_short queryport, u_short responseport, const String& clientname, ILCClient** lcclient);
	Result QuerryLanConnections(WSIPVersion ipversion, const String& interfaceaddr, u_short queryport, u_short responseport, APPFOUND_CALLBACK appfoundcbk, LPVOID appfoundcbkuser);
	Result AcceptLanConnection(WSIPVersion ipversion, const String& address, u_short queryport, u_short responseport);
	void Update();
	void Release();
};

#endif