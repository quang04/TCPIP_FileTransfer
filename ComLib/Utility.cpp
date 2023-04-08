#include "pch.h"
#include <fstream>
#include <filesystem>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")


namespace Utility
{
	namespace Com {

		uint64_t SendRecvBuffer::RecvBuffer(SOCKET s, char* buffer, int bufferSize, int chunkSize)
		{
			int i = 0;
			while (i < bufferSize) {
				const int l = recv(s, &buffer[i], min(chunkSize, bufferSize - i), 0);
				if (l < 0) { return l; } // this is an error
				i += l;
			}
			return i;
		}

		uint64_t SendRecvBuffer::SendBuffer(SOCKET s, const char* buffer, int bufferSize, int chunkSize)
		{
			int i = 0;
			while (i < bufferSize) {
				const int l = send(s, &buffer[i], __min(chunkSize, bufferSize - i), 0);
				if (l < 0) { return l; } // this is an error
				i += l;
			}
			return i;
		}

		int64_t SendRecvFile::GetFileSize(const std::string& fileName)
		{

			FILE* f;
			if (fopen_s(&f, fileName.c_str(), "rb") != 0) {
				return -1;
			}

			//move pointer to an end of file
			_fseeki64(f, 0, SEEK_END);

			const int64_t len = _ftelli64(f);

			fclose(f);

			return len;
		}

		uint8_t SendRecvFile::SendFile(const SOCKET& s, const std::string& filePath, int chunkSize)
		{
			const int64_t fileSize = GetFileSize(filePath);

            if (fileSize < 0) { return 1; }

            std::ifstream file(filePath, std::ifstream::binary);
            if (file.fail()) { return 2; }

            std::string name(std::filesystem::path(filePath).filename().string());

            char* c = new char[name.length() + 1];

            memcpy(c, name.c_str(), name.length());

            c[name.length()] = '\0';

            //Send file's name size
            int nameLength = static_cast<int>(name.length() + 1);

            //Send length of file's name
            if (SendBuffer(s, reinterpret_cast<const char*>(&nameLength), sizeof(nameLength)) != sizeof(nameLength)) {
                return 3;
            }
            //Send file's name
            if (SendBuffer(s, c, nameLength) != nameLength) {
                return 4;
            }

            //Send file size
            if (SendBuffer(s, reinterpret_cast<const char*>(&fileSize), sizeof(fileSize)) != sizeof(fileSize)) {
                return 5;
            }

            //Send file data
            char* buffer = new char[chunkSize];
            bool errored = false;
            int64_t i = fileSize;
            while (i != 0) {
                const int64_t ssize = __min(i, (int64_t)chunkSize);
                if (!file.read(buffer, ssize)) { errored = true; break; }
                const int l = SendBuffer(s, buffer, (int)ssize);
                if (l < 0) { errored = true; break; }
                i -= l;
            }

            delete[] buffer;
            delete[] c;

            file.close();

            return errored ? 6 : 0;
		}

		uint8_t SendRecvFile::RecvFile(const SOCKET& s, int chunkSize)
		{
            //Get length of file name
            int nameSize;
            if (RecvBuffer(s, reinterpret_cast<char*>(&nameSize), sizeof(nameSize)) != sizeof(nameSize)) {
                return 1;
            }

            char* name = new char[nameSize];
            if (RecvBuffer(s, name, nameSize) != nameSize) {
                return 2;
            }

            //Get size of file
            int64_t fileSize;
            if (RecvBuffer(s, reinterpret_cast<char*>(&fileSize), sizeof(fileSize)) != sizeof(fileSize)) {
                return 3;
            }

            //Get file data
            //Open file
            std::ofstream file(name, std::ofstream::binary);

            if (file.fail()) { return 4; }

            char* buffer = new char[chunkSize];
            bool errored = false;
            int64_t i = fileSize;
            while (i != 0) {
                const int r = RecvBuffer(s, buffer, (int)__min(i, chunkSize));
                if ((r < 0) || !file.write(buffer, r)) { errored = true; break; }
                i -= r;
            }
            delete[] name;
            delete[] buffer;

            file.close();

            return errored ? 5 : 0;
		}
	}

	namespace FileAndDlg {

        void LoadFileDlg(std::vector<std::string>& listFiles)
        {
            const int maxFile = 100;

            char* szFileNameIN = new char[MAX_PATH * maxFile];
            char* szFileNameOUT = new char[MAX_PATH * maxFile];


            // get the input file name
            OPENFILENAME ofn;
            ZeroMemory(szFileNameOUT, sizeof(char)* MAX_PATH * maxFile);
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFilter = TEXT("Any File\0*.*\0");
            ofn.lpstrFile = szFileNameOUT;
            ofn.nMaxFile = MAX_PATH * maxFile;
            ofn.lpstrFileTitle = szFileNameIN;
            ofn.nMaxFileTitle = MAX_PATH;
            ofn.lpstrTitle = TEXT("Select an input File");
            ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST ;

            int ret = 0;

            if (!GetOpenFileName(&ofn)) return;

            char* str = ofn.lpstrFile;
            std::string directory = str;
            str += (directory.length() + 1);

            //If select only one item, the directory will be full path file
            if(str[0] == '\0')
                listFiles.emplace_back(std::format("{}", directory));
            else
                listFiles.emplace_back(std::format("{}\\{}", directory, str));


            while (*str) {
                std::string filename = str;
                str += (filename.length() + 1);

                //Avoid null character
                if (str[0] == '\0') continue;

                listFiles.emplace_back(std::format("{}\\{}", directory, str));
            }

            delete[] szFileNameIN;
            delete[] szFileNameOUT;
        }

    }
}


