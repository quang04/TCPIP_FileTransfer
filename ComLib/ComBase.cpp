#include "pch.h"
#include "ComBase.h"

ComBase::ComBase():
	m_socketMain(INVALID_SOCKET),
	m_strPort(""),
	m_bIsSocketClose(false)
{
	WSADATA wsaData;
	auto iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) std::cout << "WSAStartup failed with error: " << iResult << std::endl;

}

ComBase::~ComBase()
{
}

void ComBase::Close()
{
	if (m_socketMain == INVALID_SOCKET) return;

	m_bIsSocketClose = true;

	auto nResult = shutdown(m_socketMain, SD_BOTH);//Shut down first

	if (nResult == SOCKET_ERROR) Sleep(10);

	closesocket(m_socketMain);

	m_socketMain = INVALID_SOCKET;

	WSACleanup();
}
