#pragma once

class ComBase
{
public:
	ComBase();
	virtual ~ComBase();

protected:
	SOCKET m_socketMain;
	std::string m_strPort;
	bool m_bIsSocketClose;

	void Close();
	constexpr SOCKET GetMainSocket() const { return m_socketMain; }
	inline std::string GetMainPort() const { return m_strPort; }
public:
	virtual void Start() = 0;
	virtual void Stop() = 0;
};