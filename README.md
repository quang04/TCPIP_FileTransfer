# TCPIP_FileTransfer
File Transfer TCPIP by using winsock

# How to use
Master will acts like sender, while slave will acts like receiver. Support send multiple files

Start a server, server will automaticallty find available port in machine to bound. Start slave connect to server, after get connected slave will start while loop to receive file.

# How to run code
Visual studio 2019(v142), x64, C++20, Window SDK 10.0 or above

# Constraints
The current solution is not suitable for large scale connection. Will improve later by using [IOCP](https://learn.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports)



