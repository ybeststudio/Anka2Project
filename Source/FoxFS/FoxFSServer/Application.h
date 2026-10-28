#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <winsock2.h>
#include <windows.h>
#pragma comment( lib, "ws2_32.lib" )
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include <iomanip>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <cstring>
#include <cstdio>

class Application
{
private:
	struct KeyInfo
	{
		std::string name;
		unsigned char key[32];
		unsigned char iv[32];
	};
	struct ClientInfo
	{
		int desc;
		struct sockaddr_in addr;
		unsigned char node[16];
		unsigned char mac[16];
	};

public:
	Application();
	~Application();

	bool initialize(unsigned int port);
	bool start(unsigned int maxClients);
	void run();
	void stop();
	void shutdown();

	void setKeyfileDirectory(const char* path);
	const char* getKeyfileDirectory() const;
	void setBanlistFile(const char* path);
	const char* getBanlistFile() const;
	void setReloadInterval(unsigned int interval);
	unsigned int getReloadInterval() const;

	void reload();

private:
	char keydir[
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		MAX_PATH + 1
#else
		PATH_MAX + 1
#endif
	];
	char banfile[
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		MAX_PATH + 1
#else
		PATH_MAX + 1
#endif
	];
	unsigned int interval;
	unsigned int maxclients;

	std::set<std::string> hwid;
	std::set<unsigned int> ip;
	std::map<std::string, std::vector<std::string> > node;

	std::vector<KeyInfo> keys;

	time_t reloaded;

	int desc;
	struct sockaddr_in addr;
	std::vector<ClientInfo> clients;
};