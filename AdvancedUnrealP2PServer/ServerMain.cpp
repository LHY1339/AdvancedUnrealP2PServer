#include "ServerMain.h"
#include "StringStatic.h"

#include <iostream>
#include <thread>

int AServerMain::Execute()
{
	std::thread th_timer(&AServerMain::ThreadTimer, this);
	th_timer.detach();
	InitNetwork();
	LoopRecv();
	return 0;
}

void AServerMain::InitNetwork()
{
	//Init WSADATA
	if (WSAStartup(MAKEWORD(2, 2), &Wsadata) != 0)
	{
		exit(-1);
	}
	while (true)
	{
		//Init Socket
		Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (Socket == INVALID_SOCKET)
		{
			exit(-1);
		}
		//Init Server Addr
		LocalAddr.sin_family = AF_INET;
		LocalAddr.sin_addr.s_addr = INADDR_ANY;
		LocalAddr.sin_port = htons(Port);
		if (bind(Socket, (sockaddr*)&LocalAddr, sizeof(LocalAddr)) != SOCKET_ERROR)
		{
			break;
		}
		Port++;
	}
	std::cout << "Server Start At Port : " << Port << std::endl;
}

void AServerMain::LoopRecv()
{
	sockaddr_in addr_buffer;
	while (true)
	{
		int addr_size = sizeof(addr_buffer);
		char buffer[2048];
		int recvLen = recvfrom(Socket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&addr_buffer, &addr_size);
		if (recvLen != SOCKET_ERROR)
		{
			buffer[recvLen] = '\0';
			std::string Utf8Str(buffer, recvLen);

			HandleMessage(Utf8Str, addr_buffer);
		}
	}
}

void AServerMain::Send(std::string InStr, sockaddr_in InAddr)
{
	sendto(Socket, InStr.c_str(), (int)InStr.size(), 0, (sockaddr*)&InAddr, sizeof(InAddr));
}


void AServerMain::HandleMessage(std::string InStr, sockaddr_in InAddr)
{
	std::vector<std::string> param_list = AStringStatic::BreakString(InStr, '#');
	if (param_list.size() < 1)
	{
		return;
	}

	if (param_list[0].empty())
	{
		return;
	}
	else if (param_list[0] == "@MessageRegister")
	{
		CmdMessage(param_list, InAddr);
	}
	else if (param_list[0] == "@ListenRegister")
	{
		CmdListen(param_list, InAddr);
	}
	else if (param_list[0] == "@PingMe")
	{
		CmdPingme(param_list, InAddr);
	}
	else if (param_list[0] == "@GetSession")
	{
		CmdGet(param_list, InAddr);
	}
}

void AServerMain::CmdMessage(std::vector<std::string> Params, sockaddr_in InAddr)
{
	for (int i = 0; i < SessionList.size() && i < 30; i++)
	{
		if (AStringStatic::GetIp(InAddr) == AStringStatic::GetIp(SessionList[i].Message) &&
			AStringStatic::GetPort(InAddr) == AStringStatic::GetPort(SessionList[i].Message))
		{
			SessionList[i].Time = MaxConnectTime;
			const std::string send_back = "@ID#" + std::to_string(SessionList[i].Id) + "#\0";
			Send(send_back, InAddr);
			return;
		}
	}
	IDStart += std::rand() % 9 + 1;
	FSession new_session;
	new_session.Id = IDStart;
	new_session.Message = InAddr;
	new_session.Time = MaxConnectTime;
	SessionList.push_back(new_session);
	const std::string send_back = "@ID#" + std::to_string(new_session.Id) + "#\0";
	Send(send_back, InAddr);
	std::cout
		<< "New Session <"
		<< new_session.Id
		<< ">"
		<< std::endl;
}

void AServerMain::CmdListen(std::vector<std::string> Params, sockaddr_in InAddr)
{
	if (Params.size() >= 4)
	{
		for (int i = 0; i < SessionList.size(); i++)
		{
			if (Params[1] == std::to_string(SessionList[i].Id))
			{
				SessionList[i].Listen = InAddr;
				SessionList[i].UsePassword = Params[2] == "true";
				SessionList[i].Property = Params[3];
				return;
			}
		}
	}
}

void AServerMain::CmdPingme(std::vector<std::string> Params, sockaddr_in InAddr)
{
	if (Params.size() >= 4 )
	{
		for (int i = 0; i < SessionList.size(); i++)
		{
			if (AStringStatic::GetIp(SessionList[i].Listen) == Params[1] &&
				std::to_string(AStringStatic::GetPort(SessionList[i].Listen)) == Params[2])
			{
				const std::string send_back =
					"@Ping#" +
					AStringStatic::GetIp(InAddr) +
					"#" +
					std::to_string(AStringStatic::GetPort(InAddr)) +
					"#" +
					Params[3] + 
					"#\0";
				Send(send_back, SessionList[i].Message);
				return;
			}
		}
	}
}

void AServerMain::CmdGet(std::vector<std::string> Params, sockaddr_in InAddr)
{
	std::string send_str = "@Session#";
	for (int i = 0; i < SessionList.size(); i++)
	{
		std::string cur_session_str = "";
		cur_session_str += AStringStatic::GetIp(SessionList[i].Listen) + "/";
		cur_session_str += std::to_string(AStringStatic::GetPort(SessionList[i].Listen)) + "/";
		cur_session_str += std::to_string(SessionList[i].Id) + "/";
		cur_session_str += std::string(SessionList[i].UsePassword ? "true" : "false") + "/";
		cur_session_str += SessionList[i].Property + "/#";
		send_str += cur_session_str;
	}
	Send(send_str, InAddr);
}

void AServerMain::ThreadTimer()
{
	while (true)
	{
		if (SessionList.size() <= 0)
		{
			IDStart = 1000;
		}
		else
		{
			for (int i = 0; i < SessionList.size(); i++)
			{
				SessionList[i].Time--;
				if (SessionList[i].Time <= 0)
				{
					std::cout << "Session Close <" << SessionList[i].Id << ">" << std::endl;
					SessionList.erase(SessionList.begin() + i);
				}
			}
		}
		Sleep(1000);
	}
}
