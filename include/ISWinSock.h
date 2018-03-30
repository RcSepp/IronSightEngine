#ifndef __ISWINSOCK_H
#define __ISWINSOCK_H

#include <ISEngine.h>


//-----------------------------------------------------------------------------
// MACROS
//-----------------------------------------------------------------------------
#ifdef EXPORT_WINSOCK
	#define WINSOCK_EXTERN_FUNC	__declspec(dllexport)
#else
	#define WINSOCK_EXTERN_FUNC	__declspec(dllimport)
#endif


//-----------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------
enum WSProtocol
	{P_TCP, P_UDP};
enum WSIPVersion
	{IP_V4, IP_V6};


//-----------------------------------------------------------------------------
// CALLBACKS
//-----------------------------------------------------------------------------
class IWSUnified;
typedef void (__stdcall* RECEIVE_CALLBACK)(char* buffer, int buflen, IWSUnified* wsu, LPVOID user);
typedef void (__stdcall* APPFOUND_CALLBACK)(const String& clientname, const String& address, LPVOID user);
typedef void (__stdcall* CONACCEPT_CALLBACK)(const String& address, LPVOID user);


//-----------------------------------------------------------------------------
// Name: IWSUnified
// Desc: Unified base type with methodes applicable to both WSServer and WSClient class
//-----------------------------------------------------------------------------
typedef class IWSUnified
{
public:

	virtual Result SendData(LPVOID data, int datalen) = 0;
	virtual Result SendFile(const FilePath& filename) = 0;
	virtual Result Ping(DWORD &ping) = 0;
	virtual inline bool IsServer() = 0;
	virtual inline bool IsClient() = 0;
}* LPWSUNIFIED;

//-----------------------------------------------------------------------------
// Name: IWSServer
// Desc: Interface to the WSServer class
//-----------------------------------------------------------------------------
typedef class IWSServer : public IWSUnified
{
public:

	virtual Result Setup(String port, bool blocking) = 0;
	//virtual Result SendData(LPVOID data, int datalen) = 0;
	//virtual Result Ping(DWORD &ping) = 0;
}* LPWSSERVER;

//-----------------------------------------------------------------------------
// Name: IWSClient
// Desc: Interface to the WSClient class
//-----------------------------------------------------------------------------
typedef class IWSClient : public IWSUnified
{
public:

	virtual Result Connect(String address, String port, bool &serverfound) = 0;
	//virtual Result SendData(LPVOID data, int datalen) = 0;
	//virtual Result Ping(DWORD &ping) = 0;
}* LPWSCLIENT;

//-----------------------------------------------------------------------------
// Name: IWSUdpBridge
// Desc: Interface to the WSUdpBridge class
//-----------------------------------------------------------------------------
typedef class IWSUdpBridge : public IWSUnified
{
public:
}* LPWSUDPBRIDGE;

//-----------------------------------------------------------------------------
// Name: ILCClient
// Desc: Interface to the LanConnectorClient class
//-----------------------------------------------------------------------------
typedef class ILCClient
{
public:
	virtual void SetConnectionAcceptCallback(const CONACCEPT_CALLBACK conacceptcbk, LPVOID user) = 0;
}* LPLCCLIENT;

//-----------------------------------------------------------------------------
// Name: IWinSock
// Desc: Interface to the WinSock class
//-----------------------------------------------------------------------------
typedef class IWinSock
{
public:

	virtual String GetComputerName() const = 0;

	virtual void Sync(GLOBALVARDEF_LIST) = 0;
	virtual Result Init(HWND wnd = NULL, UINT updatemsg = WM_APP) = 0;
	virtual Result CreateServer(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, IWSServer** server) = 0;
	virtual Result CreateClient(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, IWSClient** client) = 0;
	virtual Result CreateAndSetupUnified(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, String address, String port, IWSUnified** wsu) = 0;
	virtual Result CreateUdpBridge(WSIPVersion ipversion, RECEIVE_CALLBACK onrecv, LPVOID user, const String& address, unsigned short sendport, unsigned short recvport, IWSUdpBridge** bridge) = 0;
	virtual Result CreateAndSetupLanConnectorClient(WSIPVersion ipversion, unsigned short queryport, unsigned short responseport, const String& clientname, ILCClient** lcclient) = 0;
	virtual Result QuerryLanConnections(WSIPVersion ipversion, const String& interfaceaddr, unsigned short queryport,
										unsigned short responseport, APPFOUND_CALLBACK appfoundcbk, LPVOID appfoundcbkuser = NULL) = 0;
	virtual Result AcceptLanConnection(WSIPVersion ipversion, const String& address, unsigned short queryport, unsigned short responseport) = 0;
	virtual void Update() = 0;
	virtual void Release() = 0;
}* LPWINSOCK;

extern "C" WINSOCK_EXTERN_FUNC LPWINSOCK __cdecl CreateWinSock();

#endif