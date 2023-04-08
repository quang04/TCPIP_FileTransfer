
#include "pch.h"
#include "Client.h"


int main()
{
	std::cout << "--------------Acts like Receiver---------------" << std::endl;

	std::string strIp;
	std::cout << "Enter ip address: ";
	std::cin >> strIp;

	unsigned int port = 3000;
	std::cout << "Enter port: ";
	std::cin >> port;

	std::unique_ptr<Client> pClient = std::make_unique<Client>(std::to_string(port), strIp);
	pClient->Start();
	std::cout << "Start to Receive File Loop" << std::endl;
	std::cout << "-----------------------------" << std::endl;
	std::cout << "Press any key to close" << std::endl;

	std::cin.clear();
	std::cin.ignore(INT_MAX, '\n');
	std::cin.get();

	pClient->Stop();
}

