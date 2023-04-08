// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#define CHUNKSIZE 16384

#include <ws2tcpip.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <string>
#include <iostream>
#include <vector>
#include "Utility.h"
#include <chrono>
#include <memory>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <atomic>

#endif //PCH_H
