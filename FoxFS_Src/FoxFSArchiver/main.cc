#include <iomanip>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#include <windows.h>


#else
#include <dirent.h>
#endif

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include "ArchiveWriter.h"
#include "../FoxFS/config.h"

#include "../lz4/lz4.h"
#include "../lz4/lz4hc.h"
#include "../xxhash/xxhash.h"

#include <tbb/tbb.h>
#include <tbb/task_scheduler_init.h>

#define FILE_COMPRESSOR_MEM (1024 * 1024 * 64)

inline unsigned int MIN(unsigned int a, unsigned int b) { return (a > b) ? b : a; }
inline unsigned int MAX(unsigned int a, unsigned int b) { return (a > b) ? a : b; }

inline bool wildcard(const char* text, const char* pattern)
{
	const char* cp = 0;
	const char* mp = 0;

	while ((*text) && (*pattern != '*'))
	{
		if ((*pattern != *text) && (*pattern != '?'))
		{
			return false;
		}
		++pattern;
		++text;
	}

	while (*text)
	{
		if (*pattern == '*')
		{
			if (!*++pattern)
			{
				return true;
			}
			mp = pattern;
			cp = text + 1;
		}
		else if ((*pattern == *text) || (*pattern == '?'))
		{
			++pattern;
			++text;
		}
		else
		{
			pattern = mp;
			text = cp++;
		}
	}

	while (*pattern == '*')
	{
		++pattern;
	}

	return !*pattern;
}

struct FileCompressor
{
	std::vector<unsigned char> data;
	std::vector<unsigned char> src;
	unsigned int compressed;
	unsigned int decompressed;
	unsigned int hash;

	FileCompressor()
	{
		data.resize(FILE_COMPRESSOR_MEM);
		src.resize(FILE_COMPRESSOR_MEM);
	}
	bool operator()(const std::string& source)
	{
		compressed = 0;
		decompressed = 0;
		hash = 0;

		std::ifstream input(source.c_str(), std::ifstream::binary | std::ifstream::in);
		bool r = false;
		if (input)
		{
			input.seekg(0, std::ifstream::end);
			decompressed = input.tellg();
			input.seekg(0, std::ifstream::beg);

			input.read(reinterpret_cast<char*>(&src[0]), MIN(src.size(), decompressed));
			input.close();

			hash = XXH32(reinterpret_cast<char*>(&src[0]), decompressed, FOXFS_MAGIC);

			if ((compressed = LZ4_compressHC(reinterpret_cast<char*>(&src[0]), reinterpret_cast<char*>(&data[0]), decompressed)) >= decompressed)
			{
				memcpy(&data[0], &src[0], src.size());
				compressed = decompressed;
			}

			r = true;
		}
		return r;
	}
};
bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

struct XmlGenerator
{
	XmlGenerator() {}
	void operator()(const std::string& out, std::string input, std::string prefix, const std::string& archive, const std::vector<std::string>& ignores, const std::vector<std::pair<std::string, std::string> >& patches, const std::vector<std::string>& adds, bool kf, const std::string& kp)
	{
		if (prefix.length() && prefix[prefix.length() - 1] != '\\' && prefix[prefix.length() - 1] != '/')
		{
			prefix += "/";
			for (int i = 0; i < prefix.length(); ++i)
			{
				if (prefix[i] == '\\')
				{
					prefix[i] = '/';
				}
			}
		}
		if (input[input.length() - 1] != '\\' && input[input.length() - 1] != '/')
		{
			input += "/";
		}
		for (int i = 0; i < input.length(); ++i)
		{
			if (input[i] == '\\')
			{
				input[i] = '/';
			}
		}
		std::vector<std::string> files = parse(input);
		std::vector<std::string> filesAdd;
		for (std::vector<std::string>::iterator iter = files.begin(); iter != files.end(); ++iter)
		{
			for (std::vector<std::string>::const_iterator adn = adds.begin(); adn != adds.end(); ++adn)
			{
				if (wildcard((*iter).c_str(), (*adn).c_str()))
				{
					filesAdd.push_back((*iter).c_str());
					break;
				}
			}

			for (std::vector<std::string>::const_iterator ign = ignores.begin(); ign != ignores.end(); ++ign)
			{
				if (wildcard((*iter).c_str(), (*ign).c_str()))
				{
					files.erase(iter--);
					break;
				}
			}

		}
		files.insert(files.end(), filesAdd.begin(), filesAdd.end());

		std::ofstream o(out.c_str(), std::ofstream::out);
		if (o)
		{
			o << "<ScriptFile>" << std::endl;
			o << "\t<CreateEterPack ArchivePath=\"" << archive << "\" BuildKeyfile=\"" << (kf ? "true" : "false") << "\" ";
			if (kf)
			{
				o << "KeyPath=\"" << kp << "\" ";
			}
			o << ">" << std::endl;
			for (std::vector<std::string>::iterator iter = files.begin(); iter != files.end(); ++iter)
			{
				std::string fn = *iter;
				if (fn.length() && fn[0] == '\\' || fn[0] == '/')
				{
					fn.erase(0, 1);
				}

				for (std::vector<std::pair<std::string, std::string> >::const_iterator pt = patches.begin(); pt != patches.end(); ++pt)
				{
					replace(fn, (*pt).first, (*pt).second);
					break;
				}

				o << "\t\t<File ArchivedPath=\"" << (prefix + fn) << "\"><![CDATA[" << (input + *iter) << "]]></File>" << std::endl;
			}
			o << "\t</CreateEterPack>" << std::endl;
			o << "</ScriptFile>";
			o.close();
		}
	}

	std::vector<std::string> parse(const std::string& basepath)
	{
		std::vector<std::string> result;
		std::stack<std::string> directories;
		directories.push(basepath);

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		HANDLE find = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATAA ffd;
#else
		struct dirent* entry;
		DIR* dp;
#endif
		while (!directories.empty())
		{
			std::string path = directories.top();
			directories.pop();
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
			std::string curpath = path + "\\*";
			find = FindFirstFileA(curpath.c_str(), &ffd);
			if (find == INVALID_HANDLE_VALUE)
			{
				break;
			}
			do
			{
				if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
				{
					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						directories.push(path + "\\" + ffd.cFileName);
					}
					else
					{
						std::string fname = path + "\\" + ffd.cFileName;
						fname.erase(0, basepath.length());
						if (fname[0] == '\\' || fname[0] == '/')
						{
							fname.erase(0, 1);
						}
						for (int i = 0; i < fname.length(); ++i)
						{
							if (fname[i] == '\\')
							{
								fname[i] = '/';
							}
						}
						result.push_back(fname);
					}
				}
			} while (FindNextFileA(find, &ffd) != 0);
			FindClose(find);
#else
			dp = opendir(path.c_str());
			if (dp == 0)
			{
				break;
			}

			while ((entry = readdir(dp)))
			{
				if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
				{
					if (entry->d_type == DT_DIR)
					{
						directories.push(path + "/" + entry->d_name);
					}
					else
					{
						std::string fname = path + "/" + entry->d_name;
						fname.erase(0, basepath.length());
						if (fname[0] == '\\' || fname[0] == '/')
						{
							fname.erase(0, 1);
						}
						result.push_back(fname);
					}
				}
			}
			closedir(dp);
#endif
		}
		return result;
	}
};
struct XmlParser
{
	std::vector<boost::property_tree::ptree>& xmls;
	int argc;
	char** argv;

	XmlParser(std::vector<boost::property_tree::ptree>& r, int c, char** v) : xmls(r), argc(c), argv(v) {}
	void operator()(const tbb::blocked_range<unsigned int>& r) const
	{
		for (unsigned int i = r.begin(); i != r.end(); ++i)
		{
			read_xml(argv[i], xmls[i - 1]);
		}
	}
};
struct XmlProcessor
{
	std::vector<boost::property_tree::ptree>& xmls;
	XmlProcessor(std::vector<boost::property_tree::ptree>& r) : xmls(r) {}
	void operator()(const tbb::blocked_range<unsigned int>& r) const
	{
		FileCompressor compressor;
		for (unsigned int i = r.begin(); i != r.end(); ++i)
		{
			BOOST_FOREACH(boost::property_tree::ptree::value_type& vt, xmls[i].get_child("ScriptFile"))
			{
				if (vt.first == "CreateEterPack")
				{
					ArchiveWriter writer;
					std::string path = vt.second.get<std::string>("<xmlattr>.ArchivePath");
					std::string keypath = path;
					keypath.append(".fsk");
					keypath = vt.second.get<std::string>("<xmlattr>.KeyPath", keypath);

					if (writer.create(path.c_str(), vt.second.get<bool>("<xmlattr>.BuildKeyfile", false) ? keypath.c_str() : 0))
					{
						BOOST_FOREACH(boost::property_tree::ptree::value_type& v, vt.second)
						{
							if (v.first != "File")
							{
								continue;
							}

							if (compressor(v.second.data()))
							{
								writer.add(v.second.get<std::string>("<xmlattr>.ArchivedPath").c_str(), compressor.decompressed, compressor.compressed, compressor.hash, &compressor.data[0]);
							}
							else
							{
								std::cerr << "Cannot compress " << v.second.data() << "!" << std::endl;
							}
						}
					}
				}
				else if (vt.first == "CreateEterPackXml") // <CreateEterPackXml Input="path:prefix" ArchivePath="output" XmlPath="xml output">
				{
					std::string input = vt.second.get<std::string>("<xmlattr>.Input");
					std::size_t separator = input.find(":");
					std::string source, prefix;
					if (separator != std::string::npos)
					{
						source = input.substr(0, separator);
						prefix = input.substr(separator + 1, std::string::npos);
					}
					else
					{
						source = input;
					}

					std::vector<std::string> ignores;
					ignores.reserve(vt.second.size());
					std::vector<std::string> adds;
					adds.reserve(vt.second.size());
					std::vector<std::pair<std::string, std::string> > patches;
					patches.reserve(vt.second.size());

					BOOST_FOREACH(boost::property_tree::ptree::value_type& j, vt.second)
					{
						if (j.first == "Ignore")
						{
							ignores.push_back(j.second.get<std::string>("<xmlattr>.Pattern")); // <Ignore Pattern="[a-zA-Z0-9]+.png" />
						}
						else if (j.first == "Add")
						{
							adds.push_back(j.second.get<std::string>("<xmlattr>.Pattern")); // <Add><![CDATA[Path]]></Add>
						}
						else if (j.first == "Patch")
						{
							patches.push_back(std::make_pair<std::string, std::string>(j.second.get<std::string>("<xmlattr>.Search"), j.second.get<std::string>("<xmlattr>.Replace"))); // <Add><![CDATA[Path]]></Add>
						}
					}

					XmlGenerator gen;
					gen(vt.second.get<std::string>("<xmlattr>.XmlPath"), source, prefix, vt.second.get<std::string>("<xmlattr>.ArchivePath"), ignores, patches, adds, vt.second.get<bool>("<xmlattr>.BuildKeyfile", false), vt.second.get<std::string>("<xmlattr>.KeyPath", ""));
				}
			}
		}
	}
};

int main(int argc, char** argv)
{
	std::cout << "--- © 2025 | BestStudio Production | Anka2 Project ---" << std::endl;
	if (argc > 5)
	{
		std::cout << "Wait..." << std::endl << std::endl;
	}
	else
	{
		std::cout << "Optimized & Deployed for TURKMMO Network" << std::endl << std::endl;
	}

	try
	{
		tbb::task_scheduler_init init(tbb::task_scheduler_init::default_num_threads());  // Explicit number of threads

		std::vector<boost::property_tree::ptree> xmls;
		xmls.resize(argc - 1);

		tbb::parallel_for(tbb::blocked_range<unsigned int>(1, argc), XmlParser(xmls, argc, argv));
		tbb::parallel_for(tbb::blocked_range<unsigned int>(0, xmls.size()), XmlProcessor(xmls));
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "An unknown exception was thrown!" << std::endl;
	}
	return 0;
}