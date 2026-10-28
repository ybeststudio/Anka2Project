///////////////////////////////////////////////////////////////////////
//	CFilename Class
//
//	(c) 2003 IDV, Inc.
//
//	*** INTERACTIVE DATA VISUALIZATION (IDV) PROPRIETARY INFORMATION ***
//
//	This software is supplied under the terms of a license agreement or
//	nondisclosure agreement with Interactive Data Visualization and may
//	not be copied or disclosed except in accordance with the terms of
//	that agreement.
//
//      Copyright (c) 2001-2003 IDV, Inc.
//      All Rights Reserved.
//
//		IDV, Inc.
//		1233 Washington St. Suite 610
//		Columbia, SC 29201
//		Voice: (803) 799-1699
//		Fax:   (803) 931-0320
//		Web:   http://www.idvinc.com
//

#pragma once
#include <string>
using std::string;
using namespace std::string_literals;

///////////////////////////////////////////////////////////////////////
//	CFilename Class

/*
#define CFilename CFileNameHelper
*/

class CFilename
{
public:
	CFilename() {}
	CFilename(const char* pFilename)
	{
		m_sRaw = pFilename;
	}
	CFilename(std::string strFilename)
	{
		m_sRaw = strFilename;
	}

	virtual ~CFilename() {}

	operator const string() const
	{
		return m_sRaw;
	}
	operator string& ()
	{
		return m_sRaw;
	}
	CFilename& operator = (const CFilename& r)
	{
		m_sRaw = r.m_sRaw;
		return *this;
	}
	bool operator == (const CFilename& r) const
	{
		return m_sRaw == r.m_sRaw;
	}
	CFilename operator + (const CFilename& r) const
	{
		return CFilename(m_sRaw + r.m_sRaw);
	}
	CFilename& operator += (const CFilename& r)
	{
		m_sRaw += r.m_sRaw;
		return *this;
	}
	const char& operator[] (size_t nIdx) const
	{
		return m_sRaw[nIdx];
	}
	const char* c_str() const
	{
		return m_sRaw.c_str();
	}
	size_t find(const char* pcszSrc) const
	{
		return m_sRaw.find(pcszSrc);
	}
	bool empty() const
	{
		return m_sRaw.empty();
	}
	size_t size() const
	{
		return m_sRaw.size();
	}
	size_t length() const
	{
		return m_sRaw.length();
	}

	string& GetString()
	{
		return m_sRaw;
	}

	void ChangeDosPath()
	{
		size_t nLength = m_sRaw.length();

		for (size_t i = 0; i < nLength; ++i)
		{
			if (m_sRaw.at(i) == '/')
			{
				m_sRaw.at(i) = '\\';
			}
		}
	}

	void StringPath()
	{
		size_t nLength = m_sRaw.length();

		for (size_t i = 0; i < nLength; ++i)
		{
			if (m_sRaw.at(i) == '\\')
			{
				m_sRaw.at(i) = '/';
			}
			else
			{
				m_sRaw.at(i) = (char)tolower(m_sRaw.at(i));
			}
		}
	}

	string GetName(void);          // if filename is "/idv/code/file.cpp", it returns "file"
	string GetExtension(void);     // if filename is "/idv/code/file.cpp", it returns "cpp"
	string GetPath(void);          // if filename is "/idv/code/file.cpp", it returns "/idv/code"
	string NoExtension(void);      // if filename is "/idv/code/file.cpp", it returns "/idv/code/file"
	string NoPath(void);           // if filename is "/idv/code/file.cpp", it returns "file.cpp"

	int compare(const char* s) const
	{
		return m_sRaw.compare(s);
	}
	friend CFilename operator + (const string alfa, const CFilename& beta);

	string m_sRaw;
};

inline CFilename operator + (const string alfa, const CFilename& beta)
{
	return beta + alfa;
}

inline string CFilename::GetName(void)				// if filename is "/idv/code/file.cpp", it returns "file"
{
	string strName;

	size_t nLength = m_sRaw.length();

	if (nLength > 0)
	{
		size_t iExtensionStartPos = nLength - 1;

		for (size_t i = nLength - 1; i > 0; i--)
		{
			if (m_sRaw[i] == '.')
			{
				iExtensionStartPos = i;
			}

			if (m_sRaw[i] == '/')
			{
				strName = string(m_sRaw.c_str() + i + 1);
				strName.resize(iExtensionStartPos - i - 1);
				break;
			}
		}
	}

	return strName;
}
inline string CFilename::GetExtension(void)			// if filename is "/idv/code/file.cpp", it returns "cpp"
{
	string strExtension;

	size_t nLength = m_sRaw.length();

	if (nLength > 0)
	{
		for (size_t i = nLength - 1; i > 0 && m_sRaw[i] != '/'; i--)
			if (m_sRaw[i] == '.')
			{
				strExtension = string(m_sRaw.c_str() + i + 1);
				break;
			}
	}

	return strExtension;
}
inline string CFilename::GetPath(void)				// if filename is "/idv/code/file.cpp", it returns "/idv/code"
{
	char szPath[1024];
	szPath[0] = '\0';

	size_t nLength = m_sRaw.length();

	if (nLength > 0)
	{
		for (size_t i = nLength - 1; i > 0; i--)
		{
			if (m_sRaw[i] == '/' || m_sRaw[i] == '\\')
			{
				for (size_t j = 0; j < i + 1; j++)
				{
					szPath[j] = m_sRaw[j];
				}
				szPath[i + 1] = '\0';
				break;
			}

			if (0 == i)
			{
				break;
			}
		}
	}
	return szPath;
}
inline string CFilename::NoExtension(void)			// if filename is "/idv/code/file.cpp", it returns "/idv/code/file"
{
	std::size_t npos = m_sRaw.find_last_of('.');

	if (string::npos != npos)
	{
		return std::string(m_sRaw, 0, npos);
	}

	return m_sRaw;
}
inline string CFilename::NoPath(void)					// if filename is "/idv/code/file.cpp", it returns "file.cpp"
{
	char szPath[1024];
	szPath[0] = '\0';

	size_t nLength = m_sRaw.length();

	if (nLength > 0)
	{
		strcpy(szPath, m_sRaw.c_str());

		for (size_t i = nLength - 1; i > 0; i--)
		{
			if (m_sRaw[i] == '/' || m_sRaw[i] == '\\')
			{
				int k = 0;
				for (size_t j = i + 1; j < nLength; j++, k++)
				{
					szPath[k] = m_sRaw[j];
				}
				szPath[k] = '\0';
				break;
			}

			if (0 == i)
			{
				break;
			}
		}
	}

	return szPath;
}
///////////////////////////////////////////////////////////////////////
//	CFileNameHelper Class

class CFileNameHelper
{
public:
	static void ChangeDosPath(string& str)
	{
		size_t nLength = str.length();

		for (size_t i = 0; i < nLength; ++i)
		{
			if (str.at(i) == '/')
			{
				str.at(i) = '\\';
			}
		}
	}

	static void StringPath(string& str)
	{
		size_t nLength = str.length();

		for (size_t i = 0; i < nLength; ++i)
		{
			if (str.at(i) == '\\')
			{
				str.at(i) = '/';
			}
			else
			{
				str.at(i) = (char)tolower(str.at(i));
			}
		}
	}

	static string GetName(const string& str);          // if filename is "/idv/code/file.cpp", it returns "file"
	static string GetExtension(const string& str);     // if filename is "/idv/code/file.cpp", it returns "cpp"
	static string GetPath(const string& str);          // if filename is "/idv/code/file.cpp", it returns "/idv/code"
	static string NoExtension(const string& str);      // if filename is "/idv/code/file.cpp", it returns "/idv/code/file"
	static string NoPath(const string& str);           // if filename is "/idv/code/file.cpp", it returns "file.cpp"
};

///////////////////////////////////////////////////////////////////////
//	CFileNameHelper::GetExtension

inline string CFileNameHelper::GetName(const string& str)
{
	string strName;

	size_t nLength = str.length();

	if (nLength > 0)
	{
		size_t iExtensionStartPos = nLength - 1;

		for (size_t i = nLength - 1; i > 0; i--)
		{
			if (str[i] == '.')
			{
				iExtensionStartPos = i;
			}

			if (str[i] == '/')
			{
				strName = string(str.c_str() + i + 1);
				strName.resize(iExtensionStartPos - i - 1);
				break;
			}
		}
	}

	return strName;
}

///////////////////////////////////////////////////////////////////////
//	CFilenameHelper::GetExtension

inline string CFileNameHelper::GetExtension(const string& str)
{
	string strExtension;

	size_t nLength = str.length();

	if (nLength > 0)
	{
		for (size_t i = nLength - 1; i > 0 && str[i] != '/'; i--)
			if (str[i] == '.')
			{
				strExtension = string(str.c_str() + i + 1);
				break;
			}
	}

	return strExtension;
}

///////////////////////////////////////////////////////////////////////
//	CFilenameHelper::GetPath

inline string CFileNameHelper::GetPath(const string& str)
{
	char szPath[1024];
	szPath[0] = '\0';

	size_t nLength = str.length();

	if (nLength > 0)
	{
		for (size_t i = nLength - 1; i > 0; i--)
		{
			if (str[i] == '/' || str[i] == '\\')
			{
				for (size_t j = 0; j < i + 1; j++)
				{
					szPath[j] = str[j];
				}
				szPath[i + 1] = '\0';
				break;
			}

			if (0 == i)
			{
				break;
			}
		}
	}
	return szPath;
}

///////////////////////////////////////////////////////////////////////
//	CFilenameHelper::NoExtension

inline string CFileNameHelper::NoExtension(const string& str)
{
	std::size_t npos = str.find_last_of('.');

	if (string::npos != npos)
	{
		return std::string(str, 0, npos);
	}

	return str;
}

///////////////////////////////////////////////////////////////////////
//	CFilenameHelper::NoPath

inline string CFileNameHelper::NoPath(const string& str)
{
	char szPath[1024];
	szPath[0] = '\0';

	size_t nLength = str.length();

	if (nLength > 0)
	{
		strcpy(szPath, str.c_str());

		for (size_t i = nLength - 1; i > 0; i--)
		{
			if (str[i] == '/' || str[i] == '\\')
			{
				int k = 0;
				for (size_t j = i + 1; j < nLength; j++, k++)
				{
					szPath[k] = str[j];
				}
				szPath[k] = '\0';
				break;
			}

			if (0 == i)
			{
				break;
			}
		}
	}

	return szPath;
}
