#include "stdafx.h"
#include "CsvReader.h"
#include <fstream>
#include <algorithm>

#ifndef Assert
#include <assert.h>
#define Assert assert
#define LogToFile (void)(0);
#endif

namespace
{
	enum ParseState
	{
		STATE_NORMAL = 0,
		STATE_QUOTE
	};

	std::string Trim(std::string str)
	{
		str = str.erase(str.find_last_not_of(" \t\r\n") + 1);
		str = str.erase(0, str.find_first_not_of(" \t\r\n"));
		return str;
	}

	std::string Lower(std::string original)
	{
		std::transform(original.begin(), original.end(), original.begin(), tolower);
		return original;
	}
}

void cCsvAlias::AddAlias(const char* name, size_t index)
{
	std::string converted(Lower(name));

	Assert(m_Name2Index.find(converted) == m_Name2Index.end());
	Assert(m_Index2Name.find(index) == m_Index2Name.end());

	m_Name2Index.insert(NAME2INDEX_MAP::value_type(converted, index));
	m_Index2Name.insert(INDEX2NAME_MAP::value_type(index, name));
}

void cCsvAlias::Destroy()
{
	m_Name2Index.clear();
	m_Index2Name.clear();
}

const char* cCsvAlias::operator [] (size_t index) const
{
	INDEX2NAME_MAP::const_iterator itr(m_Index2Name.find(index));
	if (itr == m_Index2Name.end())
	{
		LogToFile(NULL, "cannot find suitable conversion for %d", index);
		Assert(false && "cannot find suitable conversion");
		return NULL;
	}

	return itr->second.c_str();
}

size_t cCsvAlias::operator [] (const char* name) const
{
	NAME2INDEX_MAP::const_iterator itr(m_Name2Index.find(Lower(name)));
	if (itr == m_Name2Index.end())
	{
		LogToFile(NULL, "cannot find suitable conversion for %s", name);
		Assert(false && "cannot find suitable conversion");
		return 0;
	}

	return itr->second;
}

bool cCsvFile::Load(const char* fileName, const char seperator, const char quote)
{
	Assert(seperator != quote);

	std::ifstream file(fileName, std::ios::in);
	if (!file) return false;

	Destroy();

	cCsvRow* row = NULL;
	ParseState state = STATE_NORMAL;
	std::string token = "";
	char buf[2048 + 1] = { 0, };

	while (file.good())
	{
		file.getline(buf, 2048);
		buf[sizeof(buf) - 1] = 0;

		std::string line(Trim(buf));
		if (line.empty() || (state == STATE_NORMAL && line[0] == '#')) continue;

		std::string text = std::string(line) + "  ";
		size_t cur = 0;

		while (cur < text.size())
		{
			if (state == STATE_QUOTE)
			{
				if (text[cur] == quote)
				{
					if (text[cur + 1] == quote)
					{
						token += quote;
						++cur;
					}
					else
					{
						state = STATE_NORMAL;
					}
				}
				else
				{
					token += text[cur];
				}
			}
			else if (state == STATE_NORMAL)
			{
				if (row == NULL)
					row = new cCsvRow();

				if (text[cur] == seperator)
				{
					row->push_back(token);
					token.clear();
				}
				else if (text[cur] == quote)
				{
					state = STATE_QUOTE;
				}
				else
				{
					token += text[cur];
				}
			}

			++cur;
		}

		if (state == STATE_NORMAL)
		{
			Assert(row != NULL);
			row->push_back(token.substr(0, token.size() - 2));
			m_Rows.push_back(row);
			token.clear();
			row = NULL;
		}
		else
		{
			token = token.substr(0, token.size() - 2) + "\r\n";
		}
	}

	return true;
}

bool cCsvFile::Save(const char* fileName, bool append, char seperator, char quote) const
{
	Assert(seperator != quote);

	std::ofstream file;
	if (append) { file.open(fileName, std::ios::out | std::ios::app); }
	else { file.open(fileName, std::ios::out | std::ios::trunc); }

	if (!file) return false;

	char special_chars[5] = { seperator, quote, '\r', '\n', 0 };
	char quote_escape_string[3] = { quote, quote, 0 };

	for (size_t i = 0; i < m_Rows.size(); i++)
	{
		const cCsvRow& row = *((*this)[i]);

		std::string line;

		for (size_t j = 0; j < row.size(); j++)
		{
			const std::string& token = row[j];

			if (token.find_first_of(special_chars) == std::string::npos)
			{
				line += token;
			}
			else
			{
				line += quote;

				for (size_t k = 0; k < token.size(); k++)
				{
					if (token[k] == quote) line += quote_escape_string;
					else line += token[k];
				}

				line += quote;
			}

			if (j != row.size() - 1) { line += seperator; }
		}

		file << line << std::endl;
	}

	return true;
}

void cCsvFile::Destroy()
{
	for (ROWS::iterator itr(m_Rows.begin()); itr != m_Rows.end(); ++itr)
		delete* itr;

	m_Rows.clear();
}

cCsvRow* cCsvFile::operator [] (size_t index)
{
	Assert(index < m_Rows.size());
	return m_Rows[index];
}

const cCsvRow* cCsvFile::operator [] (size_t index) const
{
	Assert(index < m_Rows.size());
	return m_Rows[index];
}

cCsvTable::cCsvTable()
	: m_CurRow(-1)
{
}

cCsvTable::~cCsvTable()
{
}

bool cCsvTable::Load(const char* fileName, const char seperator, const char quote)
{
	Destroy();
	return m_File.Load(fileName, seperator, quote);
}

bool cCsvTable::Next()
{
	return ++m_CurRow < (int)m_File.GetRowCount() ? true : false;
}

size_t cCsvTable::ColCount() const
{
	return CurRow()->size();
}

int cCsvTable::AsInt(size_t index) const
{
	const cCsvRow* const row = CurRow();
	if (!row || index >= row->size())
		return 0;
	return row->AsInt(index);
}

double cCsvTable::AsDouble(size_t index) const
{
	const cCsvRow* const row = CurRow();
	if (!row || index >= row->size())
		return 0.0;
	return row->AsDouble(index);
}

const char* cCsvTable::AsStringByIndex(size_t index) const
{
	const cCsvRow* const row = CurRow();
	if (!row || index >= row->size())
		return "";
	return row->AsString(index);
}

void cCsvTable::Destroy()
{
	m_File.Destroy();
	m_Alias.Destroy();
	m_CurRow = -1;
}

const cCsvRow* const cCsvTable::CurRow() const
{
	if (m_CurRow < 0)
	{
		Assert(false && "call Next() first!");
		return NULL;
	}
	else if (m_CurRow >= (int)m_File.GetRowCount())
	{
		Assert(false && "no more rows!");
		return NULL;
	}

	return m_File[m_CurRow];
}
