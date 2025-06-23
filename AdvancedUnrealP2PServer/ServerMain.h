#pragma once
#include <WinSock2.h>
#include <string>
#include <vector>

#pragma comment(lib,"ws2_32.lib")

#pragma warning(disable:4996)

struct FSession
{
	sockaddr_in Message;
	sockaddr_in Listen;
	int Id = 0;
	std::string Property = " ";
	int Time = 0;
	bool UsePassword = false;
};

class AServerMain
{
public:
	int Execute();
private:
	void InitNetwork();
	void LoopRecv();
	void Send(std::string InStr, sockaddr_in InAddr);

	void HandleMessage(std::string InStr, sockaddr_in InAddr);

	void CmdMessage(std::vector<std::string> Params, sockaddr_in InAddr);
	void CmdListen(std::vector<std::string> Params, sockaddr_in InAddr);
	void CmdPingme(std::vector<std::string> Params, sockaddr_in InAddr);
	void CmdGet(std::vector<std::string> Params, sockaddr_in InAddr);

	void ThreadTimer();

public:
	int Port = 1145;
	int MaxConnectTime = 3;
private:
	WSADATA Wsadata;
	SOCKET Socket;
	sockaddr_in LocalAddr;

	std::vector<FSession> SessionList;
	int IDStart = 100;
};

