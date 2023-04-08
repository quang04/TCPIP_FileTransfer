#include "pch.h"
#include "Server.h"



Server::Server(const std::string& strPort)
	: m_bIsSendComplete(false)
{
	m_strPort = strPort;

}

Server::~Server()
{
}

void Server::Start()
{
	std::cout << "Server Start" << std::endl;

	if (m_strPort != "") RegiterServer(m_strPort);
	else RegisterServer();
}

void Server::Stop()
{
	std::cout << "Server Stop" << std::endl;

	Close();

	//Stop first
	if (m_threadSendFile.joinable())
		m_threadSendFile.join();
}

bool Server::IsSendFileComplete()
{
	bool result = false;

	auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(10);

	std::unique_lock<std::mutex> lock(m_mutexFile);

	//Check notify done after each 10ms
	if (m_bIsSendComplete && m_cvFile.wait_until(lock, timeout) == std::cv_status::no_timeout)
		result = true;

	return result;
}


void Server::SendFiles(const std::vector<std::string>& filesFullPath)
{
	std::cout << "Start Sending File" << std::endl;
	m_bIsSendComplete = false;

	//Stop first
	if (m_threadSendFile.joinable())
		m_threadSendFile.join();

	m_threadSendFile = std::thread([this, filesFullPath]()
		{
			for(auto const &item : filesFullPath)
				Utility::Com::SendRecvFile::SendFile(m_socketMain, item);

			//Notify when done
			std::unique_lock<std::mutex> lock(m_mutexFile);
			m_bIsSendComplete = true;
			lock.unlock();
			m_cvFile.notify_one();
		});


}

uint8_t Server::RegisterServer()
{
	SOCKET ServerSocket = INVALID_SOCKET;
	struct sockaddr_in sa_in;
	int nResult;

	// Create a SOCKET for connecting to server
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		std::cout << "Fail 1" << std::endl;
		return 1;
	}

	//In case random pick same port
	int64_t optval = 1;
	nResult = setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (nResult == SOCKET_ERROR) {
		std::cout << "Fail 2" << std::endl;
		return 2;
	}

	////Optional
	optval = CHUNKSIZE * 2;
	nResult = setsockopt(ServerSocket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval));
	if (nResult == SOCKET_ERROR) {
		std::cout << "Fail 3" << " Error: " << WSAGetLastError() << std::endl;
		return 3;
	}

	optval = 1;
	//Disable Nagle algorithm
	nResult = setsockopt(ServerSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	if (nResult == SOCKET_ERROR) {
		std::cout << "Fail 4" << std::endl;
		return 4;
	}

	INT ilen;
	SOCKADDR sa;
	//  Bind the socket to a dynamically assigned port.
	sa.sa_family = AF_INET;
	memset(sa.sa_data, 0, sizeof(sa.sa_data));
	nResult = bind(ServerSocket, &sa, sizeof(sa));

	// Convert the port to the local host byte order.
	ilen = sizeof(sa_in);
	nResult = getsockname(ServerSocket, (struct sockaddr*)&sa_in, &ilen);
	if (nResult == NO_ERROR)
	{
		std::cout << "Server bound to port: " << ntohs(sa_in.sin_port) << std::endl;
	}

	m_strPort = std::to_string(ntohs(sa_in.sin_port));

	nResult = listen(ServerSocket, SOMAXCONN);
	if (nResult == SOCKET_ERROR) {
		std::cout << "Fail 5" << std::endl;
		closesocket(ServerSocket);
		return 5;
	}

	ilen = sizeof(sa);

	//At this point connect was established


	// Accept a client socket
	m_socketMain = accept(ServerSocket, &sa, &ilen);
	if (m_socketMain == INVALID_SOCKET) {
		std::cout << "Fail 6" << " Error: " << WSAGetLastError() << std::endl;
		return 6;
	}

	//At this point client was conencted


	std::cout << "Port: " << m_strPort << " connected" << std::endl;
	// No longer need server socket
	closesocket(ServerSocket);

	return 0;
}

uint8_t Server::RegiterServer(const std::string& strPort)
{
	SOCKET ServerSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int nResult = getaddrinfo(NULL, strPort.c_str(), &hints, &result);
	if (nResult != 0) {
		std::cout << "Fail 1: " << WSAGetLastError() << std::endl;
		return 1;
	}

	// Create a SOCKET for connecting to server
	ServerSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ServerSocket == INVALID_SOCKET) {
		std::cout << "Fail 2: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		return 2;
	}


	int optval = 1;
	nResult = setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
	if (nResult == SOCKET_ERROR) {
		std::cout << "Fail 3: " << WSAGetLastError() << std::endl;
		return 3;
	}

	optval = CHUNKSIZE * 2;
	nResult = setsockopt(ServerSocket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval));
	if (nResult == SOCKET_ERROR) {
		std::cout << "Fail 4" << " Error: " << WSAGetLastError() << std::endl;
		return 4;
	}
	optval = 1;

	//Disable Nagle algorithm
	nResult = setsockopt(ServerSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	if (nResult == SOCKET_ERROR) {
		std::cout << "Fail 5" << std::endl;
		return 5;
	}


	// Setup the TCP listening socket
	nResult = bind(ServerSocket, result->ai_addr, (int)result->ai_addrlen);
	if (nResult == SOCKET_ERROR) {
		std::cout << "Fail 6: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(ServerSocket);
		return 6;
	}

	freeaddrinfo(result);

	nResult = listen(ServerSocket, SOMAXCONN);
	if (nResult == SOCKET_ERROR) {
		std::cout << "Fail 7: " << WSAGetLastError() << std::endl;
		closesocket(ServerSocket);
		return 7;
	}

	//At this point connect was established

	// Accept a client socket
	m_socketMain = accept(ServerSocket, NULL, NULL);
	if (m_socketMain == INVALID_SOCKET) {
		std::cout << "Fail 8: " << WSAGetLastError() << std::endl;
		return 8;
	}

	//At this point client was conencted

	std::cout << "Port: " << m_strPort << " connected" << std::endl;

	// No longer need server socket
	closesocket(ServerSocket);

	return 0;
}

