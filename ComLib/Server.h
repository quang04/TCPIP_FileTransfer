#pragma once
#include "ComBase.h"


class Server : ComBase
{
public:
	Server(const std::string& strPort = "");
	Server(const Server& other) = delete;
	Server& operator=(const Server& other) = delete;
	Server(Server&& other) noexcept = delete;
	Server& operator=(Server&& other) noexcept = delete;
	virtual ~Server();
public:
	/// <summary>
	/// Entry point
	/// </summary>
	void Start() override;

	/// <summary>
	/// Exit point
	/// </summary>
	void Stop() override;


	/// <summary>
	/// Checking thread send file complete or not
	/// </summary>
	/// <returns></returns>
	bool IsSendFileComplete();

	/// <summary>
	/// Start sending file thread
	/// </summary>
	/// <param name="filesFullPath"></param>
	void SendFiles(const std::vector<std::string>& filesFullPath);
private:
	/// <summary>
	/// Auto bound to available port
	/// </summary>
	/// <returns></returns>
	uint8_t RegisterServer();

	/// <summary>
	/// Bound to current port
	/// </summary>
	/// <param name="strPort"></param>
	/// <returns></returns>
	uint8_t RegiterServer(const std::string& strPort);

	std::thread m_threadSendFile;

	std::atomic_bool m_bIsSendComplete;

	std::mutex m_mutexFile;

	std::condition_variable m_cvFile;
};
