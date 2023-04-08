#include "pch.h"
#include "Client.h"


Client::Client(const std::string& strPort, const std::string& strIPAddress) :
	m_strIPAddress(strIPAddress)
{
	m_strPort = strPort;
}


Client::Client(Client&& other) noexcept
{
	this->m_bIsSocketClose = std::move(other.m_bIsSocketClose);
	this->m_socketMain = std::move(other.m_socketMain);
	this->m_strIPAddress = std::move(other.m_strIPAddress);
	this->m_strPort = std::move(other.m_strPort);
	this->m_threadReceiveFile = std::move(other.m_threadReceiveFile);
}

Client& Client::operator=(Client&& other) noexcept
{
	this->m_bIsSocketClose = std::move(other.m_bIsSocketClose);
	this->m_socketMain = std::move(other.m_socketMain);
	this->m_strIPAddress = std::move(other.m_strIPAddress);
	this->m_strPort = std::move(other.m_strPort);
	this->m_threadReceiveFile = std::move(other.m_threadReceiveFile);
	return *this;
}

Client::~Client()
{
}

void Client::Start()
{
	std::cout << "Client Start" << std::endl;

	RegisterClient();

	m_threadReceiveFile = std::thread([this]()
		{
			while (true)
			{
				if (m_bIsSocketClose) break;

				if (Utility::Com::SendRecvFile::RecvFile(m_socketMain) == 0) std::cout << "Recieve Success" << std::endl;
				else
				{
					std::cout << "Recieve Error" << std::endl;
					break;
				}
			}
		});
}

void Client::Stop()
{
	std::cout << "Server Stop" << std::endl;

	Close();

	//Wait thread complete
	if(m_threadReceiveFile.joinable())
		m_threadReceiveFile.join();
}

uint8_t Client::RegisterClient()
{
	struct addrinfo* result = NULL, *ptr = NULL, hints;

	int window_size = CHUNKSIZE * 2;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int nResult = getaddrinfo(m_strIPAddress.c_str(), m_strPort.c_str(), &hints, &result);
	if (nResult != 0) {
		std::cout << "Fail 1: " << WSAGetLastError() << std::endl;
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	while (1) {
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
			// Create a SOCKET for connecting to server
			m_socketMain = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (m_socketMain == INVALID_SOCKET) {
				std::cout << "Fail 2: " << WSAGetLastError() << std::endl;
				return 2;
			}
			// Connect to server.
			nResult = connect(m_socketMain, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (nResult == SOCKET_ERROR) {
				closesocket(m_socketMain);
				m_socketMain = INVALID_SOCKET;
				continue;
			}
			break;
		}
		if (nResult != SOCKET_ERROR)
			break;
	}

	freeaddrinfo(result);

	return 0;
}
