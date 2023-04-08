#pragma once

namespace Utility
{
	namespace Com {

		class SendRecvBuffer
		{
		protected:
			static uint64_t RecvBuffer(SOCKET s, char* buffer, int bufferSize, int chunkSize = CHUNKSIZE);
			static uint64_t SendBuffer(SOCKET s, const char* buffer, int bufferSize, int chunkSize = CHUNKSIZE);
		};

		class SendRecvFile : SendRecvBuffer
		{
		public:
			static int64_t GetFileSize(const std::string& fileName);
			static uint8_t SendFile(const SOCKET& s, const std::string& filePath, int chunkSize = CHUNKSIZE);
			static uint8_t RecvFile(const SOCKET& s, int chunkSize = CHUNKSIZE);
		};
	}

	namespace FileAndDlg {
		void LoadFileDlg(std::vector<std::string>& listFiles);
	}
};