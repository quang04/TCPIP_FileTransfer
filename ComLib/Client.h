#pragma once

#include "ComBase.h"


class Client : ComBase
{
public:
	Client(const std::string& strPort, const std::string& strIPAddress);
	Client(const Client& other) = delete;
	Client& operator=(const Client& other) = delete;
	Client(Client&& other) noexcept;
	Client& operator=(Client&& other) noexcept;
	virtual ~Client();
private:
	std::string m_strIPAddress;
public:
	/// <summary>
	/// Entry ppint
	/// </summary>
	void Start() override;

	/// <summary>
	/// Exit point
	/// </summary>
	void Stop() override;
	std::thread m_threadReceiveFile;
private:
	uint8_t RegisterClient();
	inline std::string GetIPAddress() const { return m_strIPAddress; }
};