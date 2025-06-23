#pragma once
#include <string>
#include <vector>
#include <WinSock2.h>

#pragma warning(disable:4996)

class AStringStatic
{
public:
	static std::vector<std::string> BreakString(std::string BaseString, char Breaker);
	static std::string GetIp(sockaddr_in Addr);
	static int GetPort(sockaddr_in Addr);
};

