
#include "pch.h"
#include "Server.h"

int main()
{
	std::cout << "--------------Acts like Sender---------------" << std::endl;

	std::unique_ptr<Server> pServer = std::make_unique<Server>();

	pServer->Start();

	std::string command;

	while (1)
	{
		std::cout << "------------------------------" << std::endl;
		std::cout << "Enter \"e\" to exit" << std::endl;
		std::cout << "Enter \"c\" to continue" << std::endl;

		std::cout << "Input command: ";
		std::getline(std::cin >> std::ws, command);

		if (command == "e") break;

		//Start to do send file
		std::vector<std::string> filesPath;
		Utility::FileAndDlg::LoadFileDlg(filesPath);

		auto start = std::chrono::steady_clock::now();

		pServer->SendFiles(filesPath);

		//Loop to do animation while sending
		while (1)
		{
			static bool toggleAnimation = false;

			//Do animation
			if (toggleAnimation) std::cout << "\r+x+x+x+x+x+x+x+x+x";
			else std::cout << "\rx+x+x+x+x+x+x+x+x+";

			//Check send file complete or not
			if (pServer->IsSendFileComplete()) break;

			toggleAnimation = !toggleAnimation;
		}

		auto end = std::chrono::steady_clock::now();

		std::cout << "\nSend Completed. Time: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
			<< " ms"
			<< std::endl;
	}

	pServer->Stop();
}
