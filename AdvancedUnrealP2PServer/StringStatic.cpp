#include "StringStatic.h"

#include <iostream>

std::vector<std::string> AStringStatic::BreakString(std::string BaseString, char Breaker)
{
	std::vector<std::string> rtn_value;

	std::string left_string = BaseString;
	while (true)
	{
		size_t pos = left_string.find(Breaker);
		if (pos == std::string::npos)
		{
			break;
		}
		std::string sub_str = left_string.substr(0, pos);
		rtn_value.push_back(sub_str);
		left_string = left_string.substr(pos + 1);
	}
	return rtn_value;
}

std::string AStringStatic::GetIp(sockaddr_in Addr)
{
	const std::string ip = inet_ntoa(Addr.sin_addr);
	return ip;
}

int AStringStatic::GetPort(sockaddr_in Addr)
{
	const int port = ntohs(Addr.sin_port);
	return port;
}
