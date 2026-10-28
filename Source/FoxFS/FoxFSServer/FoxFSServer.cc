#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "Application.h"

int main(int argc, char** argv)
{
	unsigned int uPort = 6077;
	unsigned int uMaxClients = 128;
	unsigned int uReloadInterval = 3600;
	const char* directory = "./keyfiles";
	const char* banfile = "./banlist.txt";

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
		{
			std::cout << "Usage: " << argv[0] << " <parameters>" << std::endl;
			std::cout << "\t-p, --port\tSpecify the port you want to use [default: 6077]" << std::endl;
			std::cout << "\t-m, --max\tSpecify the max clients that can be connected to the server at one time [default: 128]" << std::endl;
			std::cout << "\t-d, --dir\tSpecify the keyfile directory [default: ./keyfiles]" << std::endl;
			std::cout << "\t-b, --ban\tSpecify the banlist file [default: ./banlist.txt]" << std::endl;
			std::cout << "\t-r, --reload\tSpecify the interval to reload the keyfiles and banlist [default: 3600]" << std::endl;
		}
		else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--port"))
		{
			if (++i < argc)
			{
				uPort = atoi(argv[i]);
			}
			else
			{
				std::cerr << "Missing parameter!" << std::endl;
				std::cerr << "Type \"" << argv[0] << " --help\" for more information!" << std::endl;
				return 0;
			}
		}
		else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--max"))
		{
			if (++i < argc)
			{
				uMaxClients = atoi(argv[i]);
			}
			else
			{
				std::cerr << "Missing parameter!" << std::endl;
				std::cerr << "Type \"" << argv[0] << " --help\" for more information!" << std::endl;
				return 0;
			}
		}
		else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dir"))
		{
			if (++i < argc)
			{
				directory = argv[i];
			}
			else
			{
				std::cerr << "Missing parameter!" << std::endl;
				std::cerr << "Type \"" << argv[0] << " --help\" for more information!" << std::endl;
				return 0;
			}
		}
		else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--ban"))
		{
			if (++i < argc)
			{
				banfile = argv[i];
			}
			else
			{
				std::cerr << "Missing parameter!" << std::endl;
				std::cerr << "Type \"" << argv[0] << " --help\" for more information!" << std::endl;
				return 0;
			}
		}
		else if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--reload"))
		{
			if (++i < argc)
			{
				uReloadInterval = atoi(argv[i]);
			}
			else
			{
				std::cerr << "Missing parameter!" << std::endl;
				std::cerr << "Type \"" << argv[0] << " --help\" for more information!" << std::endl;
				return 0;
			}
		}
		else
		{
			std::cerr << "Unknown command!" << std::endl;
			std::cerr << "Type \"" << argv[0] << " --help\" for more information!" << std::endl;
		}
	}

	Application app;
	if (app.initialize(uPort))
	{
		app.setKeyfileDirectory(directory);
		app.setBanlistFile(banfile);
		app.setReloadInterval(uReloadInterval);
		if (app.start(uMaxClients))
		{
			app.run();
		}
		else
		{
			std::cerr << "Cannot start FoxFS KeyServer!" << std::endl;
		}
	}
	else
	{
		std::cerr << "Cannot initialize FoxFS KeyServer!" << std::endl;
	}

	return 0;
}