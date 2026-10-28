#include "Application.h"

Application::Application() {}
Application::~Application() { shutdown(); }

bool Application::initialize(unsigned int port)
{
	desc = socket(AF_INET, SOCK_STREAM, 0);
	if (desc == -1)
	{
		return false;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	if (bind(desc, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) != 0)
	{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		closesocket(desc);
#else
		::close(desc);
#endif
		desc = -1;
		return false;
	}
	return true;
}
bool Application::start(unsigned int maxClients)
{
	maxclients = maxClients;
	return listen(desc, maxclients) == 0;
}
void Application::run()
{
	bool banhammer = false;

	while (desc != -1)
	{
		int maxdesc = desc;
		fd_set fdRead, fdWrite, fdExcept;
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcept);

		FD_SET(desc, &fdRead);
		FD_SET(desc, &fdExcept);

		for (std::vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); ++iter)
		{
			if ((*iter).desc == -1)
			{
				clients.erase(iter--);
				continue;
			}

			FD_SET((*iter).desc, &fdRead);
			FD_SET((*iter).desc, &fdWrite);
			FD_SET((*iter).desc, &fdExcept);
			maxdesc = ((*iter).desc > maxdesc) ? maxdesc : (*iter).desc;
		}

		if (reloaded + interval < time(0))
		{
			reload();
		}

		int r = select(maxdesc + 1, &fdRead, &fdWrite, &fdExcept, 0);
		if (r == -1)
		{
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
			closesocket(desc);
#else
			close(desc);
#endif
			desc = -1;
		}
		else if (r > 0)
		{
			if (reloaded + interval < time(0))
			{
				reload();
			}

			for (std::vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); ++iter)
			{
				if ((*iter).desc == -1)
				{
					clients.erase(iter--);
					continue;
				}

				if (FD_ISSET((*iter).desc, &fdRead))
				{
					char buffer[8192] = { 0 };

					r = recv((*iter).desc, buffer, sizeof(buffer), 0);
					if (r <= 0)
					{
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
						closesocket((*iter).desc);
#else
						close((*iter).desc);
#endif	
						clients.erase(iter--);
					}
					else
					{
						unsigned short datalen = reinterpret_cast<unsigned short*>(buffer)[0] - 2;
						unsigned char* data = reinterpret_cast<unsigned char*>(&buffer[2]);

						unsigned int offset = 0;
						bool bSendExtendedInfo = false;
						char hexnode[33] = { 0 };
						char hexmac[33] = { 0 };

						while (offset < datalen)
						{
							if (data[offset] == 'M')
							{
								++offset;
								memcpy((*iter).mac, &data[offset], 16);
								offset += 16;

								for (int i = 0; i < 16; ++i)
								{
									sprintf(&hexmac[i * 2], "%02x", (*iter).mac[i]);
								}
								hexmac[32] = 0;

								std::cout << "Client Mac ID: " << hexmac << "#" << (*iter).desc << std::endl;
							}
							else if (data[offset] == 'N')
							{
								++offset;
								memcpy((*iter).node, &data[offset], 16);
								offset += 16;

								for (int i = 0; i < 16; ++i)
								{
									sprintf(&hexnode[i * 2], "%02x", (*iter).node[i]);
								}
								hexnode[32] = 0;

								std::cout << "Client Node ID: " << hexnode << "#" << (*iter).desc << std::endl;
							}
							else if (data[offset] == 'B')
							{
								banhammer = true;
							}
							else
							{
								std::cout << "Client sent unknown data! (#" << (*iter).desc << ")" << std::endl;
							}
						}

						std::set<std::string>::iterator hwiter = hwid.find(hexmac);
						std::map<std::string, std::vector<std::string> >::iterator niter = node.find(hexnode);

						if (hwiter != hwid.end())
						{
							std::cout << "Client is banned by HWID! " << (*hwiter) << "#" << (*iter).desc << std::endl;
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
							::closesocket((*iter).desc);
#else
							::close((*iter).desc);
#endif
							(*iter).desc = -1;
						}
						else
						{
							if (niter != node.end())
							{
								std::cout << "Suspicious client! " << hexnode << "#" << (*iter).desc << std::endl;
							}

							unsigned short* length = reinterpret_cast<unsigned short*>(buffer);
							unsigned char* data = reinterpret_cast<unsigned char*>(buffer) + 2;

							*length = 2;

							for (std::vector<KeyInfo>::iterator kiter = keys.begin(); kiter != keys.end() && *length < 8192; ++kiter)
							{
								memcpy(&buffer[*length], (*kiter).name.c_str(), (*kiter).name.length() + 1);
								*length += (*kiter).name.length() + 1;
								memcpy(&buffer[*length], (*kiter).key, 32);
								memcpy(&buffer[*length + 32], (*kiter).iv, 32);
								*length += 64;
							}

							if (banhammer)
							{
								r = send((*iter).desc, buffer, *length, 0);
							}
							else
							{
								std::cout << "Client is banned by HWID! " << (*hwiter) << "#" << (*iter).desc << std::endl;
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
								::closesocket((*iter).desc);
#else
								::close((*iter).desc);
#endif
								(*iter).desc = -1;
							}
						}
					}
				}
				if ((*iter).desc != -1 && FD_ISSET((*iter).desc, &fdExcept))
				{
					int val = 0;
					int len = sizeof(val);
					r = getsockopt(desc, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&val), &len);

					std::cerr << "Socket Error on " << desc << " (Client Desc): " << val << std::endl;

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
					::closesocket((*iter).desc);
#else
					::close((*iter).desc);
#endif
					(*iter).desc = -1;
				}
			}

			if (FD_ISSET(desc, &fdRead))
			{
				ClientInfo info;
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
				int len = sizeof(info.addr);
#else
				socklen_t len = sizeof(info.addr);
#endif
				info.desc = accept(desc, reinterpret_cast<struct sockaddr*>(&info.addr), &len);
				if (info.desc != -1)
				{
					clients.push_back(info);
				}
			}
			if (FD_ISSET(desc, &fdExcept))
			{
				int val = 0;
				int len = sizeof(val);
				r = getsockopt(desc, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&val), &len);

				std::cerr << "Socket Error on " << desc << " (Server Desc): " << val << std::endl;

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
				::closesocket(desc);
#else
				::close(desc);
#endif
				desc = -1;
			}
		}
	}
}
void Application::stop()
{

}
void Application::shutdown()
{
	stop();
	keys.clear();
	hwid.clear();
	node.clear();
	ip.clear();
}

void Application::setKeyfileDirectory(const char* path) { strcpy(keydir, path); }
const char* Application::getKeyfileDirectory() const { return keydir; }
void Application::setBanlistFile(const char* path) { strcpy(banfile, path); }
const char* Application::getBanlistFile() const { return banfile; }
void Application::setReloadInterval(unsigned int interval) { interval = interval; }
unsigned int Application::getReloadInterval() const { return interval; }

void Application::reload()
{
	keys.clear();
	std::string path = keydir;
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
	HANDLE find = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA ffd;

	path.append("/*");

	find = FindFirstFileA(path.c_str(), &ffd);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0 && !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string name = ffd.cFileName;
				std::size_t pos = name.find(".");
				if (pos != std::string::npos)
				{
					name.erase(pos, std::string::npos);
				}
				transform(name.begin(), name.end(), name.begin(), tolower);

				std::string filepath = path + "/" + ffd.cFileName;
				std::ifstream in(filepath.c_str(), std::ifstream::in | std::ifstream::binary);
				if (in)
				{
					KeyInfo info;
					info.name = name;
					in.read(reinterpret_cast<char*>(info.key), 32);
					in.read(reinterpret_cast<char*>(info.iv), 32);
					in.close();
					keys.push_back(info);
				}
			}
		} while (FindNextFileA(find, &ffd) != 0);
		FindClose(find);
	}
	else
	{
		std::cerr << "Cannot open keyfile directory: " << keydir << "!" << std::endl;
	}
#else
	DIR* dp = opendir(keydir);
	struct dirent* entry = 0;

	if (dp != 0)
	{
		while ((entry = readdir(dp)))
		{
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && !(entry->d_type == DT_DIR))
			{
				std::string name = entry->d_name;
				std::size_t pos = name.find(".");
				if (pos != std::string::npos)
				{
					name.erase(pos, std::string::npos);
				}
				transform(name.begin(), name.end(), name.begin(), tolower);

				std::string filepath = path;
				filepath += "/";
				filepath += entry->d_name;
				std::ifstream in(filepath.c_str(), std::ifstream::in | std::ifstream::binary);
				if (in)
				{
					KeyInfo info;
					info.name = name;
					in.read(reinterpret_cast<char*>(info.key), 32);
					in.read(reinterpret_cast<char*>(info.iv), 32);
					in.close();
					keys.push_back(info);
				}
			}
		}
		closedir(dp);
	}
	else
	{
		std::cerr << "Cannot open keyfile directory: " << keydir << "!" << std::endl;
	}
#endif

	std::ifstream banin(banfile, std::ifstream::in);
	if (!banin)
	{
		std::cerr << "Cannot open banlist: " << banfile << "!" << std::endl;
	}
	else
	{
		hwid.clear();
		node.clear();
		ip.clear();

		std::string line;
		while (std::getline(banin, line))
		{
			std::string mode, addr;
			std::size_t pos = line.find(":");
			if (pos == std::string::npos)
			{
				continue;
			}

			transform(line.begin(), line.end(), line.begin(), tolower);
			mode = line.substr(0, pos);
			addr = line.substr(pos + 1, std::string::npos);

			if (mode == "ip")
			{
				ip.insert(inet_addr(addr.c_str()));
			}
			else if (mode == "hw")
			{
				pos = addr.find("+");
				if (pos != std::string::npos)
				{
					std::string nd = addr.substr(0, pos);
					std::string hw = addr.substr(pos + 1, std::string::npos);

					hwid.insert(hw);
					std::map<std::string, std::vector<std::string> >::iterator iter = node.find(nd);
					if (iter == node.end())
					{
						std::vector<std::string> v(1);
						v.push_back(hw);
						node.insert(std::map<std::string, std::vector<std::string> >::value_type(nd, v));
					}
					else
					{
						iter->second.push_back(hw);
					}
				}
				else
				{
					hwid.insert(addr);
				}
			}
		}
		banin.close();
	}

	reloaded = time(0);
}